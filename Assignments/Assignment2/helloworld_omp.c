#include <stdio.h>
#include <stdlib.h>
#include <omp.h>   
#include <time.h>

void Hello(void) {
	
	int my_rank = omp_get_thread_num();
	int thread_count = omp_get_num_threads();

	// printf("Hello from thread %d of %d\n", my_rank, thread_count);

}

int main(int argc, char* argv[]) {
	
	int thread_count = strtol(argv[1], NULL, 10); 
	double time_spent;
	clock_t begin ,end;
	
	begin = clock();
	
	for(int i = 0; i < thread_count; i++)
		Hello();

	end = clock();
	
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	printf("Time spent in serial : %lf\n", time_spent);


	begin = clock();
	
	#pragma omp parallel num_threads(thread_count) 
		Hello();

	end = clock();
	
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	printf("Time spent in parallel : %lf\n", time_spent);
   return 0; 
}
