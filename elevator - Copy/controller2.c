#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include "func.h"

#define PORT 3000
#define BUFFER_SIZE 1024
#define MAX_CARS 100
#define MAX_FLOOR_COMMANDS 100  // Define maximum number of floor commands
#define COMMAND_LENGTH 5         // Define maximum length for each command

typedef struct {
    char name[50];
    int lowest_floor;
    int highest_floor;
    int current_floor;
    int direction;
    char status[50];
    int ready_flag;
    int recv_flag;
    char FloorRequest[MAX_FLOOR_COMMANDS][COMMAND_LENGTH]; 
    int commandCount;  // To keep track of how many commands are stored
} Car;

Car registeredCars[MAX_CARS]; // Array to store cars
pthread_mutex_t car_mutex; // Mutex for synchronizing access to registeredCars

int availableCarID = 101;
int prev_car_id;
int carCount = 0; // Number of registered cars
void printFloorRequests(int carID);

void resetCarData(int carID);
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
int registerCar(char* name, int lowest_floor, int highest_floor) {
    pthread_mutex_lock(&car_mutex); // Lock mutex before accessing shared resource
    strcpy(registeredCars[carCount].name, name); // Copy the name of the car
    registeredCars[carCount].lowest_floor = lowest_floor; // Set the lowest floor
    registeredCars[carCount].highest_floor = highest_floor; // Set the highest floor
    registeredCars[carCount].current_floor = 0; // Initialize the current floor of the car
    registeredCars[carCount].commandCount = 0; // Initialize command count
    carCount++; // Increment the car count
    pthread_mutex_unlock(&car_mutex); // Unlock mutex after access
    return carCount - 1;
}

int findAvailableCar(int source_floor, int destination_floor) {
    pthread_mutex_lock(&car_mutex); // Lock mutex before accessing shared resource
    for (int i = 0; i < carCount; i++) {
        // Check if both the source and destination floors are within the car's floor range
        if ((registeredCars[i].lowest_floor <= source_floor) && 
            (registeredCars[i].highest_floor >= source_floor) &&
            (registeredCars[i].lowest_floor <= destination_floor) &&
            (registeredCars[i].highest_floor >= destination_floor)) {
            pthread_mutex_unlock(&car_mutex); // Unlock before returning
            return i; // Return the first car that can service both floors
        }
    }
    pthread_mutex_unlock(&car_mutex); // Unlock before returning
    return 101; // No available car found
}

char** processRequestFloor(const char *sourceFloorStr, const char *destinationFloorStr) {
    int sourceFloor = convertFloortoINT(sourceFloorStr);
    int destinationFloor = convertFloortoINT(destinationFloorStr);

    // Allocate memory for the array of floor commands (2 elements)
    char** floorCommands = (char**)malloc(2 * sizeof(char*));

    // Allocate memory for the start and destination commands
    floorCommands[0] = (char*)malloc(5 * sizeof(char)); // Start floor command
    floorCommands[1] = (char*)malloc(5 * sizeof(char)); // Destination floor command

    // Determine if we are going up or down
    if (sourceFloor < destinationFloor) {
        // Going up
        sprintf(floorCommands[0], "U%d", sourceFloor);     // U1
        sprintf(floorCommands[1], "U%d", destinationFloor); // U3
    } else {
        // Going down
        sprintf(floorCommands[0], "D%d", sourceFloor);     // D3
        sprintf(floorCommands[1], "D%d", destinationFloor); // D1
    }

    return floorCommands; // Return the array of 2 commands
}

