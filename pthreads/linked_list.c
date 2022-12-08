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
pthread_rwlock_t rwlock;

void Usage(char* progname);

void Get_args(int argc, char* argv[]);

/* Thread function */
void* Thread_work(void* rank);




struct list_node_s {
    int data;
    struct list_node_s* next;
};

int Insert(int value);
int Member(int value);
void print_linked_list();
int Delete(int value);

struct list_node_s *head = NULL;

/*-----------------------------------------------------------------*/
int main(int argc, char* argv[]) {
	long thread;
	pthread_t* thread_handles;
	message = malloc(MAX_STRING*sizeof(char));
	
	// if(argc != 1) Usage(argv[0]);
	
	// thread_count = 4;
	Get_args(argc, argv);
    pthread_rwlock_init(&rwlock, NULL);

    // Insert(2);
    // Insert(4);
    // printf("%d", Member(4, head));
    // print_linked_list(head);
    // Delete(4, &head);
    // print_linked_list(head);
    // printf("%d", Member(4, head));

	
	/* allocate array for threads */
     thread_handles = malloc(thread_count*sizeof(pthread_t));

	
	/* start threads */
	for(thread = 0; thread < thread_count; thread++) {
		pthread_create(&thread_handles[thread], NULL, Thread_work, 
					   (void*) thread);
	}
	
	/* wait for threads to complete */
	for(thread = 0; thread < thread_count; thread++) {
		pthread_join(thread_handles[thread], NULL);
	}

    // Print_result();
    pthread_rwlock_destroy(&rwlock);
    // print_linked_list(head);

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
    int my_rank = (int)rank;
    srand(time(NULL));
    
    for(int i=0; i<n; i++) {
        int operation =  (int)3*((double)rand()/RAND_MAX);
        // printf("%d operation\n", operation);
            switch(operation) {
            case 0:
            pthread_rwlock_rdlock(&rwlock);
            Member(my_rank);
            printf("After member read %d\n", my_rank);
            print_linked_list();
            pthread_rwlock_unlock(&rwlock);
            break;
            case 1:
            pthread_rwlock_wrlock(&rwlock);
            Insert(my_rank);
            printf("After insert  %d\n", my_rank);
            print_linked_list();
            pthread_rwlock_unlock(&rwlock);
            break;
            case 2:
            pthread_rwlock_wrlock(&rwlock);
            Delete(my_rank-1);
            printf("After delete  %d\n", my_rank-1);
            print_linked_list();
            pthread_rwlock_unlock(&rwlock);
            break;
        }
    }
	return NULL;
}


void print_linked_list() {
    struct list_node_s* temp = head;
   while(temp!=NULL) {
        printf("%d ", temp->data);
        temp=temp->next;
   }
   printf("\n");
}





int Member(int value) {
    struct list_node_s* curr_p = head;
    while(curr_p!=NULL && curr_p->data < value)
        curr_p = curr_p->next;
    if (curr_p == NULL || curr_p->data > value) {
        return 0;
    } else {
        return 1;
    }
}

int Delete(int value) {
    struct list_node_s* curr_p = head;
    struct list_node_s* pred_p = NULL;

    while(curr_p!=NULL && curr_p->data<value) {
        pred_p=curr_p;
        curr_p=curr_p->next;
    }
    if (curr_p!=NULL && curr_p->data==value) {
        if(pred_p==NULL) {
            head=curr_p->next;
            free(curr_p);
        } else {
            pred_p->next = curr_p->next;
            free(curr_p);
        }
        return 1;
    } else {
        return 0;
    }
}


int Insert(int value) {
    struct list_node_s* curr_p = head;
    struct list_node_s* pred_p = NULL;
    struct list_node_s* temp_p;

    while(curr_p!=NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }
    if(curr_p==NULL || curr_p->data>value) {
        temp_p = malloc(sizeof(struct list_node_s));
        temp_p->data = value;
        temp_p->next = curr_p;
        if(pred_p == NULL)
            head = temp_p;
        else
            pred_p->next = temp_p;
        return 1;
    } else {
        return 0;
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
   if (argc != 3) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);  
   if (thread_count <= 0 || thread_count > MAX_THREADS) Usage(argv[0]);
   n = strtoll(argv[2], NULL, 10);
   if (n <= 0) Usage(argv[0]);
}  /* Get_args */
