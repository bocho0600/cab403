#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

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
    uint32_t len = htonl(strlen(buf)); // htonl() is for network byte order
    send_looped(fd, &len, sizeof(len));
    send_looped(fd, buf, strlen(buf));
}

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket()");
        exit(1);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3000);
    const char *ipaddress = "127.0.0.1";
    if (inet_pton(AF_INET, ipaddress, &addr.sin_addr) != 1) {
        fprintf(stderr, "inet_pton(%s)\n", ipaddress);
        exit(1);
    }
    
    if (connect(sockfd, (const struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("connect()");
        exit(1);
    }

    char buf[1024];
    fgets(buf, 1023, stdin);

    send_message(sockfd, buf);

    printf("Sent this msg to client: %s\n", buf);
        
    if (shutdown(sockfd, SHUT_RDWR) == -1) {
        perror("shutdown()");
        exit(1);
    }
    if (close(sockfd) == -1) {
        perror("close()");
        exit(1);
    }
}
