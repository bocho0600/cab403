#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1023

int main(int argc, char **argv) {
    /* client file descriptor */
    int clientfd;
    /* receive buffer */
    char buffer[BUFFER_SIZE];

    struct sockaddr clientaddr;
    socklen_t addrlen;

    /* Check if the user has passed the port number */
    if (argc != 2) {
        printf("\nCorrect usage: a.out <port number>\n");
        return -1;
    }

    /* Create a TCP/IP socket */
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("\nsocket()\n");
        return 1;
    }

    /* Specify the socket address (IP Address + Port) */
    struct sockaddr_in serverAddress;
    memset(&serverAddress, '\0', sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(atoi(argv[1]));

    /* Bind the socket to the address */
    if (bind(fd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("bind()");
        return 1;
    }

    /* Place server in passive mode - listen for incoming client requests */
    if (listen(fd, 10) == -1) {
        perror("listen()");
        return 1;
    }

    /* infinite loop */
    while (1) {
        /* Generate a new socket for data transfer with the client */
        clientfd = accept(fd, &clientaddr, &addrlen);
        if (clientfd == -1) {
            perror("accept()");
            return 1;
        }

        /* Receive the incoming client data */
        int bytesRcv = recv(clientfd, buffer, BUFFER_SIZE, 0); // Store the message in buffer and return the number of bytes received
        if (bytesRcv == -1) {
            perror("recv()");
            return 1;
        }

        /* Add null terminator to received data and print out message */
        buffer[bytesRcv] = '\0';
        printf("Received from client: %s\n", buffer);

        /* Send a response back to the client */
        char response[] = "Hello from the server!";
        if (send(clientfd, response, strlen(response), 0) == -1) {
            perror("send()");
            return 1;
        }

        /* Close the socket used to receive data */
        close(clientfd);
    }

    /* Shutdown and close the listening socket */
    if (shutdown(fd, SHUT_RDWR) == -1) {
        perror("shutdown()");
        return 1;
    }

    close(fd);
    return 0;
}
