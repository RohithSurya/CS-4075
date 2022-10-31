#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>


void Read_n(int *nbins_p, double* min_value_p, double* max_value_p, int* nvals_p, int my_rank, int comm_sz, MPI_Comm comm);
void Read_data(double local_vec[], int local_n, double min_value, double max_value, int my_rank, int comm_sz, MPI_Comm comm);
int find_bin(double data, double* bin_maxes, int bin_count, double min_meas);
void Print_vector(int bin_counts[], int nbins, char title[], double min_value, double bin_maxes[], int my_rank, MPI_Comm comm);
double* generateRandomNumbers(int nvals, double max_value, double min_value);

int main(void)
{


    // #ifndef ONLINE_JUDGE
    //     freopen("input.txt", "r", stdin);
    //     // freopen("output.txt", "w", stdout);
    // #endif
    int nbins, nvals, local_n, bin;
    double min_value, max_value, bin_width;
    int comm_sz, my_rank;
    double* local_vec;
    double* bin_maxes;
    int* bin_counts;
    MPI_Comm comm;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    comm = MPI_COMM_WORLD;

    /* Print input data */
    Read_n(&nbins, &min_value, &max_value, &nvals, my_rank, comm_sz, comm);
    local_n = nvals/comm_sz;
    local_vec =  (double*)malloc(local_n * sizeof(double));
    Read_data(local_vec, local_n, min_value, max_value, my_rank, comm_sz, comm);
    bin_width = (max_value - min_value)/nbins;
    bin_maxes = (double*)malloc(nbins * sizeof(double));
    bin_counts = malloc(nbins * sizeof(int));
    for(int b=0; b<nbins; b++) {
        bin_counts[b]=0;
        bin_maxes[b]=min_value + bin_width*(b+1);
    }


    for(int i=0; i<local_n; i++) {
        bin = find_bin(local_vec[i], bin_maxes, nbins, min_value);
        bin_counts[bin]++;
    }

    Print_vector(bin_counts, nbins, "fda", min_value, bin_maxes, my_rank, comm);

    // free(bin_counts);
    // free(bin_counts);
    // free(local_vec);

    MPI_Finalize();
    return 0;
}


int find_bin(double data, double* bin_maxes, int bin_count, double min_meas) {
    if(min_meas<= data && data<bin_maxes[0]) return 0;
    for(int i=1; i<bin_count; i++) {
        if(bin_maxes[i-1]<=data && data<bin_maxes[i]) {
            return i;
        }
    }
    return 1;
}


void Read_data(double local_vec[], int local_n, double min_value, double max_value, int my_rank, int comm_sz, MPI_Comm comm)
{
    double *a = NULL;

    if (my_rank == 0)
    {
        a = generateRandomNumbers(local_n*comm_sz, min_value, max_value);
        MPI_Scatter(a, local_n, MPI_DOUBLE, local_vec, local_n, MPI_DOUBLE, 0, comm);
        free(a);

    } else {
        MPI_Scatter(a, local_n, MPI_DOUBLE, local_vec, local_n, MPI_DOUBLE, 0, comm);
    }    
}


double* generateRandomNumbers(int nvals, double min_value, double max_value) {
    srand(time(NULL));
    double range = max_value - min_value;
    double *r_vals = malloc(nvals * sizeof(double));
    printf("Data: ");
    for(int i=0; i<nvals; i++) {
        r_vals[i] = min_value + (double)rand()/RAND_MAX*range;
        printf("%.3lf ", r_vals[i]);
    }
    printf("\n");

    return r_vals;
    
}

void Read_n(int *nbins_p, double* min_value_p, double* max_value_p, int* nvals_p, int my_rank, int comm_sz, MPI_Comm comm)
{
    if (my_rank == 0)
    {
        printf("Enter the Number of bins\n");
        scanf("%d", nbins_p);
        printf("Enter Minimum value\n");
        scanf("%lf", min_value_p);
        printf("Enter Maximum value\n");
        scanf("%lf", max_value_p);
        printf("Number of random values to be used:(Value should be divisible by %d)\n", comm_sz);
        scanf("%d", nvals_p);
    }
    MPI_Bcast(nbins_p, 1, MPI_INT, 0, comm);
    MPI_Bcast(min_value_p, 1, MPI_DOUBLE, 0, comm);
    MPI_Bcast(max_value_p, 1, MPI_DOUBLE, 0, comm);
    MPI_Bcast(nvals_p, 1, MPI_DOUBLE, 0, comm);
}

void Print_vector(int bin_counts[], int nbins, char title[], double min_value, double bin_maxes[], int my_rank, MPI_Comm comm)
{
    int *a = NULL;
    int i;
    if (my_rank == 0)
    {
        a = malloc(nbins * sizeof(int));
        MPI_Reduce(bin_counts, a, nbins, MPI_INT, MPI_SUM, 0, comm);
        // printf("%s", title);
        printf("[\t%.3lf|%.3lf)  %d\n", min_value, bin_maxes[0], a[0]);
        for (i = 1; i < nbins; i++)
        {
            printf("[\t%.3lf|%.3lf)  %d\n", bin_maxes[i-1], bin_maxes[i], a[i]);
        }
        printf("\n");
        free(a);
    }
    else
    {
        MPI_Reduce(bin_counts, a, nbins, MPI_INT, MPI_SUM, 0, comm);
    }
}