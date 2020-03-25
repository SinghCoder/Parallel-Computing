/*
Parallel Gaussian Algorithm submitted as a part of Assignment 1 for the course Parallel Computing
Group - 1
Group members-
Ayush Singhal - 2017A7PS0116P
Harpinder Jot Singh - 2017A7PS0057P

Notes-
1.	Format of input.txt should be-
		Size(in int)
		"Equation 1 in numerical form" "RHS of equation 1"
		"Equation 2 in numerical form" "RHS of equation 2"
		and so on
2.	Please use float or double format while specifying the equations in numerical form as using int may result in unexplained behavior of double in the program
3.	This program can only take maximum 500 different equations. Please increase matrices size if a larger number of equation is to be provided
*/
#include<stdio.h>
#include <stdlib.h>
#include <mpi/mpi.h>
#include <time.h>

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);	//Initialising the processes
    double time_start = MPI_Wtime();
    //Declaring variables to be used in the Program
    
    int i,j,k;
    int map[500];
    double input_matrix[500][500],RHS[500],ratio[500],solution[500],sum=0.0;
    int size;
    int rank, num_procs;

    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);   //Getting current process rank in rank variable
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs); //Geeting total number of generated processes in num_procs variable

    /////////////////////////////////////////////////////////////////////////////
    
    //Scanning the input matrices from "input.txt" file
    
    if (rank==0)
    {
        
        FILE *fptr;
	fptr = fopen("input.txt", "r");
	fscanf(fptr,"%d",&size);
	for(int i=0;i<size;i++){
		for (int j = 0; j < size; j++)
		{
			fscanf(fptr,"%lf", &input_matrix[i][j]);
		}
		fscanf(fptr,"%lf", &RHS[i]);
	}
	fclose(fptr);
        printf("\n Equation matrix scanned from the file:\n");
        for (i=0; i<size; i++)
        {
            for (j=0; j<size; j++)
                printf("%9.6lf ",input_matrix[i][j]);
            printf("\n");
        }
        printf("\n RHS array scanned from the file:\n");
        for (i=0; i<size; i++)
            printf("%9.6lf ",RHS[i]);
        printf("\n\n");

    }

    //////////////////////////////////////////////////////////////////////////////////

    //Decomposing the matrix to apply gaussian algorithm on the rows assigned to an individual process using map array
    
    MPI_Bcast(&size,1,MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast (&input_matrix[0][0],500*500,MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Bcast (RHS,size,MPI_DOUBLE,0,MPI_COMM_WORLD);    

    for(i=0; i<size; i++)
    {
        map[i]= i % num_procs;
    } 

    for(k=0;k<size;k++)
    {
        MPI_Bcast (&input_matrix[k][k],size-k,MPI_DOUBLE,map[k],MPI_COMM_WORLD);
        MPI_Bcast (&RHS[k],1,MPI_DOUBLE,map[k],MPI_COMM_WORLD);
        for(i= k+1; i<size; i++) 
        {
            if(map[i] == rank)
            {
                ratio[i]=input_matrix[i][k]/input_matrix[k][k];
                for(j=0;j<size;j++)
                {
                    input_matrix[i][j]=input_matrix[i][j]-( ratio[i]*input_matrix[k][j] );
                }
                RHS[i]=RHS[i]-( ratio[i]*RHS[k] );
            }
        }               
        
    }

    //////////////////////////////////////////////////////////////////////////////////

    //Back Substitution of solution from bottom up in rank 0 process
    
    if (rank==0)
    { 
        solution[size-1]=RHS[size-1]/input_matrix[size-1][size-1];
        for(i=size-2;i>=0;i--)
        {
            sum=0;

            for(j=i+1;j<size;j++)
            {
                sum=sum+input_matrix[i][j]*solution[j];
            }
            solution[i]=(RHS[i]-sum)/input_matrix[i][i];
        }


    }
    //////////////////////////////////////////////////////////////////////////////////
    
    //Printing the Solution
    
    if (rank==0)
    { 
        printf("\nThe solution is:");
        for(i=0;i<size;i++)
        {
            printf("\nx%d=%f\t",i,solution[i]);

        }

        
    }
    
    double time_elapsed = MPI_Wtime() - time_start;
    double execution_time;
    MPI_Reduce(&time_elapsed, &execution_time, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if(rank == 0){              
        printf("\nTime taken by parallel gaussian elimination : %lf\n", execution_time);
    }

    MPI_Finalize();
    return(0);


}