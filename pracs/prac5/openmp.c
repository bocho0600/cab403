// to compile: gcc -fopenmp openmp.c -o openmp

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

void Hello(void); /* Function to thread */



int main(int argc, char *argv[]) {
	int num_threads = 4;
	omp_set_num_threads(num_threads);

	/*A very important point to note is that data allocated 
	outside of # pragma omp parallel is available to all the threads. 
	If the data is allocated inside the # pragma omp parallel declaration the data 
	is only available to that thread (i.e. it is private to the thread).*/

	#pragma omp parallel // pragma to start parallel region, 4 threads run parallel
	{
		Hello();
	}
	return EXIT_SUCCESS;
}

void Hello(void) {
	int my_rank = omp_get_thread_num(); // get thread number
	int thread_count = omp_get_num_threads(); // get total number of threads
	printf("Hello from thread %d of %d\n", my_rank, thread_count);
}
