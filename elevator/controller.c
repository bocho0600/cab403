#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#define PORT 3000
#define BUFFER_SIZE 1024
#include "func.h"
#define MAX_CARS 100
#include <pthread.h>

typedef struct {
      char name[50];
      int lowest_floor;
      int highest_floor;
      int current_floor;
      int direction;
      char status[50];
      //int FloorQueue[];
} Car;

Car registeredCars[MAX_CARS]; // Array to store cars
int carCount = 0; // Number of registered cars
// Convert floor string like "B10" or "15" to integer
int convertFloortoINT(const char *floorStr) {
    if (floorStr[0] == 'B') {
        return -atoi(&floorStr[1]); // Convert B10 to -10, B2 to -2, etc.
    } else {
        return atoi(floorStr); // Convert normal floors like "10" or "5"
    }
}

char* convertINTToFloor(int floor) {
    char *floorStr = (char*)malloc(10 * sizeof(char)); // Allocate memory for the floor string
    if (floorStr == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    if (floor < 0) {
        sprintf(floorStr, "B%d", -floor); // Convert -10 to "B10"
    } else {
        sprintf(floorStr, "%d", floor); // Convert 10 to "10"
    }

    return floorStr; // Return the dynamically allocated string
}

// Register a car with the controller
void registerCar(char* name, int lowest_floor, int highest_floor) {
      if (carCount >= MAX_CARS) {
            printf("Cannot register more cars\n");
            return;
      } else{
            strcpy(registeredCars[carCount].name, name); // Copy the name of the car
            registeredCars[carCount].lowest_floor = lowest_floor; // Set the lowest floor
            registeredCars[carCount].highest_floor = highest_floor; // Set the highest floor
            registeredCars[carCount].current_floor = 0; // Initiallize the current floor of the car
            carCount++; // Increment the car count
      }
}

Car* findAvailableCar(int source_floor, int destination_floor) {
    for (int i = 0; i < carCount; i++) {
        if (registeredCars[i].lowest_floor <= source_floor && registeredCars[i].highest_floor >= destination_floor) {
            return &registeredCars[i]; // Return the first car that can service both floors
        }
    }
    return NULL; // No available car found
}



void *on_new_client(void *client_socket) {
      int sock = *(int *)client_socket;
      char buffer[BUFFER_SIZE];
      char msg[BUFFER_SIZE];
      while(1){
            snprintf(buffer, BUFFER_SIZE, "FLOOR 1"); // Car is available
            sendMessage(sock, buffer); // Send the message to the socket server to announce the car
      }
      close(sock);
      return NULL;
      }

int main(){
      int server_fd, opt = 1; // Server file descriptor and option
      struct sockaddr_in address; // Server address structure 
      int addrlen = sizeof(address); // Address length

      // Create a socket
      if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            perror("socket failed");
            exit(EXIT_FAILURE);
      }

      // Reuse the address and port
      if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
            perror("setsockopt");
            exit(EXIT_FAILURE);
      }

      address.sin_family = AF_INET; // Set the address family to AF_INET
      address.sin_addr.s_addr = INADDR_ANY; // Set the server IP address
      address.sin_port = htons(PORT); // Port number 3000
      if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) { // Bind the socket to the address
            perror("bind failed");
            exit(EXIT_FAILURE);
      }

      if (listen(server_fd, 3) < 0) { // set the host to listen to incomming command
            perror("listen");
            exit(EXIT_FAILURE);
      }

      signal(SIGPIPE, SIG_IGN); // Ignore SIGPIPE signal
      int floorcall = 0;

      while(1){
            int client_socket; // New socket file descriptor
            if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) { // Accept the connection from the client - returns a new file descriptor
                  perror("accept");
                  exit(EXIT_FAILURE);
            }

            char buffer[BUFFER_SIZE]; // prepare a buffer to store the message
            char response[BUFFER_SIZE]; // prepare a buffer to store the message
            if (floorcall){
                  snprintf(buffer, BUFFER_SIZE,"FLOOR 1"); // Car is available
                  sendMessage(client_socket, buffer); // Send the message to the socket server to announce the car'
                  printf("test1");
                  fflush(stdout);
            }
            // Receive the response from the server
            bzero(buffer, sizeof(buffer));
            recvLooped(client_socket, buffer, sizeof(buffer));
            printf("Command sent to the controller: %s\n", buffer);

            // Split the message into tokens
            char *token = strtok(buffer, " ");

            if(token != NULL && strcmp(token, "CALL") == 0) { // Check if the command is CALL
                  char *lowest_floor_str = strtok(NULL, " "); // Get the source floor
                  char *highest_floor_str = strtok(NULL, " "); // Get the destination floor
                  int lowest_floor = convertFloortoINT(lowest_floor_str); // Convert the source floor to an integer
                  int highest_floor = convertFloortoINT(highest_floor_str); // Convert the destination floor to an integer
                  Car *availableCar = findAvailableCar(lowest_floor, highest_floor);
                  if (availableCar == NULL){
                        //int fd = connect_to_controller();
                        snprintf(buffer, BUFFER_SIZE, "UNAVAILABLE"); // No car available
                        sendMessage(client_socket, buffer); // Send the message to the socket server
                        
                  } else {
                  
                        snprintf(buffer, BUFFER_SIZE, "CAR %s", availableCar->name); // Car is available
                        sendMessage(client_socket, buffer); // Send the message to the socket server to announce the car'
                        if (availableCar->current_floor == 0){
                              pthread_t thread_id;
                              int *client_sock = malloc(sizeof(int));
                              *client_sock = client_socket; // Allocate memory for client socket
                              pthread_create(&thread_id, NULL, on_new_client, (void *)client_sock);
                              pthread_detach(thread_id); // Detach the thread to handle cleanup automaticall
                        }
      
                  }
            } else if (token != NULL && strcmp(token, "CAR") == 0){ // Register a new car

                  char *car_name = strtok(NULL, " "); // Get the car name
                  char *lowest_floor_str = strtok(NULL, " "); // Get the lowest floor
                  char *highest_floor_str = strtok(NULL, " "); // Get the highest floor
                  int lowest_floor = convertFloortoINT(lowest_floor_str); // Convert the lowest floor to an integer
                  int highest_floor = convertFloortoINT(highest_floor_str); // Convert the highest floor to an integer
                  registerCar(car_name, lowest_floor, highest_floor); // Register the car
            
            
            } else if (token != NULL && strcmp(token, "STATUS") == 0){
                  char *carStatus = strtok(NULL, " "); // Get the car status
                  int current_floor = convertFloortoINT(strtok(NULL, " ")); // Get the destination floor
                  int destination_floor = convertFloortoINT(strtok(NULL, " ")); // Get the highest floor 
                  if(strcmp(token, "Between") == 0){
                        if (current_floor > destination_floor){ // if the car is going down
                              current_floor = current_floor - 1; // decrement the current floor
                        } else if (current_floor < destination_floor){
                              current_floor = current_floor + 1; // increment the current floor
                        }
                  }
            }

      }
}