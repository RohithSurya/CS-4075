#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h> 

int main(void) {
   int comm_sz, my_rank;
   double* a;
   MPI_Comm comm;
   
   MPI_Init(NULL, NULL);
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
   MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

   a = malloc(sizeof(double));

    if (my_rank==0) {
        printf("Enter a");
        scanf("%lf", a);
    }

    MPI_Bcast(a, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if(my_rank==1)
        printf("%lf", *a);
   
   MPI_Finalize();
   return 0;
}