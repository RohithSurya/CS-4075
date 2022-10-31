#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>


void pi_estimator(int number_of_tosses, int my_rank, int comm_sz, MPI_Comm comm);
void Read_n(int *n_p, int *local_n_p, int my_rank, int comm_sz,
            MPI_Comm comm);

int main(void)
{
// #ifndef ONLINE_JUDGE
//     freopen("input.txt", "r", stdin);
//     freopen("output.txt", "w", stdout);
// #endif

    int n, local_n;
    int comm_sz, my_rank;
    MPI_Comm comm;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    comm = MPI_COMM_WORLD;

    Read_n(&n, &local_n, my_rank, comm_sz, comm);
    pi_estimator(local_n, my_rank, comm_sz, comm);


    MPI_Finalize();
    return 0;
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

void pi_estimator(int number_of_tosses, int my_rank, int comm_sz, MPI_Comm comm) {
    int number_in_circle=0, toss;
    double x, y, distance_squared, min_value=-1;
    int total_in_circles=0;
    srand(time(NULL));
    for(toss=0; toss<number_of_tosses; toss++) {
        x = min_value + (double)rand()/RAND_MAX;
        y = min_value + (double)rand()/RAND_MAX;
        distance_squared = x*x + y*y;
        if(distance_squared<=1) number_in_circle++;
    }

    MPI_Reduce(&number_in_circle, &total_in_circles, 1, MPI_INT, MPI_SUM, 0, comm);
    if(my_rank==0) {
        double pi_estimate;
        pi_estimate = 4*total_in_circles/(double)(number_of_tosses*comm_sz);
        printf("Estimated PI value %lf", pi_estimate);
    }
}