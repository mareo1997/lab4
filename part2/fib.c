#include <pthread.h>
#include <stdio.h>

int sum; /*this data is shared by the threads*/
void *runner(void *param); /*threads call this function*/

int main(int argc, char *argv[]){
	pthread_t tid; /*thread identifier*/
	pthread_attr_t attr; /*set of thread attributes*/
	
	if(argc !=2){
		fprintf(stderr, "error 1\n");
		return -1;
	}
	if(atoi(argv[1]) < 0){
		fprintf(stderr, "error 2 %d < 0\n", atoi(argv[1]));
		return -1;
	}

	pthread_attr_init(&attr); /*get default attributes*/
	pthread_create(&tid, &attr, runner, argv[1]); /*create the thread*/
	pthread_join(tid, NULL); /*wait for thread to exit*/
	pthread_exit(NULL);
}

void *runner(void *param){
	int i, upper = atoi(param);
	int a = 0;
	int b = 1;
	sum = a + b;
	if(upper > 0){ /*print 0 if the cmd line parameter is 1*/
		printf("0 ");
	}
	if(upper > 1){ /*print 0 1 if cmd line parameter is 2*/
		printf("1 ");
	}
	for(i = 3; i <= upper; i++){ 
		a = b;		
		b = sum;
		printf("%d ", sum);
		sum = a + b;
	}
	printf("\n");
	pthread_exit(0); /*terminates the thread once finished*/
}
