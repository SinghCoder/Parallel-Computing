#include <stdio.h>
#include <stdlib.h>
#include <omp.h>   
#include <time.h>

int main(int argc, char* argv[]) {
	
	int thread_count = strtol(argv[1], NULL, 10); 
	double time_spent;
	clock_t begin ,end;
	
	begin = clock();
	
        for(int i = 0; i < thread_count; i++) {
            printf("hi");
        }
        printf("\n");

	end = clock();
	
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	printf("Time spent in serial : %lf\n", time_spent);


	begin = clock();
	
	#pragma omp parallel num_threads(thread_count) 
    {
        #pragma omp for
            for(int i = 0; i < thread_count; i++) {
                printf("hi");
            }
    }
    printf("\n");
	end = clock();
	
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	printf("Time spent in parallel : %lf\n", time_spent);
   return 0; 
}
