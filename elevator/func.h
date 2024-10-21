#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <ctype.h>
typedef struct
{
  pthread_mutex_t mutex;           // Locked while the contents of the structure are being accessed/modified
  pthread_cond_t cond;             // Signalled when the contents of the structure change
  char current_floor[4];           // NUL-terminated C string in the range of "B99" to "B1" and "1" to "999"
  char destination_floor[4];       // Same format as above
  char status[8];                  // NUL-terminated C string indicating the elevator's status
  uint8_t open_button;             // 1 if open doors button is pressed, 0 otherwise
  uint8_t close_button;            // 1 if close doors button is pressed, 0 otherwise
  uint8_t door_obstruction;        // 1 if obstruction detected, 0 otherwise
  uint8_t overload;                // 1 if overload detected
  uint8_t emergency_stop;          // 1 if emergency stop button has been pressed, 0 otherwise
  uint8_t individual_service_mode; // 0 if not in individual service mode, 1 if in individual service mode
  uint8_t emergency_mode;          // 0 if not in emergency mode, 1 if in emergency mode
} car_shared_mem;


int isValidFloor(char *floor) {
    int len = strlen(floor);

    // Check for basement floors (B1-B99)
    if (floor[0] == 'B') {
        // Check length for valid range B1 to B99
        if (len == 2 && isdigit(floor[1])) { // B1 to B9
            return 1;
        } else if (len == 3 && isdigit(floor[1]) && isdigit(floor[2])) { // B10 to B99
            // Convert the number part to an integer and check range
            int basement_num = atoi(floor + 1); // Skip 'B' and convert remaining part
            if (basement_num >= 1 && basement_num <= 99) {
                return 1;
            }
        }
        return 0; // Invalid basement floor format
    }

    // Check for regular floors (1-999)
    // All characters must be digits and length should be between 1 to 3
    if (len >= 1 && len <= 3) {
        for (int i = 0; i < len; i++) {
            if (!isdigit(floor[i])) {
                return 0; // Invalid format if any non-digit is found
            }
        }

        int floor_num = atoi(floor);
        if (floor_num >= 1 && floor_num <= 999) {
            return 1; // Valid regular floor
        }
    }

    return 0; // Invalid regular floor format
}

int isValidStatus(char *status) {
    if (strcmp(status, "Closed") == 0 || strcmp(status, "Opening") == 0 || strcmp(status, "Open") == 0 || strcmp(status, "Closing") == 0 || strcmp(status, "Between") == 0) {
        return 1;
    }
    return 0;
}

int isValidField(car_shared_mem *car) {
    // Check each uint8_t field for values greater than 1
    if (car->open_button > 1) return 0;
    if (car->close_button > 1) return 0;
    if (car->door_obstruction > 1) return 0;
    if (car->overload > 1) return 0;
    if (car->emergency_stop > 1) return 0;
    if (car->individual_service_mode > 1) return 0;
    if (car->emergency_mode > 1) return 0;

    // All fields are valid (0 or 1)
    return 1;
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


int connect_to_controller(void)
{
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in sockaddr;
  memset(&sockaddr, 0, sizeof(sockaddr));
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_port = htons(3000);
  sockaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  if (connect(fd, (const struct sockaddr *)&sockaddr, sizeof(sockaddr)) == -1)
  {
    perror("connect()");
    exit(1);
  }
  return fd;
}


void send_looped(int fd, const void *buf, size_t sz)
{
  const char *ptr = buf;
  size_t remain = sz;

  while (remain > 0) {
    ssize_t sent = write(fd, ptr, remain);
    if (sent == -1) {
      perror("write()");
      exit(1);
    }
    ptr += sent;
    remain -= sent;
  }
}

void send_message(int fd, const char *buf)
{
  uint32_t len = htonl(strlen(buf));
  send_looped(fd, &len, sizeof(len));
  send_looped(fd, buf, strlen(buf));
}