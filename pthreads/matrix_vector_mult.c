/* File:     
 *     4.3_pth_producer_comsumer.c
 *
 * Purpose:  
 *     Implement producer-consumer synchronization with two threads using 
 *        a mutex
 *
 * Input:
 *     None
 *
 * Output:
 *     message
 *
 * Compile:  gcc -g -Wall -o 4.3_pth_producer_consumer 
 *              4.3_pth_producer_consumer.c -lpthread
 * Usage:
 *     4.3_pth_producer_consumer
 *
 * Notes:  
 *	- rank 1 create a message
 *	- rank 0 print out message
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

const int MAX_STRING = 99;

const int MAX_THREADS = 1024;

int thread_count;
int msg = 0;
char* message;
int m, n;
long long total_in_circles=0;
double **A;
double *x;
double *y;

void Usage(char* progname);

void Get_args(int argc, char* argv[]);

/* Thread function */
void* Thread_work(void* rank);

void Print_result();

/*-----------------------------------------------------------------*/
int main(int argc, char* argv[]) {
	long thread;
	pthread_t* thread_handles;
	message = malloc(MAX_STRING*sizeof(char));
	
	// if(argc != 1) Usage(argv[0]);
	
	// thread_count = 4;
	Get_args(argc, argv);

	
	/* allocate array for threads */
	thread_handles = malloc(thread_count*sizeof(pthread_t));

    printf("Enter the size of the matrix m, n\n");
    scanf("%d", &m);
    scanf("%d", &n);


    A = (double **)malloc(m * sizeof(double*));
    x = (double*) malloc(n * sizeof(double));
    y = (double*) malloc(n * sizeof(double));
    for(int i = 0; i < m; i++) 
        A[i] = (double*)malloc(n * sizeof(double));
    
    printf("Enter the matrix\n");
    for(int i=0; i<m; i++) {
        for(int j=0; j<n; j++) {
            scanf("%lf", &A[i][j]);
        }
    }

    printf("Enter the vector\n");
    for(int i=0; i<n; i++) {
        scanf("%lf", &x[i]);
    }
	
	/* start threads */
	for(thread = 0; thread < thread_count; thread++) {
		pthread_create(&thread_handles[thread], NULL, Thread_work, 
					   (void*) thread);
	}
	
	/* wait for threads to complete */
	for(thread = 0; thread < thread_count; thread++) {
		pthread_join(thread_handles[thread], NULL);
	}

    Print_result();

    free(message);
	free(thread_handles);
	
	return 0;
}
/*-------------------------------------------------------------------
 * Function:    Thread_work
 * Purpose:     Producer rank 1: create msg
 *		Consumer rank 0: print out msg
 * In arg:      rank
 * Global var:  mutex, msg, message
 */

void *Thread_work(void* rank) {
    long my_rank = (long)rank;
    int i, j;
    int local_m = m/thread_count;
    int my_first_row = my_rank*local_m;
    int my_last_row = (my_rank+1)*local_m-1;

    for(i = my_first_row; i<=my_last_row; i++) {
        y[i]=0.0;
        for(j=0; j<n; j++) {
            y[i]+=A[i][j]*x[j];
        }
    }
	return NULL;
}


void Print_result() {
    printf("Printing the result vector\n");

    for(int i=0; i<n; i++) {
        printf("%lf\n", y[i]);
    }
}


/*--------------------------------------------------------------------
 * Function:    Usage
 * Purpose:     Print command line for function and terminate
 * In arg:      prog_name
 */

void Usage(char* progname)
{
	fprintf(stderr, "Usage: %s \n", progname);
	exit(0);
}


void Get_args(int argc, char* argv[]) {
   if (argc != 2) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);  
   if (thread_count <= 0 || thread_count > MAX_THREADS) Usage(argv[0]);
}  /* Get_args */
