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
pthread_mutex_t mutex;


int n;
double a, b, h;



void Usage(char* progname);

void Get_args(int argc, char* argv[]);

/* Thread function */
void* Thread_work(void* rank);

void Read_n();
double total_approx=0;
double func(double a);
void Read_n();
double Trap(int n, double a, double b);

/*-----------------------------------------------------------------*/
int main(int argc, char* argv[]) {
	long thread;
	pthread_t* thread_handles;
	
	// if(argc != 1) Usage(argv[0]);
	
	// thread_count = 4;
	Get_args(argc, argv);
    Read_n();

    h=(b-a)/n;

	
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

    printf("The integration response %lf\n", total_approx);



	pthread_mutex_destroy(&mutex);
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
    int local_n = n/thread_count;
    double local_a = a + my_rank*local_n*h;
    double local_b = local_a + local_n*h;
    double approx = Trap(local_n, local_a, local_b);
    pthread_mutex_lock(&mutex);
    total_approx+=approx;
    pthread_mutex_unlock(&mutex);
    return NULL;
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

void Read_n()
{
    printf("What are the no of partitions?\n");
    scanf("%d", &n);
    printf("What is a\n");
    scanf("%lf", &a);
    printf("What is b\n");
    scanf("%lf", &b);
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