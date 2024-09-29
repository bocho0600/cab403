#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define MAX_ITEMS 5

int buffer[BUFFER_SIZE] = {0};
int in = 0;
int out = 0;
int produced_count = 0;
int consumed_count = 0;

pthread_mutex_t mutex;
pthread_cond_t not_empty;
pthread_cond_t not_full;
void* producer(void* arg) ;
void* consumer(void* arg) ;

void printBuffer() {
	for (int i=0; i < BUFFER_SIZE; i++) {
		printf(" %d ", buffer[i]);
	}
	printf("\n");
}

void* producer(void* arg) {
	for (;;) {
            pthread_mutex_lock(&mutex);
		while (((in + 1) % BUFFER_SIZE) == out){
                  pthread_cond_wait(&not_full, &mutex); //release mutex and wait for signal when buffer is not full
            } // do nothingndhile buffer is full
		buffer[in] = 1;		
		in = (in + 1) % BUFFER_SIZE;
		printBuffer();
		sleep(1);
            pthread_cond_signal(&not_empty); // signal that buffer is not empty
            pthread_mutex_unlock(&mutex);
	}
}

void* consumer(void* arg) {
	for (;;) {
            pthread_mutex_lock(&mutex);
		while (in == out){
                  pthread_cond_wait(&not_empty, &mutex); //release mutex and wait for signal when buffer is not empty
            } // do nothing while buffer is empty
		buffer[out]=0;    
		out = (out + 1) % BUFFER_SIZE;
		printBuffer();
		sleep(1);
            pthread_cond_signal(&not_full); // signal that buffer is not full
            pthread_mutex_unlock(&mutex);
	}
 }

int main(int argc, char *argv[]) {
      pthread_t producerThread1, producerThread2, consumerThread1, consumerThread2;
      // mutex not used as yet
      pthread_mutex_init(&mutex, NULL);
      pthread_cond_init(&not_empty, NULL);
      pthread_cond_init(&not_full, NULL);

      pthread_create(&producerThread1, NULL, producer, NULL);
      pthread_create(&producerThread2, NULL, producer, NULL);
      pthread_create(&consumerThread1, NULL, consumer, NULL);
      pthread_create(&consumerThread2, NULL, consumer, NULL);
            
      pthread_join(producerThread1, NULL);
      pthread_join(producerThread1, NULL);
      pthread_join(consumerThread1, NULL);
      pthread_join(consumerThread2, NULL);
      pthread_mutex_destroy(&mutex);
      // Cleanup
      pthread_cond_destroy(&not_empty);
      pthread_cond_destroy(&not_full);
      return 0;
}
