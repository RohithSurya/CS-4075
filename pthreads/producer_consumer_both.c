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
int receiver=1;
char* message;
pthread_mutex_t mutex;

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
	Get_args(argc, argv);
	
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
	long my_rank = (long) rank;
	int send=1;
	int recv=1;
	
	while(1) {
		pthread_mutex_lock(&mutex);
		if(send==1 && (my_rank+1)%thread_count == receiver && msg==0) {
			sprintf(message, "hello from %ld", my_rank);
			msg=1;
			send=0;
			pthread_mutex_unlock(&mutex);
		}

		if(recv==1 && my_rank==receiver && msg==1) {
			printf("Th %ld > message = %s\n", my_rank, message);
			receiver = (receiver+1)%thread_count;
			msg=0;
			recv=0;
			pthread_mutex_unlock(&mutex);
		}
		if(send==0 && recv==0) {
			pthread_mutex_unlock(&mutex);
			break;
		}
		pthread_mutex_unlock(&mutex);
	}
	
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
