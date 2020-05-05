#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

void usage(char* program) {
   fprintf(stderr, "usage: %s matrixAInput.txt matrixBInput.txt\n", program);
   exit(EXIT_FAILURE);
} 

void multiplyMatrices(double **matA, double **matB, double **matC, int mA, int mB, int nA, int nB) {

    for(int i = 0; i < mA; i++){
        for(int j = 0; j < nB; j++){

            matC[i][j] = 0;

            for(int k = 0; k < nA; k++){
                matC[i][j] += matA[i][k] * matB[k][j];
            }
        }
    }
}

int main(int argc, char *argv[], char** environ)
{
    if(argc != 3){
        usage(argv[0]);
    }

    double timeSerial, timeParallel;
	struct timeval start, end;

    char *fileA = argv[1];
    char *fileB = argv[2];
    
    FILE *matAFile = fopen(fileA, "r");
    FILE *matBFile = fopen(fileB, "r");

    int mA, nA, mB, nB;

    fscanf(matAFile, "%d %d\n", &mA, &nA);
    fscanf(matBFile, "%d %d\n", &mB, &nB);
    
    if(nA != mB){
        fprintf(stderr, "matrix A's nummber of columns must be same as matrix B's nummber of rows\n");
        exit(EXIT_FAILURE);
    }

    int numThreads;
    
    
    double **matA = malloc( sizeof(double*) * mA);

    for(int i = 0; i < mA; i++){
        matA[i] = malloc( sizeof(double) * nA);

        for(int j = 0; j < nA; j++){            
            fscanf(matAFile, "%lf", &matA[i][j]);
        }

        fscanf(matAFile, "\n");
    }

    double **matB = malloc( sizeof(double) * mB);

    for(int i = 0; i < mB; i++){
        matB[i] = malloc( sizeof(double) * nB);

        for(int j = 0; j < nB; j++){            
            fscanf(matBFile, "%lf", &matB[i][j]);
        }

        fscanf(matBFile, "\n");
    }

    // printf("Matrix A is \n");
    // for(int i = 0; i < mA; i++){
    //     for(int j = 0; j < nA; j++){
    //         printf("%lf ", matA[i][j]);
    //     }
    //     printf("\n");
    // }

    // printf("Matrix B is \n");
    // for(int i = 0; i < mB; i++){
    //     for(int j = 0; j < nB; j++){
    //         printf("%lf ", matB[i][j]);
    //     }
    //     printf("\n");
    // }

    double **matC = malloc(sizeof(double *) * mA);

    for(int i = 0; i < mA; i++){
        matC[i] = malloc(sizeof(double) * nB);
        memset(matC[i], 0, sizeof(double) *nB);
    }

    for(int a = 1; a < 300; a++){
    
        numThreads = a;
        printf("%d,", numThreads);

        gettimeofday(&start, NULL);

            multiplyMatrices(matA, matB, matC, mA, mB, nA, nB);

        gettimeofday(&end, NULL);

        timeSerial = ((end.tv_sec  - start.tv_sec) * 1000000u +  end.tv_usec - start.tv_usec) / 1.e6;

        printf("%lf,", timeSerial);
        
        gettimeofday(&start, NULL);
            int i, j, k;
            #pragma omp parallel num_threads(numThreads) private(i, j, k) shared(matA, matB, matC, mA, nA, mB, nB)
                #pragma omp for collapse(3)
                for(i = 0; i < mA; i++){
                    for(j = 0; j < nB; j++){
                        for(k = 0; k < nA; k++){
                            matC[i][j] += matA[i][k] * matB[k][j];
                        }
                    }
                }

        gettimeofday(&end, NULL);

        timeParallel = ((end.tv_sec  - start.tv_sec) * 1000000u +  end.tv_usec - start.tv_usec) / 1.e6;

        printf("%lf,", timeParallel);
        printf("%lf\n", timeSerial/timeParallel);
    }
    // printf("Matrix C is \n");
    // for(int i = 0; i < mA; i++){
    //     for(int j = 0; j < nB; j++){
    //         printf("%lf ", matC[i][j]);
    //     }
    //     printf("\n");
    // }

    return 0;
}