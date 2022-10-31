#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void Read_n(int *n_p, int *local_n_p, int my_rank, int comm_sz,
            MPI_Comm comm);

void Read_data(double local_vec1[], int local_n, int my_rank, int comm_sz, MPI_Comm comm);

void Print_vector(double local_vec[], int local_n, int n, char title[], int my_rank, MPI_Comm comm);

void Compute_prefix(double local_vec[], double result_vec[], int local_n, int my_rank, int comm_sz, MPI_Comm comm);

int main(void)
{
// #ifndef ONLINE_JUDGE
//     freopen("input.txt", "r", stdin);
//     // freopen("output.txt", "w", stdout);
// #endif

    int n, local_n;
    double *local_vec1;
    int comm_sz, my_rank;
    MPI_Comm comm;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    comm = MPI_COMM_WORLD;

    Read_n(&n, &local_n, my_rank, comm_sz, comm);
    if (my_rank == 0)
    {
        printf("The n is %d\n", n);
        printf("The local n is %d\n", local_n);
    }

    local_vec1 = malloc(local_n * sizeof(double));

    Read_data(local_vec1, local_n, my_rank, comm_sz, comm);
    for(int i=1; i<local_n; i++) {
        local_vec1[i]+=local_vec1[i-1];
    }
    double temp, offset;
    MPI_Scan(&local_vec1[local_n-1], &temp, 1, MPI_DOUBLE, MPI_SUM, comm);
    offset = temp - local_vec1[local_n-1];
    local_vec1[local_n-1] = temp;
    for(int i=0; i<local_n-1; i++)
        local_vec1[i]+=offset;
    Print_vector(local_vec1, local_n, n, "Prefix vector", my_rank, comm);

    MPI_Finalize();
    free(local_vec1);
    return 0;
}


void Compute_prefix(double local_vec[], double result_vec[], int local_n, int my_rank, int comm_sz, MPI_Comm comm) {

        if(my_rank==0) {
        printf("fda");
        for(int i=0; i<local_n; i++)
        printf("%lf ", local_vec[i]);
        printf("\n");
    }
}


/* Get the input of n: size of the vectors, and then calculate local_n according to comm_sz and n */
/* where local_n is the number of elements each process obtains */
/*-------------------------------------------------------------------*/
void Read_n(int *n_p, int *local_n_p, int my_rank, int comm_sz,
            MPI_Comm comm)
{
    if (my_rank == 0)
    {
        printf("What is the size of the vector?\n");
        scanf("%d", n_p);
        *local_n_p = *n_p / comm_sz;
    }
    MPI_Bcast(local_n_p, 1, MPI_INT, 0, comm);

} /* Read_n */

/* local_vec1 and local_vec2 are the two local vectors of size local_n which the process pertains */
/* process 0 will take the input of the scalar, the two vectors a and b */
/* process 0 will scatter the two vectors a and b across all processes */
/*-------------------------------------------------------------------*/
void Read_data(double local_vec1[], int local_n, int my_rank, int comm_sz, MPI_Comm comm)
{
    double *a = NULL;
    int i;
    if (my_rank == 0)
    {
        a = malloc(local_n * comm_sz * sizeof(double));
        printf("Enter the first vector\n");
        for (i = 0; i < local_n * comm_sz; i++)
            scanf("%lf", &a[i]);
        printf("Vector 1:\n");
        for (i = 0; i < local_n * comm_sz; i++)
            printf("%lf ", a[i]);
        printf("\n");
        MPI_Scatter(a, local_n, MPI_DOUBLE, local_vec1, local_n, MPI_DOUBLE, 0, comm);
        free(a);
    }
    else
    {
        MPI_Scatter(a, local_n, MPI_DOUBLE, local_vec1, local_n, MPI_DOUBLE, 0, comm);
    }
} /* Read_data */



/* The print_vector gathers the local vectors from all processes and print the gathered vector */
/*-------------------------------------------------------------------*/
void Print_vector(double local_vec[], int local_n, int n, char title[],
                  int my_rank, MPI_Comm comm)
{
   double *a = NULL;
   int i;


   if (my_rank == 0)
   {
      a = malloc(n * sizeof(double));
      MPI_Gather(local_vec, local_n, MPI_DOUBLE, a, local_n, MPI_DOUBLE, 0, comm);
      printf("%s\n", title);
      for(i=0; i<n; i++) {
         printf("%lf ", a[i]);
      }
      printf("\n");
      free(a);
   }
   else {
      MPI_Gather(local_vec, local_n, MPI_DOUBLE, a, local_n, MPI_DOUBLE, 0, comm);
   }
} /* Print_vector */