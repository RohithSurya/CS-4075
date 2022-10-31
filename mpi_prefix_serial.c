#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void Read_n(int *n_p, int *local_n_p, int my_rank, int comm_sz,
            MPI_Comm comm);

void Read_data(double local_vec1[], double local_vec2[], int local_n, int my_rank, int comm_sz, MPI_Comm comm);

void Print_vector(double local_vec[], int local_n, int n, char title[], int my_rank, MPI_Comm comm);

void vector_add(double local_vec1[], double local_vec2[], int local_n);

int main(void)
{
#ifndef ONLINE_JUDGE
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
#endif

    int n;
    double *a=NULL;
    int comm_sz, my_rank;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    printf("Enter the no of elements\n");
    scanf("%d", &n);
    a = malloc(n*sizeof(double));
    printf("Enter the elements");
    for(int i=0; i<n; i++) {
        scanf("%lf", &a[i]);
        if(i!=0)
            a[i]+=a[i-1];
    }
    for(int i=0; i<n; i++) {
        printf("%lf ", a[i]);
    }
    printf("\n");

    MPI_Finalize();
    // free(local_vec1);
    // free(local_vec2);
    return 0;
}