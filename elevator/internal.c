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

#define SHARED_MEM_BASE_NAME "/car"

// Shared memory structure of the car (elevator)
typedef struct {
    pthread_mutex_t mutex;           // Locked while accessing struct contents
    pthread_cond_t cond;             // Signalled when the contents change
    char current_floor[4];           // C string in the range B99-B1 and 1-999
    char destination_floor[4];       // Same format as above
    char status[8];                  // C string indicating the elevator's status
    uint8_t open_button;             // 1 if open doors button is pressed, else 0
    uint8_t close_button;            // 1 if close doors button is pressed, else 0
    uint8_t door_obstruction;        // 1 if obstruction detected, else 0
    uint8_t overload;                // 1 if overload detected
    uint8_t emergency_stop;          // 1 if stop button has been pressed, else 0
    uint8_t individual_service_mode; // 1 if in individual service mode, else 0
    uint8_t emergency_mode;          // 1 if in emergency mode, else 0
} car_shared_mem;

car_shared_mem *car;

// Function declarations
char* UpFloor(const char currentFloor[4]);
char* DownFloor(const char currentFloor[4]);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s {car name} <operation>\n", argv[0]);
        return 1;
    }

    char *car_name = argv[1];
    char *operation = argv[2];

    // Open the shared memory object
    char shm_name[256];
    snprintf(shm_name, sizeof(shm_name), "%s%s", SHARED_MEM_BASE_NAME, car_name); // Create the shared memory name with carname
    int shm_fd = shm_open(shm_name, O_RDWR, 0666); // Open the shared memory object - read/write mode
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

    // Mutex lock
    pthread_mutex_lock(&car->mutex);

    // Perform the operation
    if (strcmp(operation, "open") == 0) {
        car->open_button = 1;
    } else if (strcmp(operation, "close") == 0) {
        car->close_button = 1;
        //strcpy(car->status, "Closed");
    } else if (strcmp(operation, "stop") == 0) {
        car->emergency_stop = 1;
    } else if (strcmp(operation, "service_on") == 0) {
        car->individual_service_mode = 1;
        car->emergency_mode = 0;
    } else if (strcmp(operation, "service_off") == 0) {
        car->individual_service_mode = 0;
    } else if (strcmp(operation, "up") == 0) {
        if (car->individual_service_mode) {
            if (strcmp(car->status, "Closed") == 0) {
                strcpy(car->destination_floor, UpFloor(car->current_floor)); // Update destination floor
            } else if(strcmp(car->status, "Between") == 0) {
                printf("Operation not allowed while elevator is moving.\n");
            } else {
                printf("Operation not allowed while doors are open.\n");
            }
        } else {
            printf("Operation only allowed in service mode.\n");
        }
    } else if (strcmp(operation, "down") == 0) {
        if (car->individual_service_mode) {
            if(strcmp(car->status, "Closed") == 0) {
                strcpy(car->destination_floor, DownFloor(car->current_floor)); // Update destination floor
            } else if(strcmp(car->status, "Between") == 0) {
                printf("Operation not allowed while elevator is moving.\n");
            } else {
                printf("Operation not allowed while doors are open.\n");
            }
        } else {
            printf("Operation only allowed in service mode.\n");
        }
    } else {
        printf("Invalid operation.\n");
        pthread_mutex_unlock(&car->mutex); // Unlock mutex before return
        return 1;
    }

    // Unlock mutex
    pthread_mutex_unlock(&car->mutex);

    // Optionally, signal the condition variable to notify waiting threads
    pthread_cond_broadcast(&car->cond);

    return 0;
}

char* UpFloor(const char currentFloor[4]) {
    static char newFloor[4]; // Use static to return a pointer to a local variable
    if (currentFloor[0] == 'B') { // Check if the current floor starts with 'B' - basement
        int currentFloorNum = atoi(currentFloor + 1); // Convert to int
        if (currentFloorNum > 1){
            currentFloorNum--; // Increment the floor number
            snprintf(newFloor, sizeof(newFloor), "B%d", currentFloorNum); // Format new floor
        } else if (currentFloorNum == 1) {
            snprintf(newFloor, sizeof(newFloor), "%d", currentFloorNum); // Format new floor
        }
    } else if (isdigit(currentFloor[0])) { // Check if the current floor is a digit - above basement
        int currentFloorNum = atoi(currentFloor); // Convert to int
        if (currentFloorNum < 999){
            currentFloorNum++;
            snprintf(newFloor, sizeof(newFloor), "%d", currentFloorNum); // Format new floor
        }
    } else {
        printf("Invalid Floor.\n");
        return (char* )currentFloor; // No change made, return the same floor
    }
    return newFloor; // Return the new floor
}

char* DownFloor(const char currentFloor[4]) {
    static char newFloor[4]; // Use static to return a pointer to a local variable
    if (currentFloor[0] == 'B') { // Check if the current floor starts with 'B' - basement
        int currentFloorNum = atoi(currentFloor + 1); // Convert to int
        if (currentFloorNum < 99){
            currentFloorNum++; // Increment the floor number
            snprintf(newFloor, sizeof(newFloor), "B%d", currentFloorNum); // Format new floor
        } else if (currentFloorNum == 99) {
            snprintf(newFloor, sizeof(newFloor), "%d", currentFloorNum); // Format new floor
        }
    } else if (isdigit(currentFloor[0])) { // Check if the current floor is a digit - above basement
        int currentFloorNum = atoi(currentFloor); // Convert to int
        if (currentFloorNum > 1) {
            currentFloorNum--;
            snprintf(newFloor, sizeof(newFloor), "%d", currentFloorNum); // Format new floor
        } else if (currentFloorNum == 1) {
            snprintf(newFloor, sizeof(newFloor), "B%d", currentFloorNum); // Format new floor
        }
    } else {
        printf("Invalid Floor.\n");
        return (char*)currentFloor; // No change made, return the same floor
    }
    return newFloor; // Return the new floor
}
