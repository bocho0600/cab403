#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define TRUE 1

buffer_item buffer[BUFFER_SIZE];
pthread_mutex_t mutex;
sem_t empty;
sem_t full;

int insertPointer = 0, removePointer = 0;

void *producer(void *param);
void *consumer(void *param);

int insert_item(buffer_item item) {
	//Acquire Empty Semaphore
	sem_wait(&empty); // Wait for an empty slot


	//Acquire mutex lock to protect buffer
	pthread_mutex_lock(&mutex);
	buffer[insertPointer++] = item;
	insertPointer = insertPointer % BUFFER_SIZE;

	//Release mutex lock and full semaphore
	pthread_mutex_unlock(&mutex); 
	sem_post(&full); // Signal that there's a new item

	return 0;
}

int remove_item(buffer_item *item) {
	/* insert your code here for Task 1 (a) */
	sem_wait(&full); // Wait for a filled slot from the producer
	/* Acquire Full Semaphore */
	pthread_mutex_lock(&mutex);
	/* Acquire mutex lock to protect buffer */
	*item = buffer[removePointer]; // Remove the item from the buffer
	buffer[removePointer++] = -1; // Set the buffer to -1
	removePointer = removePointer % BUFFER_SIZE;// Increment the remove pointer
	pthread_mutex_unlock(&mutex);
	sem_post(&empty);
	/* Release mutex lock and empty semaphore */

	return 0;
}

int main(int argc, char *argv[]) {
	int sleepTime, producerThreads, consumerThreads;
	int i, j;

	if (argc != 4) 	{
		fprintf(stderr, "Useage: <sleep time> <producer threads> <consumer threads>\n");
		return -1;
	}

	/* insert you code below for Task 1 (b) */
	sleepTime = atoi(argv[1]);
	producerThreads = atoi(argv[2]);
	consumerThreads = atoi(argv[3]);
	pthread_t tid[producerThreads];
	pthread_t cid[consumerThreads];
	/* Get command line arguments argv[1], argv[2], argv[3]*/

	/* Initialize the the locks */
	pthread_mutex_init(&mutex, NULL);
	sem_init(&empty, 0, BUFFER_SIZE); // Initialize the empty semaphore
	sem_init(&full, 0, 0); // Initialize the full semaphore
	/* Create the producer and consumer threads */
	for (i = 0; i < producerThreads; i++) {
		pthread_attr_t attr; // Set of thread attributes
		pthread_attr_init(&attr); // Get the default attributes
		pthread_create(&tid[i], &attr, producer, NULL);
	}
	for (j = 0; j < consumerThreads; j++) {
		pthread_attr_t attr; // Set of thread attributes
		pthread_attr_init(&attr); // Get the default attributes
		pthread_create(&cid[j], &attr, consumer, NULL);
	}
	/* Sleep for user specified time */
	sleep(sleepTime);
		// Here you might want to join the threads to clean up
	for (i = 0; i < producerThreads; i++) {
		pthread_cancel(tid[i]); // Cancel producer threads
	}
	for (j = 0; j < consumerThreads; j++) {
		pthread_cancel(cid[j]); // Cancel consumer threads
	}

	pthread_mutex_destroy(&mutex);
	sem_destroy(&empty);
	sem_destroy(&full);

	return 0;
}

void *producer(void *param) {
	buffer_item random;
	int r;

	while (TRUE) 	{
		r = rand() % 5; //r is a random number between 0 and 4
		sleep(r);
		random = rand(); //random is a random number

		if (insert_item(random)) {
			fprintf(stderr, "Error");
		} else {
			printf("Producer produced %d \n", random);
		}
	}
}

void *consumer(void *param) {
	buffer_item random;
	int r;

	while (TRUE) 	{
		r = rand() % 5;
		sleep(r);

		if (remove_item(&random)) {
			fprintf(stderr, "Error Consuming");
		} else {
			printf("Consumer consumed %d \n", random);
		}
	}
}
