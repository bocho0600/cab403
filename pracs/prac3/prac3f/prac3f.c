#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 

int main(int argc, char *argv[]) {
  printf("\nWaiting for PIN...\n");
  pid_t pid = fork();
  if(pid == 0){ // Child process
    int ppid = getppid();
    srand(ppid); // Seed the random number generator
    int RandomNumber = rand()% 10000;
    printf("\nBytes read: %ld\n", sizeof(RandomNumber));
    printf("\nPIN: %d\n",RandomNumber);
    exit(EXIT_SUCCESS);
  } else if(pid>0){ // Parent process

   wait(NULL); // Wait for child process to finish

  } else { // Error
    printf("\nUnable to create child process.\n");
  }
  return EXIT_SUCCESS;
}