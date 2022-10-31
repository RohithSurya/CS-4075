#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char **argv){
    int rank, mpisize, total_size = 16;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpisize);
    int *data = NULL;   

    if(rank == 0){
       data = malloc(total_size * sizeof(int));
       for(int i = 0; i < total_size; i++)
          data[i] = i;
    }
    int size_per_process = total_size / mpisize;
    int *localdata = malloc(size_per_process * sizeof(int));
    MPI_Scatter(data, size_per_process, MPI_INT, localdata, size_per_process, MPI_INT, 0, MPI_COMM_WORLD);

    int key = 1;
    int temp = 0;
    while(key <= mpisize/2){                      
      if((rank+1) % key == 0){
          if(rank/key % 2 == 0){    
             temp = localdata[size_per_process-1];
             MPI_Send(&temp, 1, MPI_INT, rank+key, 0, MPI_COMM_WORLD);
          }
          else {
             MPI_Recv(&temp, 1, MPI_INT, rank-key, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
             localdata[size_per_process-1]+= temp;
         }
      }
      key = 2 * key;
      MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Gather(localdata, size_per_process, MPI_INT, data, size_per_process, MPI_INT, 0, MPI_COMM_WORLD);

    if(rank == 0){
       for(int i = 0; i < total_size; i++)
               printf("%d ", data[i]);
       printf("\n");
    }
    free(data);
    free(localdata);    
    MPI_Finalize();
    return 0;
}