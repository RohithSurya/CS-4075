#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>


void Read_n(int *n_p, int* local_np, double* a_p, double* b_p, int my_rank, int comm_sz, MPI_Comm comm);
double func(double a);
double Trap(int n, double a, double b);

int main(void)
{


    // #ifndef ONLINE_JUDGE
    //     freopen("input.txt", "r", stdin);
    //     // freopen("output.txt", "w", stdout);
    // #endif
    int n, local_n;
    double a, b, h, local_a, local_b, local_int, total_int=0;
    int comm_sz, my_rank;
    MPI_Comm comm;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    comm = MPI_COMM_WORLD;

    /* Print input data */
    Read_n(&n, &local_n, &a, &b, my_rank, comm_sz, comm);

    // printf("For process %d the a is %lf\n", my_rank, a);
    // printf("For process %d the b is %lf\n", my_rank, b);

    h=(b-a)/n;
    local_a = a + my_rank*local_n*h;
    local_b = local_a + local_n*h;

    // printf("For process %d the local_a is %lf\n", my_rank, local_a);
    // printf("For process %d the local_b is %lf\n", my_rank, local_b);

    local_int = Trap(local_n, local_a, local_b);

    MPI_Reduce(&local_int, &total_int, 1, MPI_DOUBLE, MPI_SUM, 0, comm);

    if(my_rank==0)
      printf("Integration: %lf\n", total_int);
    /* Print results */

    /* Compute and print dot product */

    /* Compute scalar multiplication and print out result */
    // free(local_int);

    MPI_Finalize();
    return 0;
}
    

double Trap(int n, double a, double b) {
    double h = (b-a)/n;
    double x_i;
    double approx = (func(a)+func(b))/2;
    for(int i=1; i<=n-1; i++) {
        x_i = a + i*h;
        approx+=func(x_i);
    }
    approx*=h;
    return approx;
}

double func(double a) {
    return a*a;
}

void Read_n(int *n_p, int* local_np, double* a_p, double* b_p, int my_rank, int comm_sz, MPI_Comm comm)
{
    if (my_rank == 0)
    {
        printf("What are the no of partitions?\n");
        scanf("%d", n_p);
        printf("What is a\n");
        scanf("%lf", a_p);
        printf("What is b\n");
        scanf("%lf", b_p);
        *local_np = *n_p / comm_sz;
    }
    MPI_Bcast(n_p, 1, MPI_INT, 0, comm);
    MPI_Bcast(a_p, 1, MPI_DOUBLE, 0, comm);
    MPI_Bcast(b_p, 1, MPI_DOUBLE, 0, comm);
    *local_np = *n_p / comm_sz;
    // printf("For process %d the local_b is %d\n", my_rank, *local_np);
}