
#include "func.h"

#define SHARED_MEM_BASE_NAME "/car"
#define SHM_NAME_LENGTH 256
#define SHM_PERMISSIONS 0666
#define ERROR_MSG_MAX_LENGTH 256

car_shared_mem *car;

// Helper function to write error messages
static void write_error(const char *message) {
    write(STDERR_FILENO, message, strlen(message));  // Write error messages to stderr
}

int main(int argc, char *argv[]) {
    int shm_fd;
    char shm_name[SHM_NAME_LENGTH];

    // Ensure correct usage
    if (argc != 2) {
        write_error("Usage: {car name}\n");  // Use write for usage message
        return EXIT_FAILURE;  // Use standard exit codes
    }

    // Create shared memory name with car name
    snprintf(shm_name, sizeof(shm_name), "%s%s", SHARED_MEM_BASE_NAME, argv[1]);

    // Open the shared memory object - read/write mode
    shm_fd = shm_open(shm_name, O_RDWR, SHM_PERMISSIONS);
    if (shm_fd == -1) {
        write_error("Unable to access car., %s \n");  // Use write for error message
        return EXIT_FAILURE;
    }

    // Map the shared memory object onto the address space
    car = (car_shared_mem *)mmap(NULL, sizeof(car_shared_mem), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (car == MAP_FAILED) {
        perror("Error mapping the shared memory object");
        return EXIT_FAILURE;
    }

    while (1) {
        // Lock the mutex to safely access shared memory
        pthread_mutex_lock(&car->mutex);

        // Wait for a change in the shared memory using the condition variable
        pthread_cond_wait(&car->cond, &car->mutex);

        // Perform checks
        if (car->emergency_mode != 1 && 
            (!isValidFloor(car->current_floor) ||      
             !isValidFloor(car->destination_floor) ||  
             !isValidStatus(car->status) ||            
             !isValidField(car) ||                     
             car->emergency_mode > 1 ||                
             (car->door_obstruction && 
              strcmp(car->status, "Opening") != 0 && 
              strcmp(car->status, "Closing") != 0))) {
            
            write_error("Data consistency error!\n");  // Use write for error message
            car->emergency_mode = 1;  // Set the emergency mode
        }

        // Handle door obstruction scenario
        if (car->door_obstruction && strcmp(car->status, "Closing") == 0) {
            strcpy(car->status, "Opening"); // Set status to "Opening" if there is an obstruction while closing
        }

        // Handle emergency stop scenario
        if (car->emergency_stop && !car->emergency_mode) {
            write_error("The emergency stop button has been pressed!\n");  // Use write for error message
            car->emergency_mode = 1;  // Set the emergency mode
        }

        // Handle overload scenario
        if (car->overload && !car->emergency_mode) {
            write_error("The overload sensor has been tripped!\n");  // Use write for error message
            car->emergency_mode = 1;  // Set the emergency mode
        }

        // Unlock the mutex after accessing shared memory
        pthread_mutex_unlock(&car->mutex);
    }

    // Clean up resources (never reached in current loop)
    munmap(car, sizeof(car_shared_mem));
    close(shm_fd);

    return EXIT_SUCCESS; // Use standard exit codes
}
