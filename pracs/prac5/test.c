#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Function to be executed by the threads
void* print_message(void* thread_id) {
    int tid = *(int*)thread_id;
    printf("Thread %d is running\n", tid);
    sleep(1); // Simulate some work by sleeping for 1 second
    printf("Thread %d is finishing\n", tid);
    return NULL;
}

int main() {
    pthread_t threads[2]; // Array to hold thread identifiers
    int thread_ids[2];    // Array to hold thread IDs

    // Create two threads
    for (int i = 0; i < 2; i++) {
        thread_ids[i] = i + 1; // Assign thread ID (1 and 2)
        printf("Creating thread %d\n", thread_ids[i]);
        int result = pthread_create(&threads[i], NULL, print_message, (void*)&thread_ids[i]);
        //int pthread_create(pthread_t *thread, pthread_attr_t *attr,  void *thread_function,void *thread_args);
        if (result != 0) { // if threads created successfully, result will be 0
            fprintf(stderr, "Error creating thread %d\n", thread_ids[i]);
            exit(1);
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < 2; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("All threads have finished execution.\n");

    return 0;
}
