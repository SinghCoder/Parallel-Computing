#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void usage(char* program) {
   fprintf(stderr, "usage: %s num_rows num_cols file_name\n", program);
   exit(EXIT_FAILURE);
} 

int main(int argc, char *argv[])
{
    if(argc != 4){
        usage(argv[0]);
    }

    int num_rows = atoi(argv[1]);
    int num_cols = atoi(argv[2]);
    char *file_name = argv[3];

    FILE *outpFile = fopen(file_name, "w");

    float val;
    int upperLimit = 100;
    srand(time(NULL));

    fprintf(outpFile, "%d %d\n", num_rows, num_cols);

    for(int i = 0; i < num_rows; i++){
        
        for(int j = 0; j < num_cols; j++){
            val = ((float)rand()/(float)(RAND_MAX)) * upperLimit;

            if(j != num_cols-1){
                fprintf(outpFile, "%lf ", val);
            }
            else{
                fprintf(outpFile, "%lf", val);
            }
        }

        fprintf(outpFile, "\n");
    }

    fclose(outpFile);
}