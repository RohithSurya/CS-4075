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
long long n;
long long total_in_circles=0;

void Usage(char* progname);

void Get_args(int argc, char* argv[]);

/* Thread function */
void* Thread_work(void* rank);

/*-----------------------------------------------------------------*/
int main(int argc, char* argv[]) {
	long thread;
	pthread_t* thread_handles;
	message = malloc(MAX_STRING*sizeof(char));
	
	// if(argc != 1) Usage(argv[0]);
	
	// thread_count = 4;
	Get_args(argc, argv);

    printf("Enter the number of tosses\n");
    scanf("%lld", &n);
	
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

    double pi_estimate;
    pi_estimate = 4*total_in_circles/(double)n;
    printf("Estimated PI value %lf", pi_estimate);

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
    long long number_of_tosses = n/thread_count;
    int number_in_circle=0, toss;
    double x, y, distance_squared, min_value=-1;
    srand(time(NULL));
    for(toss=0; toss<number_of_tosses; toss++) {
        x = min_value + 2 * (double)rand()/RAND_MAX;
        y = min_value + 2 * (double)rand()/RAND_MAX;
        distance_squared = x*x + y*y;
        if(distance_squared<=1) number_in_circle++;
    }

    pthread_mutex_lock(&mutex);
    total_in_circles+=number_in_circle;
    pthread_mutex_unlock(&mutex);
	return NULL;
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
