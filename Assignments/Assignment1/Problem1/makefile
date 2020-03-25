all: p_bsort
		mpirun --hostfile hostfile -np ${np} p_bsort

p_bsort : parallel_bubble_sort.c
		mpicc -o p_bsort parallel_bubble_sort.c