void *on_new_client(void *client_socket) {
    int sock = *(int *)client_socket;
    char response[BUFFER_SIZE];
    char msg[BUFFER_SIZE];
    free(client_socket);
    
    while(1) {
        bzero(response, sizeof(response));
        recvLooped(sock, response, sizeof(response));
        char *token = strtok(response, " ");
        
        if (token != NULL && strcmp(token, "STATUS") == 0) {
            char* status = strtok(NULL, " ");
            int current_floor = convertFloortoINT(strtok(NULL, " "));
            int destination_floor = convertFloortoINT(strtok(NULL, " "));

            pthread_mutex_lock(&car_mutex); // Lock mutex before accessing shared resource
            if (strcmp(status, "Between") == 0) {
                if (current_floor > destination_floor) { // if the car is going down
                    registeredCars[prev_car_id].current_floor = current_floor - 1; // decrement the current floor
                } else if (current_floor < destination_floor) {
                    current_floor = current_floor + 1; // increment the current floor
                }
                registeredCars[prev_car_id].current_floor = current_floor;
            } else if (strcmp(status, "Opening") == 0 || strcmp(status, "Closed") == 0) {
                if (current_floor == destination_floor) {
                    registeredCars[prev_car_id].current_floor = current_floor;
                    registeredCars[prev_car_id].ready_flag = 1;
                }
            }
            pthread_mutex_unlock(&car_mutex); // Unlock after accessing shared resource
        }

        // Wait until there is a command available for the availableCarID
        while ((registeredCars[availableCarID].commandCount <= 0) && registeredCars[availableCarID].ready_flag == 0) {}
      //printFloorRequests(availableCarID);
        bzero(msg, sizeof(msg));
        snprintf(msg, BUFFER_SIZE, "FLOOR %s", registeredCars[availableCarID].FloorRequest[0] + 1); 
        sendMessage(sock, msg); // Send the message to the socket server to announce the car
        
        pthread_mutex_lock(&car_mutex); // Lock before modifying shared resource
        for (int i = 1; i < registeredCars[availableCarID].commandCount; i++) {
            strncpy(registeredCars[availableCarID].FloorRequest[i - 1], registeredCars[availableCarID].FloorRequest[i], COMMAND_LENGTH - 1);
            registeredCars[availableCarID].FloorRequest[i - 1][COMMAND_LENGTH - 1] = '\0'; // Ensure null termination
        }

        // Decrement the command count since we’ve consumed one request
        registeredCars[availableCarID].commandCount--;
        registeredCars[availableCarID].ready_flag = 0;
        pthread_mutex_unlock(&car_mutex); // Unlock after modification
    }
    close(sock);
}




