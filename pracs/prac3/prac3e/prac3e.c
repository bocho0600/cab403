#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 

int main(int argc, char *argv[]) {
  for (int i = 0; i < 5; i++) {
    printf("\nIteration: %d\n", i+1);
    pid_t pid = fork(); 
    if(pid == 0) {
      printf("\nChild => PPID: %d PID: %d\n", getppid(), getpid());
      //getppid(): returns the parent process ID, getpid(): returns the child process ID
      exit(EXIT_SUCCESS);
    } else if(pid > 0) { 
      printf("\nParent => PID: %d\n", getpid());
      printf("\nWaiting for child process to finish.\n");
      wait(NULL);
      printf("\nChild process finished.\n");
    }  else {
      printf("\nUnable to create child process.\n");
    }
  }
    return EXIT_SUCCESS;
}