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
	
	struct sockaddr clientaddr; // Client address
	socklen_t addrlen; 	// Client address length
	
	/* Check user has passed in the port number at the command line 
	 * local host will be used as the default address 
	*/
	if (argc != 2) {
		printf("\nCorrect usage: a.out <port number>\n");
		return -1;
	}
		
	/* Create a TCP/IP socket making sure to select appropriate family, communication type & 
	 * protocol
	 */
	int fd = socket(AF_INET, SOCK_STREAM, 0); // Create a socket - 0 is default protocol (TCP) - returns a file descriptor
	if (fd == -1) {
		perror("Error creating socket");
		return 1;
	}
	 
	/* Declare a data structure to specify the socket address (IP Address + Port)
	 * Complete the Internet socket address structure
	 * An IPv4 socket address structure - use memset to zero the struct out
	 */
	struct sockaddr_in serverAddress; // Server address structure
	memset(&serverAddress, 0, sizeof(serverAddress)); //Memory set to 0 - clear out the struct
	serverAddress.sin_family = AF_INET; // Set the address family to AF_INET
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); // Set the server IP address
	serverAddress.sin_port = htons(atoi(argv[1])); // Set the port number - htons() converts the port number to network byte order

	
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
