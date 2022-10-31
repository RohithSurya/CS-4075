#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void Read_n(int *n_p, int *local_n_p, int my_rank, int comm_sz,
            MPI_Comm comm);
void Check_for_error(int local_ok, char fname[], char message[],
                     MPI_Comm comm);
void Read_data(double local_vec1[], double local_vec2[], double *scalar_p,
               int local_n, int my_rank, int comm_sz, MPI_Comm comm);
void Print_vector(double local_vec[], int local_n, int n, char title[],
                  int my_rank, MPI_Comm comm);
double Par_dot_product(double local_vec1[], double local_vec2[],
                       int local_n, MPI_Comm comm);
void Par_vector_scalar_mult(double local_vec[], double scalar,
                            double local_result[], int local_n);

void Print_dot_product(double dot_product, int my_rank, MPI_Comm comm);

int main(void)
{
   int n, local_n;


   #ifndef ONLINE_JUDGE
  freopen("input.txt", "r", stdin);
  freopen("output.txt", "w", stdout);
#endif

   double *local_vec1, *local_vec2;
   double scalar;
   double *local_scalar_mult1, *local_scalar_mult2;
   double dot_product;
   int comm_sz, my_rank;
   MPI_Comm comm;

   MPI_Init(NULL, NULL);
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
   MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
   comm = MPI_COMM_WORLD;

   /* Print input data */
   Read_n(&n, &local_n, my_rank, comm_sz, comm);
   // printf("%d", local_n);
   local_vec1 = malloc(local_n * sizeof(double));
   local_vec2 = malloc(local_n * sizeof(double));

   local_scalar_mult1 = malloc(local_n * sizeof(double));
   local_scalar_mult2 = malloc(local_n * sizeof(double));
   Read_data(local_vec1, local_vec2, &scalar, local_n, my_rank, comm_sz, comm);
   dot_product = Par_dot_product(local_vec1, local_vec2, local_n, comm);
   Par_vector_scalar_mult(local_vec1, scalar, local_scalar_mult1, local_n);
   Par_vector_scalar_mult(local_vec2, scalar, local_scalar_mult2, local_n);
   Print_dot_product(dot_product, my_rank, comm);
   Print_vector(local_scalar_mult1, local_n, n, "Scalar Mult Vec 1:", my_rank, comm);
   Print_vector(local_scalar_mult2, local_n, n, "Scalar Mult Vec 2:", my_rank, comm);

       /* Print results */

       /* Compute and print dot product */

       /* Compute scalar multiplication and print out result */

   free(local_scalar_mult2);
   free(local_scalar_mult1);
   free(local_vec2);
   free(local_vec1);

   MPI_Finalize();
   return 0;
}


void Print_dot_product(double dot_product, int my_rank, MPI_Comm comm) {
   double total_int = 0;
   MPI_Reduce(&dot_product, &total_int, 1, MPI_DOUBLE, MPI_SUM, 0, comm);
   if(my_rank==0)
      printf("Dot product: %lf\n", total_int);
}

/*-------------------------------------------------------------------*/
void Check_for_error(
    int local_ok /* in */,
    char fname[] /* in */,
    char message[] /* in */,
    MPI_Comm comm /* in */)
{
   int ok;

   MPI_Allreduce(&local_ok, &ok, 1, MPI_INT, MPI_MIN, comm);
   if (ok == 0)
   {
      int my_rank;
      MPI_Comm_rank(comm, &my_rank);
      if (my_rank == 0)
      {
         fprintf(stderr, "Proc %d > In %s, %s\n", my_rank, fname,
                 message);
         fflush(stderr);
      }
      MPI_Finalize();
      exit(-1);
   }
} /* Check_for_error */

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
void Read_data(double local_vec1[], double local_vec2[], double *scalar_p,
               int local_n, int my_rank, int comm_sz, MPI_Comm comm)
{
   double *a = NULL, *b = NULL;
   int i;

   if (my_rank == 0)
   {
      printf("What is the scalar?\n");
      scanf("%lf", scalar_p);
   }

   MPI_Bcast(scalar_p, 1, MPI_DOUBLE, 0, comm);

   if (my_rank == 0)
   {
      a = malloc(local_n * comm_sz * sizeof(double));
      printf("Enter the first vector\n");
      for (i = 0; i < local_n * comm_sz; i++)
         scanf("%lf", &a[i]);

      MPI_Scatter(a, local_n, MPI_DOUBLE, local_vec1, local_n, MPI_DOUBLE, 0, comm);
      b = malloc(local_n * comm_sz * sizeof(double));
      printf("Enter the second vector\n");
      for (i = 0; i < local_n * comm_sz; i++)
         scanf("%lf", &b[i]);
      MPI_Scatter(b, local_n, MPI_DOUBLE, local_vec2, local_n, MPI_DOUBLE, 0, comm);
      printf("Vector 1:\n");
      for (i = 0; i < local_n * comm_sz; i++)
         printf("%lf ", a[i]);
      printf("\n");
      printf("Vector 2:\n");
      for (i = 0; i < local_n * comm_sz; i++)
         printf("%lf ", b[i]);
      printf("\n");
      printf("Scalar: %lf\n", *scalar_p);
      free(a);
      free(b);
   }
   else
   {
      MPI_Scatter(a, local_n, MPI_DOUBLE, local_vec1, local_n, MPI_DOUBLE, 0, comm);
      MPI_Scatter(b, local_n, MPI_DOUBLE, local_vec2, local_n, MPI_DOUBLE, 0, comm);
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

/* This function computes and returns the partial dot product of local_vec1 and local_vec2 */
/*-------------------------------------------------------------------*/
double Par_dot_product(double local_vec1[], double local_vec2[],
                       int local_n, MPI_Comm comm)
{
   double result = 0;
   for (int i = 0; i < local_n; i++)
   {
      result += local_vec1[i] * local_vec2[i];
   }
   // printf("Result: %lf", result);
   return result;

} /* Par_dot_product */

/* This function gets the vector which is the scalar times local_vec, and put the vector into local_result */
/*-------------------------------------------------------------------*/
void Par_vector_scalar_mult(double local_vec[], double scalar,
                            double local_result[], int local_n)
{
   for (int i = 0; i < local_n; i++)
   {
      local_result[i] = local_vec[i] * scalar;
   }
} /* Par_vector_scalar_mult */
