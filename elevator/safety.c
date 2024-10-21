#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdint.h>
#include <ctype.h>
#include "func.h"
#define SHARED_MEM_BASE_NAME "/car"


car_shared_mem *car;

int main(int argc, char *argv[]) {

  if (argc != 2) {
    printf("Usage: %s {car name}\n", argv[0]);
    return 1;
  }

  char *car_name = argv[1];
  char shm_name[256];
  snprintf(shm_name, sizeof(shm_name), "%s%s", SHARED_MEM_BASE_NAME, car_name);  // Create the shared memory name with car name

  int shm_fd = shm_open(shm_name, O_RDWR, 0666);  // Open the shared memory object - read/write mode
  if (shm_fd == -1) {
    printf("Unable to access car %s.\n", car_name);
    return 1;
  }

  // Map the shared memory object onto the address space
  car = (car_shared_mem *)mmap(NULL, sizeof(car_shared_mem), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (car == MAP_FAILED) {
    perror("Error mapping the shared memory object");
    return 1;
  }

  while (1) {


    // Lock the mutex to safely access shared memory
    pthread_mutex_lock(&car->mutex);

    // Wait for a change in the shared memory using the condition variable
    pthread_cond_wait(&car->cond, &car->mutex);

    // Perform checks

    if (car->emergency_mode != 1 && (!isValidFloor(car->current_floor) ||      // Check if the current floor is valid
                                     !isValidFloor(car->destination_floor) ||  // Check if the destination floor is valid
                                     !isValidStatus(car->status) ||            // Check if the status is valid
                                     !isValidField(car) ||                     // Check if the uint8_t fields are valid
                                     car->emergency_mode > 1 ||                // Check if the emergency mode is valid
                                     (car->door_obstruction && strcmp(car->status, "Opening") != 0 && strcmp(car->status, "Closing") != 0))) {

      printf("Data consistency error!\n");
      car->emergency_mode = 1;  // Set the emergency mode
    }

    if (car->door_obstruction && strcmp(car->status, "Closing") == 0) {
      strcpy(car->status, "Opening");  // Set status to "Opening" if there is an obstruction while closing
    }

    if (car->emergency_stop && !car->emergency_mode) {
      printf("The emergency stop button has been pressed!\n");
      car->emergency_mode = 1;  // Set the emergency mode
    }
    if (car->overload && !car->emergency_mode) {
      printf("The overload sensor has been tripped!\n");
      car->emergency_mode = 1;  // Set the emergency mode
    }



    // Unlock the mutex after accessing shared memory
    pthread_mutex_unlock(&car->mutex);
  }

  return 0;
}
