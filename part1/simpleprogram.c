#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int SharedVariable = 0;

#ifdef PTHREAD_SYNC
pthread_mutex_t lock_x; 
pthread_barrier_t barr;
#endif

void *SimpleThread(void *which){
	long tid;
	tid = (long) which; //converts the thread being used to a long int
	int num, val;
	for(num = 0; num < 20; num++){
		if(random() > RAND_MAX/2)
			usleep(10);
#ifdef PTHREAD_SYNC		
		pthread_mutex_lock(&lock_x); //blocks other threads from acquring this specific val
#endif
		val = SharedVariable;
		printf("*** thread %ld sees value %d\n", tid, val);
		SharedVariable = val + 1;
#ifdef PTHREAD_SYNC
		pthread_mutex_unlock(&lock_x); //unlocks the block placed on other threads
#endif
	}
#ifdef PTHREAD_SYNC
	pthread_barrier_wait(&barr); //makes all threads wait until the last one is finished
#endif 
	val = SharedVariable;
	printf("Thread %ld sees final value %d\n", tid, val);
	pthread_exit(NULL); //terminates the thread being called
}
int main(int argc, char *argv[]){
	int a, rc;
	scanf("%d", &a); //Takes the parameter from the cmd line
	pthread_t thr[a]; //Creates the number of threads specfied
	long t;

#ifdef PTHREAD_SYNC
	pthread_mutex_init(&lock_x, NULL); /*initalize mutex lock*/
	pthread_barrier_init(&barr, NULL, a); /*initalize barrier*/
#endif

	for(t=0; t < a; t++){
		printf("Creating thread %ld\n", t);		
		rc = pthread_create(&thr[t], NULL, SimpleThread, (void *)t); //starts a new thread
		if(rc){
			printf("Error");
			exit(-1);
		}		
	}

#ifdef PTHREAD_SYNC
	for(t=0;t<a;t++){
		pthread_join(thr[t], NULL); //blocks until all threads complete
	}
#endif

	pthread_exit(NULL);
}
