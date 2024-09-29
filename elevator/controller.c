/* Instruction:
- This is the elevator controller, compile by ./controller.
- This will bind port 3000 and listen on it with a TCP socket for clients to connect.
- Clients of the controller include:
      + Car (elevator car) : provide some information about themselves, then remain connected in order to provide status updates to the controller and to receive destinations.
      + Call pad (call pad): connect to the controller, provide the call pad's floor and destination, receive a dispatch notification and then disconnect. One connection will be made each time a user calls an elevator.
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h> 
#define BUFFER_SIZE 1023

/* These variables are just for initial testing, have to implement them in another way with multiples car*/
uint8_t car_available = 1; // Car availability flag
char *car_name = "MINIONS"; // Car name


int main(void) {
            
      /* client file descriptor */
      int clientfd;
      /* receive buffer */
      char buffer[BUFFER_SIZE];
      
      struct sockaddr clientaddr; // Client address
      socklen_t addrlen; 	// Client address length
      
      int fd = socket(AF_INET, SOCK_STREAM, 0); // Create a socket - 0 is default protocol (TCP) - returns a file descriptor
      if (fd == -1) {
            perror("Error creating socket");
            return 1;
      }
      struct sockaddr_in serverAddress; // Server address structure
      memset(&serverAddress, 0, sizeof(serverAddress)); //Memory set to 0 - clear out the struct
      serverAddress.sin_family = AF_INET; // Set the address family to AF_INET
      //serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); // Set the server IP address
      serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); // Set the server IP address to localhost
      serverAddress.sin_port = htons(3000); // Port number

      
      /* Assign a name to the socket created - Implement bind() system call */
      if (bind(fd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1) { // Bind the socket to the address
            perror("Error binding socket");
            return 1;
      }
      
      /* Place server in passive mode - listen for incomming client request */
      if (listen(fd, 10)==-1) {
            perror("listen()");
            return 1;
      }
      
      /* infinite loop */
      while (1) {
            
            /*  Generate a new socket for data transfer with the client */
            clientfd  = accept(fd, &clientaddr, &addrlen); // Accept the connection from the client - returns a new file descriptor
            if (clientfd == -1) { // Check for errors
                  perror("Error accepting connection");
                  return 1;
            }
            /* receive the incomming client data */
            int bytesReceived = recv(clientfd, buffer, BUFFER_SIZE, 0); // Receive data from the client using the client descriptor - returns the number of bytes received
            // Also store the data in the 'buffer' array
            if (bytesReceived == -1) { // Check for errors
                  perror("Error receiving data");
                  return 1;
            }
            /* add null terminator to received data and print out message */
            buffer[bytesReceived] = '\0'; // Add a null terminator to the end of the received data
            printf("Received: %s\n", buffer); // Print out the received data
            // The recieved data will be in the format "CAR <source_floor> <destination_floor>"
            char *token = strtok(buffer, " "); // Get the first token (CAR)
            char *source_floor = strtok(NULL, " "); // Get the second token (source floor)
            char *destination_floor = strtok(NULL, " "); // Get the third token (destination floor)
            printf("Source Floor: %s\n", source_floor); // Print out the source floor
            printf("Destination Floor: %s\n", destination_floor); // Print out the destination floor
            if (car_available) { // Check if the car is available
                  car_available = 0; // Set the car availability flag to 0
                  // Send a message to the car to dispatch it to the source floor
                  snprintf(buffer, BUFFER_SIZE, "CAR %s", car_name); // Store the message in the buffer
                  if (send(clientfd, buffer, strlen(buffer), 0) == -1) { // Send the message to the car
                        perror("Error sending data");
                        close(clientfd); // Close the client file descriptor
                        return 1;
                  }
            } else { // If there is no car (elevator) available
                  // Send a message to the car to queue the destination floor
                  snprintf(buffer, BUFFER_SIZE, "UNAVAILABLE"); // Store the message in the buffer
                  if (send(clientfd, buffer, strlen(buffer), 0) == -1) { // Send the message to the car
                        perror("Error sending data");
                        close(clientfd); // Close the client file descriptor
                        return 1;
                  }
            }
            /* close the socket used to receive data */
            close(clientfd); // Close the client file descriptor
            
      } // end while
      
      /* shutdown the connection - end communication to and from the socket SHUT_RDWR */
      if (shutdown(fd, SHUT_RDWR) == -1) { // Shutdown the socket
            perror("Error shutting down socket");
            return 1;
      }
      /* close the socket */
            /* sockets can remain open after program termination - close listening socket*/
      
                  
}


int isValidFloor(const T)