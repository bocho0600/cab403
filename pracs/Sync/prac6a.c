#include <pthread.h>
#include <stdio.h>

int globalVar = 0;
pthread_mutex_t lock;           // Mutex variable
void *addMillion(void *arg);

int main(int argc, char *argv[]) {
      pthread_t thread1, thread2, thread3;
      // Initialize the mutex before using it
      pthread_mutex_init(&lock, NULL);
      pthread_create(&thread1, NULL, addMillion, NULL);
      pthread_create(&thread2, NULL, addMillion, NULL);
      pthread_create(&thread3, NULL, addMillion, NULL);
      //pthread_create(&thread4, NULL, addMillion, NULL);
      pthread_join(thread1, NULL);
      pthread_join(thread2, NULL);
      pthread_join(thread3, NULL);
      pthread_mutex_destroy(&lock); // destroy the mutex lock after use
      printf("\n\nTotal for globalVar = %d\n", globalVar);
      return 0;
}

//This is the critical section
void *addMillion(void *arg) {   
      pthread_mutex_lock(&lock); // Lock the mutex before entering the critical section
      printf("locking the mutex\n");
      fflush(stdout);
      for (int i = 0; i < 1000000; i++) {
            globalVar++;
      }    
      pthread_mutex_unlock(&lock); // Unlock the mutex after leaving the critical section
      printf("unlocking the mutex\n");
      fflush(stdout);
      return NULL;
}