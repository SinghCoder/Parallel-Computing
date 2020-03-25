all: p_gaussian_elim
		mpirun --hostfile hostfile -np ${np} p_gaussian_elim

p_gaussian_elim : gaussian_parallel.c
		mpicc -o p_gaussian_elim gaussian_parallel.c