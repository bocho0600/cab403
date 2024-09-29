#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h> 

char *message = "Hello Child! This is a message from parent.";
char *message2 = "Hello parent! This is a message from child!!!";

int main() {
    pid_t childpid; /* variable to store the child's pid */
    char buf[1024];
    int fd[2];
    pipe(fd); /* create pipe */
    if (fork() != 0)
    { /* I am the parent */
        write(fd[1], message, strlen(message) + 1); // Write the message to the pipe. MUST BE fd[1]
        wait(NULL); // Wait for the child process to finish
        read(fd[0], buf, strlen(message) + 1); // Read the message from the pipe. MUST BE fd[0]
        printf("PARENT: I got the following message from my child: %s\n", buf);
        printf("Parent process finished.\n");
    } else { /* Child code */
        childpid=getpid();
        printf("CHILD: I am the child process!\n");
        printf("CHILD: Here's my PID: %d\n", childpid);
        printf("CHILD: My parent's PID is: %d\n", getppid());
        read(fd[0], buf, strlen(message) + 1); // Read the message from the pipe. MUST BE fd[0]
        write(fd[1], message2, strlen(message2) + 1); // Write the message to the pipe. MUST BE fd[1]
        printf("CHILD: I got the following message from my parent: %s\n", buf);
    }
}

