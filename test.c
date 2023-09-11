#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include "mybsem.h"

typedef struct ThreadData{
	pthread_t *id;
	int state;
	bsem_t *sem;
}thread_t;

thread_t *array;

void *routine(void *buffer){
	int i, me = rand();
	thread_t *info;
	info = (thread_t *)buffer;
	while (1){
		mysem_down(info->sem);
		printf("Hello there %d\n", me);
		i=100;
		while(i > 0){
			i--;
		}
		printf("Bye there %d\n", me);
		if (mysem_up(info->sem))
			break;
		
// 		printf("\nI Waitting :-)\n");
	}
	/*mysem_down(info->sem);
	printf("Hello there %d\n", me);
	i=100;
	while(i > 0){
		i--;
	}
	printf("Bye there %d\n", me);
	mysem_up(info->sem);*/
	info->state = 1;
	return(NULL);
}


int main(int argc, char *argv[]){
	int i, num, check=1;
	bsem_t *s=NULL;
	
	//Read Number of threads
	printf("\nPlease give the number of threads you want: ");
	scanf("%d", &num);
	if (num <=0){
		return(0);
	}
	
	
	//Initializing Semaphores
	s = mysem_create(s,1);
	
	
	//Main body
	array = (thread_t *)malloc(num * sizeof(thread_t));
	for (i =0; i < num; i++){
		array[i].id = (pthread_t *)malloc(sizeof(pthread_t));
		array[i].state = 0;
		array[i].sem = s;
		if (pthread_create(array[i].id,NULL,routine,&array[i])){
			printf("\nSomething went wrong at pthread_create\n");
		}
	}
	
	//Wait all the threads to finish
	while (check){
// 		printf("\nWaitting :-)\n");
		check = 0;
		for (i=0; i < num;i++){
			if(array[i].state == 0){
				check++;
			}
		}
	}
	
	
	
	//Destroy and Free Everything
	mysem_destroy(s);
	for (i=0; i < num; i++){
		free(array[i].id);
	}
	free(array);
	return(0);
} 