int main(){
      pthread_mutex_init(&car_mutex, NULL); // Initialize the mutex
      int server_fd, opt = 0; // Server file descriptor and option
      struct sockaddr_in address; // Server address structure 
      int addrlen = sizeof(address); // Address length

      // Create a socket
      if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            perror("socket failed");
            exit(EXIT_FAILURE);
      }

      // Reuse the address and port
      if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
            perror("setsockopt_SO_REUSEADDR");
            exit(EXIT_FAILURE);
      }
      if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1) {
            perror("setsockopt_SO_REUSEPORT");
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

      

      while(1){
            signal(SIGPIPE, SIG_IGN); // Ignore SIGPIPE signal
            int client_socket; // New socket file descriptor
            if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) { // Accept the connection from the client - returns a new file descriptor
                  perror("accept");
                  exit(EXIT_FAILURE);
            }

            char buffer[BUFFER_SIZE]; // prepare a buffer to store the message
            char response[BUFFER_SIZE]; // prepare a buffer to store the message
            // Receive the response from the server
            bzero(buffer, sizeof(buffer));
            recvLooped(client_socket, buffer, sizeof(buffer));

            // Split the message into tokens
            char *token = strtok(buffer, " ");

            f(token != NULL && strcmp(token, "CALL") == 0) { // Check if the command is CALL
                  char *source_floor_str = strtok(NULL, " "); // Get the source floor
                  char *destination_floor_str = strtok(NULL, " "); // Get the destination floor
                  char** commands = processRequestFloor(source_floor_str, destination_floor_str);
                  int source_floor= convertFloortoINT(source_floor_str); // Convert the source floor to an integer
                  int destination_floor = convertFloortoINT(destination_floor_str); // Convert the destination floor to an integer
                 // printf("Source floor: %d, Destination floor: %d\n", source_floor, destination_floor);
                  availableCarID = 101;
                  availableCarID = findAvailableCar(source_floor, destination_floor);
                  registeredCars[availableCarID].ready_flag = 0;
                  if (availableCarID == 101){
                        snprintf(buffer, BUFFER_SIZE, "UNAVAILABLE"); // No car available
                        sendMessage(client_socket, buffer); // Send the message to the socket server
                        free(commands);
                  } else {
                        snprintf(buffer, BUFFER_SIZE, "CAR %s", registeredCars[availableCarID].name);

                        sendMessage(client_socket, buffer);
                        registeredCars[availableCarID].current_floor = source_floor;
                        
                        for (int i = 0; i < 2; i++) {
                              if (registeredCars[availableCarID].commandCount < MAX_FLOOR_COMMANDS) {
                                    // Append commands[i] to the next available position in FloorRequest
                                    strncpy(registeredCars[availableCarID].FloorRequest[registeredCars[availableCarID].commandCount], commands[i], COMMAND_LENGTH - 1);
                                    registeredCars[availableCarID].FloorRequest[registeredCars[availableCarID].commandCount][COMMAND_LENGTH - 1] = '\0';
                                    
                                    // Increment commandCount to point to the next available position
                                    registeredCars[availableCarID].commandCount++;
                                    //printf("Command %d: %s\n", i + 1, commands[i]);
                              } else {
                                    printf("Warning: FloorRequest capacity exceeded for car %s\n", registeredCars[availableCarID].name);
                              }
                        }
                        registeredCars[availableCarID].recv_flag = 1;
                  }

            } else if (token != NULL && strcmp(token, "CAR") == 0){ // Register a new car
  
                  char *car_name = strtok(NULL, " "); // Get the car name
                  char *lowest_floor_str = strtok(NULL, " "); // Get the lowest floor
                  char *highest_floor_str = strtok(NULL, " "); // Get the highest floor
                  int lowest_floor = convertFloortoINT(lowest_floor_str); // Convert the lowest floor to an integer
                  int highest_floor = convertFloortoINT(highest_floor_str); // Convert the highest floor to an integer
                 // resetCarData(prev_car_id); // Reset the car data
                  prev_car_id = registerCar(car_name, lowest_floor, highest_floor); // Register the car
                  pthread_t thread_id;
                  int *client_sock = malloc(sizeof(int));
                  *client_sock = client_socket; // Allocate memory for client socket
                  if (pthread_create(&thread_id, NULL, on_new_client, (void *)client_sock) != 0) {
                        perror("Failed to create thread");
                        free(client_sock); // Free memory on error
                        continue; // Continue to the next iteration
                  }
                  pthread_detach(thread_id); // Detach the thread to handle cleanup automaticall

            }
      }
      pthread_mutex_destroy(&car_mutex); // Destroy the mutex
      //close(server_fd);
      for (int i = 0; i < carCount; i++) {
            resetCarData(i); // Reset all car data
      }
      
      return 0;
}





void printFloorRequests(int carID) {
    // Lock the mutex to prevent concurrent access
    pthread_mutex_lock(&car_mutex);

    printf("total %d Floor requests for car %s :\n",registeredCars[carID].commandCount,registeredCars[carID].name);
    fflush(stdout);
    for (int i = 0; i < registeredCars[carID].commandCount; i++) {
        printf("Request %d: %s ", i + 1, registeredCars[carID].FloorRequest[i]);
      fflush(stdout);
    }

    // Unlock the mutex after accessing the shared resource
    pthread_mutex_unlock(&car_mutex);
}


void resetCarData(int carID) {
    if (carID >= 0 && carID < MAX_CARS) { // Ensure carID is within valid range
        memset(&registeredCars[carID], 0, sizeof(registeredCars[carID])); // Clear all fields to zero
    } else {
        printf("Invalid car ID: %d\n", carID);
    }
}