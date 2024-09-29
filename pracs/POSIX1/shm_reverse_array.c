/**
 * CAB403 - POSIX Shared Memory Walkthrough
 * 
 * Demonstration of the creation of a POSIX shared memory object.
 * 
 * Application to generate an array of random integers and sort them into non-descending order
 * or non-ascending order depending if child/parent process.
 * The sorted random numbers will be stored in an array which is part of the data
 * structure defined in shm_data.h. This data structure represents the shared memory object
 * and will be used to access the shared memory.
 * Four parameters should be passed at the command line when running application.
 * Format to run application "./app_name shm_name lower upper count"
 * \shm_name: shared memory object name
 * \lower: lowest value for random number
 * \upper: highest value for random number
 * \count: amount of random numbers to create
 *  
 * The shared memory object will be mapped to the process before calling fork() to 
 * create a child process. The child will inherit the shared memory identifier and 
 * mapped memory object.The child will create an array of integers and then sort them
 * into non-descending order. This data will then be stored in the shared memory object 
 * created by the parent process. The child should print the sorted array to the terminal and exit. 
 * The parent will wait until the child has exited then will confirm the array has
 * been sorted by printing the array stored in the shared memory object. Parent will then sort
 * the array stored in the shared memory object into non-ascending order and print out the sequence
 * of numbers from the shared memory object.
 * Parent will then remove the shared memory object and exit.
 */


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>       // for O_* constants
#include <sys/mman.h>    // for mmap and shm_open
#include <unistd.h>      // for fork and getpid
#include <sys/wait.h>    // for wait
#include <time.h>        // for time and srand
#include <string.h>      // for strlen
#include "shm_data.h"    // assumed header for data_share_t structure and narration strings
#include "protoHeader.h" // assumed header for function prototypes

int main(int argc, char *argv[]) {  
    if (argc != 5) {
        fprintf(stderr, "Usage: %s shm_name lower_range upper_range count\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Arguments of the command line
    char *shm_name = argv[1];
    if (checkShMemName(shm_name)) {
        fprintf(stderr, "Invalid shared memory name format. It must start with '/' and not be empty.\n");
        return EXIT_FAILURE;
    }

    int lower = atoi(argv[2]);
    int upper = atoi(argv[3]);
    int count = atoi(argv[4]);
    
    // Validate command line arguments
    if (lower >= upper || count <= 0 || isNumber(argv[2]) || isNumber(argv[3]) || isNumber(argv[4])) {
        fprintf(stderr, "Invalid arguments. Ensure that lower < upper and count is a positive integer.\n");
        return EXIT_FAILURE;
    }
    
    int arrSize = sizeof(data_share_t) + count * sizeof(int);

    // Call function createSharedMemoryObject() to create the shared memory object
    int shm_fd = createSharedMemoryObject(shm_name, arrSize);

    // Check return value to determine if failed to create shared memory object
    if (shm_fd == -1) {
        fprintf(stderr, "Error creating shared memory object\n");
        return EXIT_FAILURE;
    }

    // Call function mapSHMObject() to map the shared memory object to the virtual address space
    data_share_t *data = mapSHMObject(shm_fd, arrSize);

    // Set the arrSize value in the data_share_t ADT
    data->arrSize = count; 
    
    // Generate random numbers and store them in shared memory
    if (fork() == 0) {
        // Child process
        int *randomNumbers = genRandomIntegerNumbers(lower, upper, count);
        if (randomNumbers == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }

        // Store the random numbers in shared memory
        for (int i = 0; i < count; i++) {
            data->arrayData[i] = randomNumbers[i];
        }
        
        free(randomNumbers); // Free the allocated memory
        
        // Print and sort array in ascending order
        printArray(pre, data->arrayData, data->arrSize);
        qsort(data->arrayData, data->arrSize, sizeof(int), compareA);
        printArray(postAC, data->arrayData, data->arrSize);
        
        // Unmap and close shared memory
        unMapClose(data, arrSize, shm_fd);
        exit(EXIT_SUCCESS);
    } else {
        // Parent process
        wait(NULL); // Wait for the child to complete
        
        // Print the current array and sort it in descending order
        printArray(postPA, data->arrayData, data->arrSize);
        qsort(data->arrayData, data->arrSize, sizeof(int), compareB);
        printArray(postPD, data->arrayData, data->arrSize);
        
        // Unmap and close shared memory, then unlink it
        unMapClose(data, arrSize, shm_fd);
        shm_unlink(shm_name);
    }

    return EXIT_SUCCESS;
}

/**
 * Function to map the shared memory object to virtual address space of the calling process
 * \param fd - file descriptor returned from creating shared memory object
 * \param arrSize - the length of the array that contains the random integers generated
 * \returns - pointer/address used for mapping that can be used to modify data in shared memory object
 *          - returns MAP_FAILED if the mapping fails
 */
data_share_t *mapSHMObject(int fd, int arrSize) {
  data_share_t *data = mmap(0, arrSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); // map shared memory object
  if (data == MAP_FAILED) {
    fprintf(stderr, "Error mapping shared memory object\n");
    return MAP_FAILED;
  } else {
    return data;
  }
}
int createSharedMemoryObject(char *name, int arrSize) {
    int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666); // create shared memory object
    if (shm_fd == -1) {
        fprintf(stderr, "Error creating shared memory object\n");
        return -1;
    }
    if (ftruncate(shm_fd, arrSize) == -1) { // configure size
        perror("ftruncate");
        close(shm_fd);
        return -1;
    }
    return shm_fd;
}
/**
 * Function to remove share memory object mapping and close the file descriptor
 * returned from opening the shared memory object
 * \param data - start address for the mapping
 * \param arrSize - the size of the array
 * \param fd - file descriptor returned from shm_open()
 */
void unMapClose(data_share_t *data, int arrSize,int fd) {
  munmap (data, arrSize); // Remove shred memory object mapping
  close(fd); // Close file descriptor
}