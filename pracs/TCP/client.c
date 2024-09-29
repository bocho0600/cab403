#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1023

int main(int argc, char **argv) {
    int sockfd; // Client file descriptor
    struct sockaddr_in serverAddress;
    char buffer[BUFFER_SIZE];
    
    // Check if server IP and port number are provided as arguments
    if (argc != 3) {
        printf("Usage: %s <server_ip> <port_number>\n", argv[0]);
        return -1;
    }

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error creating socket");
        return 1;
    }

    // Set up server address structure
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(atoi(argv[2])); // Port number

    // Convert IP address from text to binary form
    if (inet_pton(AF_INET, argv[1], &serverAddress.sin_addr) <= 0) {
        perror("Invalid address or Address not supported");
        return 1;
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Error connecting to server");
        close(sockfd);
        return 1;
    }

    // Prepare the message to send
    snprintf(buffer, BUFFER_SIZE, "Hello from the client!");

    // Send the message to the server
    if (send(sockfd, buffer, strlen(buffer), 0) == -1) {
        perror("Error sending data");
        close(sockfd);
        return 1;
    }

    // Receive the response from the server
    int bytesReceived = recv(sockfd, buffer, BUFFER_SIZE, 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        printf("Received from server: %s\n", buffer);
    } else if (bytesReceived == 0) {
        printf("Server closed the connection.\n");
    } else {
        perror("Error receiving data");
    }

    // Close the socket
    close(sockfd);

    return 0;
}
