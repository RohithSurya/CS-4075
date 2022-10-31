#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void Read_n(int *n_p, int *local_n_p, int my_rank, int comm_sz,
            MPI_Comm comm);

void Read_data(double local_vec1[], double local_vec2[], int local_n, int my_rank, int comm_sz, MPI_Comm comm);

void Print_vector(double local_vec[], int local_n, int n, char title[], int my_rank, MPI_Comm comm);


void matrix_vect_multiply(double local_vec1[], double local_vec2[], int local_n, int comm_sz, double result[]);

int main(void)
{
    int n, local_n;
    double* local_vec1;
    double *local_vec2, *result;
    int comm_sz, my_rank;
    MPI_Comm comm;

    // #ifndef ONLINE_JUDGE
        // freopen("input.txt", "r", stdin);
    //     freopen("output.txt", "w", stdout);
    // #endif

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    comm = MPI_COMM_WORLD;

    Read_n(&n, &local_n, my_rank, comm_sz, comm);

    local_vec1 = (double*)malloc(local_n * local_n *comm_sz * sizeof(double));
    local_vec2 = (double*)malloc(local_n * comm_sz * sizeof(double));
    result = (double*)malloc(local_n * comm_sz * sizeof(double));

    Read_data(local_vec1, local_vec2, local_n, my_rank, comm_sz, comm);
    matrix_vect_multiply(local_vec1, local_vec2, local_n, comm_sz, result);
    Print_vector(result, local_n, n, "Result matrix", my_rank, comm);

    MPI_Finalize();
    free(local_vec1);
    free(local_vec2);
    free(result);
    return 0;
}


void matrix_vect_multiply(double local_vec1[], double local_vec2[], int local_n, int comm_sz, double result[]) {
    int n=local_n*comm_sz;
    for(int i=0; i<local_n; i++) {
        result[i]=0;
        for(int j=0; j<n; j++) {
            result[i] += local_vec1[i*n+j] * local_vec2[j];
        }
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
void Read_data(double local_vec1[], double local_vec2[], int local_n, int my_rank, int comm_sz, MPI_Comm comm)
{
    double *a=NULL;
    int i, j;
    int n = local_n * comm_sz;
    if (my_rank == 0)
    {
        a = (double*)malloc(n * n * sizeof(double));
        printf("Enter the Matrix\n");
        for (i = 0; i < n * n; i++)
            scanf("%lf", &a[i]);
        printf("Matrix:\n");
        for (i = 0; i < n; i++) {
            for(j=0; j<n; j++) {
                printf("%lf ", a[i*n+j]);
            }
            printf("\n");    
        }
        printf("\n");

        MPI_Scatter(a, n*n/comm_sz, MPI_DOUBLE, local_vec1, n*n/comm_sz, MPI_DOUBLE, 0, comm);

        printf("Enter the vector\n");
        for (i = 0; i < n; i++)
            scanf("%lf", &local_vec2[i]);
        printf("Vector:\n");
        for (i = 0; i < n; i++)
            printf("%lf ", local_vec2[i]);
        printf("\n");   
        free(a);
    }
    else
    {
        MPI_Scatter(a, n*n/comm_sz, MPI_DOUBLE, local_vec1, n*n/comm_sz, MPI_DOUBLE, 0, comm);
    }
    MPI_Bcast(local_vec2, n, MPI_DOUBLE, 0, comm);
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