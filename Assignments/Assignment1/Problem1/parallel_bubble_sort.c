#include <mpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

/**
 * @brief 
 * 
 */
void sequential_bsort(int *data, int num_elems){
    
    for (int i = 0; i < num_elems-1; i++)      
        for (int j = 0; j < num_elems-i-1; j++)  
            if (data[j] > data[j+1]){
                int temp = data[j];
                data[j] = data[j+1];
                data[j+1] = temp;
            }
                
}
/**
 * @brief Stores lower num_elems numbers from the arrays src1 and src2 each containing num_elems elements
 */
void store_lower(int *src1, int *src2, int num_elems){

    int *temp = (int*)malloc(sizeof(int) * num_elems);  /* temp array to store lower elements */

    int i = 0, j = 0, k = 0;

    while(k < num_elems)
    {
        if(src1[i] < src2[j])
            temp[k++] = src1[i++];
        else
            temp[k++] = src2[j++];
    }

    k = 0;

    while(k < num_elems){
        src1[k] = temp[k];
        k++;
    }

    free(temp);

}

/**
 * @brief Stores higher num_elems numbers from the arrays src1 and src2 each containing num_elems elements
 */
void store_upper(int *src1, int *src2, int num_elems){
    
    int *temp = (int*)malloc(sizeof(int) * num_elems);  /* temp array to store higher elements */

    int i = num_elems-1, j = num_elems-1, k = num_elems-1;

    while(k >= 0)
    {
        if(src1[i] > src2[j])
            temp[k--] = src1[i--];
        else
            temp[k--] = src2[j--];
    }

    k = 0;
    while(k < num_elems){
        src1[k] = temp[k];
        k++;
    }

    free(temp);

}

/**
 * @brief Merging the given two sorted arrays in sorted order
 * 
 * @param dest      - final sorted array
 * @param left      - first source array
 * @param l_size    - size of first source array
 * @param right     - second source array
 * @param r_size    - size of second source array
 */
void merge(int *dest, int *left, int l_size, int *right, int r_size){
    int i = 0, j = 0, k = 0;
    while(j < l_size && k < r_size){

        if(left[j] < right[k]){
            dest[i++] = left[j++];
        }
        else{
            dest[i++] = right[k++];
        }

    }

    while(j < l_size)
        dest[i++] = left[j++];
    
    while(k < r_size)
        dest[i++] = right[k++];
}

/**
 * @brief Recursive function to sequentially sort the n/p elements with each processor
 * 
 * @param arr - array containing elements
 * @param num  - number of elements
 */
void merge_sort(int *arr, int num){
    int mid = 0;
    int *left_arr = NULL, *right_arr = NULL;

    if(num < 2){
        return;
    }

    mid = num/2;

    left_arr = (int*)malloc(sizeof(int) * mid);
    right_arr = (int*)malloc(sizeof(int) * (num-mid));

    for(int i = 0; i < mid; i++)
        left_arr[i] = arr[i];
    
    for(int i = mid; i < num; i++)
        right_arr[i-mid] = arr[i];
    
    merge_sort(left_arr, mid);
    merge_sort(right_arr, num-mid);
    merge(arr, left_arr, mid, right_arr, num-mid);
    free(left_arr);
    free(right_arr);
}

