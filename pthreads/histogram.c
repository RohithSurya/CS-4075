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
pthread_mutex_t mutex;


int nbins, nvals;
double min_value, max_value, bin_width;
double* bin_maxes;
int* bin_counts;
double *a = NULL;



void Usage(char* progname);

void Get_args(int argc, char* argv[]);

/* Thread function */
void* Thread_work(void* rank);

void Read_n();
int find_bin(double data, double* bin_maxes, int bin_count, double min_meas);
double* generateRandomNumbers();
void Print_vector();

/*-----------------------------------------------------------------*/
int main(int argc, char* argv[]) {
	long thread;
	pthread_t* thread_handles;
	message = malloc(MAX_STRING*sizeof(char));
	
	// if(argc != 1) Usage(argv[0]);
	
	// thread_count = 4;
	Get_args(argc, argv);
    Read_n();

    bin_width = (max_value - min_value)/nbins;
    bin_maxes = (double*)malloc(nbins * sizeof(double));
    bin_counts = malloc(nbins * sizeof(int));
    for(int b=0; b<nbins; b++) {
        bin_counts[b]=0;
        bin_maxes[b]=min_value + bin_width*(b+1);
    }

    a = generateRandomNumbers();
	
	/* allocate array for threads */
	thread_handles = malloc(thread_count*sizeof(pthread_t));
	
	/* initialize mutex */
	pthread_mutex_init(&mutex, NULL);
	
	/* start threads */
	for(thread = 0; thread < thread_count; thread++) {
		pthread_create(&thread_handles[thread], NULL, Thread_work, 
					   (void*) thread);
	}
	
	/* wait for threads to complete */
	for(thread = 0; thread < thread_count; thread++) {
		pthread_join(thread_handles[thread], NULL);
	}

    Print_vector();


	pthread_mutex_destroy(&mutex);
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
    int bin;
    long my_rank = (long)rank;

    int local_n = nvals/thread_count;
    long long my_first_i = local_n*my_rank;
    long long my_last_i = my_first_i+local_n;

    
    for(int i=my_first_i; i<my_last_i; i++) {
        bin = find_bin(a[i], bin_maxes, nbins, min_value);
        pthread_mutex_lock(&mutex);
        bin_counts[bin]++;
        pthread_mutex_unlock(&mutex);
    }
	return NULL;
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

void Read_n()
{
    printf("Enter the Number of bins\n");
    scanf("%d", &nbins);
    printf("Enter Minimum value\n");
    scanf("%lf", &min_value);
    printf("Enter Maximum value\n");
    scanf("%lf", &max_value);
    printf("Number of random values to be used:(Value should be divisible by %d)\n", thread_count);
    scanf("%d", &nvals);
}


double* generateRandomNumbers() {
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


void Print_vector()
{
    int i=0;
    printf("[%.3lf|%.3lf)  %d\n", min_value, bin_maxes[0], bin_counts[0]);
    for (i = 1; i < nbins; i++)
    {
        printf("[%.3lf|%.3lf)  %d\n", bin_maxes[i-1], bin_maxes[i], bin_counts[i]);
    }
    printf("\n");
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
