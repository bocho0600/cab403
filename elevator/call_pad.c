#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h> // for isdigit
#include <stdint.h> // for uint32_t

#define BUFFER_SIZE 100

// Function prototypes
int isValidFloor(char *floor);
void sendMessage(int fd, const char *msg);
void recvLooped(int fd, char *buf, size_t sz);

int main(int argc, char **argv) {
    // Check for valid arguments
    if (argc != 3) {
        printf("Usage: %s <source_floor> <destination_floor>\n", argv[0]);
        return 1;
    }

    char *source_floor = argv[1];
    char *destination_floor = argv[2];

    // Validate source and destination floors
    if (!isValidFloor(source_floor) || !isValidFloor(destination_floor)) {
        printf("Invalid floor(s) specified.\n");
        return 1;
    }

    // Prevent calling the elevator to the same floor
    if (strcmp(source_floor, destination_floor) == 0) {
        printf("You are already on that floor!\n");
        return 1;
    }

    // Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error creating socket");
        return 1;
    }

    const char *serverName = "127.0.0.1"; // localhost
    // Set up server address structure
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(3000); // Connect to port 3000

    // Convert IP address from text to binary form
    if (inet_pton(AF_INET, serverName, &serverAddress.sin_addr) <= 0) {
        perror("Invalid address or Address not supported");
        close(sockfd);
        return 1;
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        printf("Unable to connect to elevator system.\n");
        close(sockfd);
        return 1;
    }

    // Prepare the message to send
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "CALL %s %s", source_floor, destination_floor);
    
    // Send the message to the server
    //bzero(buffer, sizeof(buffer)); // Clear the buffer
    //printf("%s\n", buffer);
    sendMessage(sockfd, buffer);
    
    // Receive the response from the server
    bzero(buffer, sizeof(buffer));
    recvLooped(sockfd, buffer, sizeof(buffer));
    
    // Process the received message
    //printf("Received response: %s\n", buffer);
    char *token = strtok(buffer, " ");
    if (strcmp(token, "UNAVAILABLE") == 0) {
        printf("Sorry, no car is available to take this request.\n");
    } else if (strcmp(token, "CAR") == 0) {
        char *car_name = strtok(NULL, " ");
        printf("Car %s is arriving.\n", car_name);
    }

    // Close the socket
    close(sockfd);
    return 0;
}

// Helper function to validate a floor string
int isValidFloor(char *floor) {
    int len = strlen(floor);

    // Check for basement floors (B1-B99)
    if (floor[0] == 'B') {
        if (len == 2 && isdigit(floor[1])) { // B1 to B9
            return 1;
        } else if (len == 3 && isdigit(floor[1]) && isdigit(floor[2])) { // B10 to B99
            return 1;
        } else {
            return 0; // Invalid basement floor
        }
    }

    // Check for regular floors (1-999)
    for (int i = 0; i < len; i++) {
        if (!isdigit(floor[i])) {
            return 0; // Invalid floor format
        }
    }

    int floor_num = atoi(floor);
    if (floor_num >= 1 && floor_num <= 999) {
        return 1; // Valid floor
    }

    return 0; // Invalid floor
}

// Function to send a message to the server
void sendMessage(int fd, const char *msg) {
    int len = strlen(msg);
    uint32_t netLen = htonl(len); // Convert host byte order to network byte order

    // Send the length of the message
    if (send(fd, &netLen, sizeof(netLen), 0) == -1) {
        perror("Failed to send message length");
        exit(1);
    }

    // Send the actual message
    if (send(fd, msg, len, 0) != len) {
        fprintf(stderr, "send did not send all data\n");
        exit(1);
    }
}

// Function to receive a message from the server in a loop
void recvLooped(int fd, char *buf, size_t sz) {
    uint32_t netLen;

    // Receive the length of the incoming message
    if (recv(fd, &netLen, sizeof(netLen), 0) <= 0) {
        perror("Failed to receive message length");
        exit(1);
    }
    
    size_t len = ntohl(netLen); // Convert network byte order to host byte order

    // Ensure the message fits into the buffer
    if (len > sz) {
        fprintf(stderr, "Received message too large for buffer\n");
        exit(1);
    }

    // Receive the actual message
    ssize_t totalBytesReceived = 0;
    while (totalBytesReceived < len) {
        ssize_t bytesReceived = recv(fd, buf + totalBytesReceived, len - totalBytesReceived, 0);
        if (bytesReceived < 0) {
            perror("recv() failed");
            exit(1);
        }
        totalBytesReceived += bytesReceived;
    }

    buf[totalBytesReceived] = '\0'; // Null-terminate the received string
}