int main(int argc, char *argv[])
{

    MPI_Init(&argc, &argv);         // initialize MPI environment
    double time_start = MPI_Wtime();
    
    int rank = 0;                   // Current processor's rank
    int num_procs = 0;              // Number of processors in environment
    int num_elems = 0;              // Number of elements to sort
    int elems_per_proc = 0;         // Elements distributed per processor
    int *data = NULL;               // Array to hold elements to be sorted
    
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);


    if(rank == 0){
        FILE *fptr = fopen("input.txt", "r");   // file containing input

        // char *input = NULL;
        size_t inp_sz = 0;

        // getline(&input, &inp_sz, fptr);     // read entire file as a string

        // char *token = strtok(input, ", ");      // tokenize using "," or " " as delimeter
        // int i = 0;      // a flag to check first element and hence initialize number of elements
        int cnt = 0;    // index of current iata_size; i++)
        //     printf("data[%dnput scanned
        int max_elem = INT_MIN;
        int data_size;
        fscanf(fptr, "%d", &num_elems);
        data_size = (num_elems + num_procs -  (num_elems % num_procs) );
        data = (int*)malloc(sizeof(int) * data_size);
        elems_per_proc = num_elems / num_procs;        
        if((num_elems % num_procs) != 0)  //ceil operation
            elems_per_proc++;
        
        while(!feof(fptr)){
            fscanf(fptr, "%d", &data[cnt]);
            max_elem = (max_elem > data[cnt]) ? max_elem : data[cnt];
            cnt++;
        }

        for(int i=num_elems; i < data_size; i++)
            data[i] = max_elem + 1;
        
        fclose(fptr);
        // for(int i=0; i<data_size; i++)
        //     printf("data[%d] = %d\n", i, data[i]);
    }
    
    MPI_Bcast(&elems_per_proc, 1, MPI_INT, 0, MPI_COMM_WORLD);  //send num of elems per processor to each processor

    int *my_portion = (int*)malloc(sizeof(int) * (elems_per_proc)); // array of n/p elements with each processor
    int *received_data = (int*)malloc(sizeof(int) * (elems_per_proc));  // array to hold the elements received in each pass

    for(int i = 0; i < elems_per_proc; i++){    // initialize all to INT_MAX
        my_portion[i] = INT_MAX;
        received_data[i] = INT_MAX;
    }

    MPI_Scatter(data, elems_per_proc, MPI_INT, my_portion, elems_per_proc, MPI_INT, 0, MPI_COMM_WORLD); // rank 0 distributes array elements to each processor
      
    merge_sort(my_portion, elems_per_proc);     // each processor sequentially sorts it's portion of array

    for(int i = 0; i < num_procs; i++){ 

        if(i % 2 == 0){  //even phase
            int even_partner_rank = (rank + 1 < num_procs) ? rank + 1 : MPI_PROC_NULL;
            int odd_partner_rank = (rank - 1 >= 0) ? rank - 1 : MPI_PROC_NULL;

            if(rank % 2 == 0){   //even rank
                if(even_partner_rank != MPI_PROC_NULL){
                    MPI_Sendrecv(my_portion, elems_per_proc, MPI_INT, even_partner_rank, 0, received_data, elems_per_proc, MPI_INT, even_partner_rank, 0, MPI_COMM_WORLD, NULL);
                    store_lower(my_portion, received_data, elems_per_proc);
                }
            }   // end of if even rank
            else{
                if(odd_partner_rank != MPI_PROC_NULL){
                    
                    MPI_Sendrecv(my_portion, elems_per_proc, MPI_INT, odd_partner_rank, 0, received_data, elems_per_proc, MPI_INT, odd_partner_rank, 0, MPI_COMM_WORLD, NULL);
                    store_upper(my_portion, received_data, elems_per_proc);
                }
            }   // end of else odd rank
        }   // end of if even phase
        else{   //odd phase
            int even_partner_rank = (rank - 1 >= 0) ? rank - 1 : MPI_PROC_NULL;
            int odd_partner_rank = (rank + 1 < num_procs) ? rank + 1 : MPI_PROC_NULL;

            if(rank % 2 == 0){   //even rank
                if(even_partner_rank != MPI_PROC_NULL){                    
                    MPI_Sendrecv(my_portion, elems_per_proc, MPI_INT, even_partner_rank, 0, received_data, elems_per_proc, MPI_INT, even_partner_rank, 0, MPI_COMM_WORLD, NULL);
                    store_upper(my_portion, received_data, elems_per_proc);
                }
            }   // end of if even rank
            else{
                if(odd_partner_rank != MPI_PROC_NULL){                    
                    MPI_Sendrecv(my_portion, elems_per_proc, MPI_INT, odd_partner_rank, 0, received_data, elems_per_proc, MPI_INT, odd_partner_rank, 0, MPI_COMM_WORLD, NULL);
                    store_lower(my_portion, received_data, elems_per_proc);
                }
            }   // end of else odd rank
        }   // end of else odd phase

    }

    MPI_Gather(my_portion,elems_per_proc, MPI_INT, data, elems_per_proc, MPI_INT, 0, MPI_COMM_WORLD);   // collect the sorted subarrays from each processor to 0th processor

    if((rank == 0) && data != NULL){    // print sorted elements
        printf("Sorted output:\n");
        for(int i = 0; i < num_elems; i++)
            printf("%d\n", i, data[i]);
    }

    free(my_portion);
    free(received_data);
    
    double time_elapsed = MPI_Wtime() - time_start;
    double execution_time_pbsort;
    MPI_Reduce(&time_elapsed, &execution_time_pbsort, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if(rank == 0){
        clock_t sbsort_start = clock();
        sequential_bsort(data, num_elems);
        clock_t sbsort_end = clock();

        double execution_time_sbsort = (double)(sbsort_end - sbsort_start) / CLOCKS_PER_SEC;

        printf("Time taken by seq bsort : %lf\n", execution_time_sbsort);
        printf("Time taken by parallel bsort : %lf\n", execution_time_pbsort);
        printf("Speedup achieved : %lf\n", execution_time_sbsort / execution_time_pbsort);
    }
    free(data);
    MPI_Finalize(); 
    
    return 0;
}
