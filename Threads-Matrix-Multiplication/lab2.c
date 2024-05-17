#include <stdio.h>
#include <stdnoreturn.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pthread.h>

//glopal variables
int** matrixA;
int** matrixB;
int** matrixC;
pthread_t threads[400];

//Structs
typedef struct {
    int arow;
    int acol;
    int brow;
    int bcol;
    int i;
    int j;

} ThreadArgs;

//prototype for functions
int** readMatrixFromFile(const char* filename, int* numRows, int* numCols);
int Multiply_A_B();
int write_file_C(const char* filename, int numRows, int numCols,char* method);
void Multiply_A_B_thread_per_matrix(ThreadArgs *args);
void Multiply_A_B_thread_per_row(ThreadArgs *args);
void Multiply_A_B_thread_per_element(ThreadArgs *args);
void*  get_Matrix(ThreadArgs *args);
void*  get_Row(ThreadArgs *args);
void*  get_Element(ThreadArgs *args);

//main
int main(){
   return Multiply_A_B();
}

//function implementations
int** readMatrixFromFile(const char* filename, int* numRows, int* numCols)
{
 FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return NULL;
    }


    // Read number of rows and columns from the file
    if (fscanf(file, "row=%d col=%d", numRows, numCols) != 2) {
        printf("Error reading row and column values from file.\n");
        fclose(file);
        return NULL;
    }

    // Allocate memory for the matrix
    int** matrix = (int**)malloc((*numRows) * sizeof(int*));
    for (int i = 0; i < *numRows; i++) {
        matrix[i] = (int*)malloc((*numCols) * sizeof(int));
    }

    // Read matrix elements from the file
    for (int i = 0; i < *numRows; i++) {
        for (int j = 0; j < *numCols; j++) {
    if (fscanf(file, "%d", &matrix[i][j]) != 1) {
                printf("Error reading matrix data from file.\n");
                fclose(file);
                return NULL;
            }
          }
    }

    fclose(file);
    return matrix;
}
int write_file_C(const char* filename, int numRows, int numCols,char* method)
{
    FILE *file;
    file = fopen(filename, "w"); // Open file in write mode

    if (file == NULL) {
        printf("Error opening file!\n");
        return 1;
    }
    //print Method
         fprintf(file, "Method:%s\n", method);
    
    //print number of rows and columns
         fprintf(file, "rows=%dcol=%d\n", numRows,numCols);


    // Write each element of the matrix to the file
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            fprintf(file, "%d ", matrixC[i][j]);
        }
        fprintf(file, "\n");
        fprintf(file, "\n"); // Move to the next line after writing each row
    }

    fclose(file); // Close the file
    return 0;
}
int Multiply_A_B()
{
    int aRows, aCols =0;
    int bRows, bCols =0;
        matrixA = readMatrixFromFile("/home/habiba/a.txt", &aRows, &aCols);
        matrixB = readMatrixFromFile("/home/habiba/b.txt", &bRows, &bCols);
        matrixC = (int**)malloc(aRows * sizeof(int*));
        for (int i = 0; i < aRows; i++) {
            matrixC[i] = (int*)malloc(bCols * sizeof(int));
        }

    if (matrixA == NULL || matrixB == NULL) {
        return 1; // Error reading file
    }
    if(aCols != bRows){
         printf("Can't multiply these two matrices!");
        return 1;
    }
     ThreadArgs args = {aRows, aCols , bRows, bCols ,0 ,0};

   struct timeval stop, start;

    //A thread per matrix
    gettimeofday(&start, NULL); //start checking time
    Multiply_A_B_thread_per_matrix(&args);
    gettimeofday(&stop, NULL); //end checking time
    write_file_C("/home/habiba/C1",aRows, bCols ,"A thread per matrix");
    printf("A thread per matrix\n");
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    printf("\n");

    //A thread per row
    printf("A thread per row\n");
    gettimeofday(&start, NULL); 
    Multiply_A_B_thread_per_row(&args);
    gettimeofday(&stop, NULL);
    write_file_C("/home/habiba/C2",aRows, bCols ,"A thread per row"); 
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    printf("\n");

    //A thread per element
    printf("A thread per element\n");
    gettimeofday(&start, NULL); 
    Multiply_A_B_thread_per_element(&args);
    gettimeofday(&stop, NULL); 
    write_file_C("/home/habiba/C3",aRows, bCols ,"A thread per element"); 
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    printf("\n");

    return 0;
}
void Multiply_A_B_thread_per_matrix(ThreadArgs *args)
{
   int ret= pthread_create(&threads[0], NULL, (void *)get_Matrix, args);
   if(ret != 0) 
        {
            printf("error creating thread");
            return ;
        }
    //wait for threads to finish
    pthread_join(threads[0], NULL);
}
void Multiply_A_B_thread_per_row(ThreadArgs *args)
{
    for(int j=0; j<args->arow; j++)
    {
           int ret= pthread_create(&threads[j], NULL, (void *)get_Row, args);
            if(ret != 0) 
        {
            printf("error creating thread");
            return ;
        }
    }
    for(int i=0; i<args->arow; i++)
    {
        pthread_join(threads[i], NULL);
    }

}
void Multiply_A_B_thread_per_element(ThreadArgs *args)
{
       for(int i=0; i<args->arow; i++)
       {
        for(int j=0; j<args->bcol; j++)
        {
        int ret =    pthread_create(&threads[i*20+j], NULL, (void *)get_Element, args);
        if(ret != 0) 
        {
            printf("error creating thread");
            return ;
        }
        }
       }
       for(int i=0; i<args->arow*args->bcol; i++)
       {
        
        pthread_join(threads[i], NULL);
        
       }
}
void* get_Matrix(ThreadArgs *args)
{
    //calculate the whole result matrix
    for(int i=0; i< args->arow; i++)
    {
        for(int j=0; j<args-> bcol; j++)
        {
            matrixC[i][j]=0;
            for(int k=0; k<args->acol; k++)
            {
                matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
            }
        }
    }
     //exit to notify that the thread finished its computation
   // pthread_exit(NULL);

}
void*  get_Row(ThreadArgs *args)
{
    //calculate row number i of the result matrix
    for(int j=0; j<args-> bcol; j++)
    {
        matrixC[args->i][j]=0;
        for(int k=0; k<args->acol; k++)
        {
            matrixC[args->i][j] += matrixA[args->i][k] * matrixB[k][j];
        }

    }
   //  pthread_exit(NULL);

}
void*  get_Element(ThreadArgs *args)
{
   //calculate the element of c matrix at row i ,cloumn j
    matrixC[args->i][args->j]=0;
    for(int k=0; k<args->acol; k++)
    {
        matrixC[args->i][args->j] += matrixA[args->i][k] * matrixB[k][args->j];
    }
      // pthread_exit(NULL);
}

