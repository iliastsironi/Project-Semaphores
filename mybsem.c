#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <errno.h>
#include <fcntl.h>
#include "mybsem.h"

bsem_t *mysem_create(bsem_t *s, int value){
	int check;
	
	if (value != 0 && value != 1){
		printf("\nSemaphore is binary, give as value 0 or 1\n");
		return(NULL);
	}
	//Create a semaphore
	s = (bsem_t *)malloc(sizeof(bsem_t));
	s->semid = semget(IPC_PRIVATE,1,S_IRWXU);
	if (s->semid < 0){
		printf("\n(Create:)Something went wrong at semget, errno: %d\n", errno);
		return(NULL);
	}
	//Inititalize semaphore with value 1
	check = semctl(s->semid,0,SETVAL,value);
	if (check < 0){
		printf("\n(Create:)Something went wrong at semctl, errno: %d\n", errno);
		return(NULL);
	}
	return(s);
}

int mysem_up(bsem_t *s){
	struct sembuf op;
	unsigned short int semval, /*semncnt,*/ semzcnt;
	
	//If semval = 1 then error
	semval = semctl(s->semid, 0, GETVAL);
	if (semval){
		printf("\nSemaphore with address %p has already the value 1\n", s);
		return(-2);
	}
	//If there are blocked threads, wake up someone
	semzcnt = semctl(s->semid,0,GETZCNT);
	if (semzcnt){
		op.sem_num = 0;
		op.sem_op = 0;
		op.sem_flg = 0;
		if (semop(s->semid,&op,1)){         //Is semval >=|sem_op| (0 >= |0|) so continue 
			printf("\n(Up:)Something went wrong at semop, errno: %d\n", errno);
			return(-1);
		}  
		return(0);
	}
	//If there are no blocked threads increase semval
	op.sem_num = 0;
	op.sem_op = 1;
	op.sem_flg = 0;
	if (semop(s->semid,&op,1)){
		printf("\n(Up:)Something went wrong at semop, errno: %d\n", errno);
		return(-1);
	}
	return(1);
}

int mysem_down(bsem_t *s){
	struct sembuf op;
	unsigned short int semval;
	
	//If semval = 0 then blocks
	semval = semctl(s->semid, 0, GETVAL);
	if (!semval){
		op.sem_num = 0;
		op.sem_op = -1;
		op.sem_flg = 0;
		if (semop(s->semid,&op,1)){         //Is semval < |sem_op| (0 < |-1|) so sleep until: semval >= semop 
			printf("\n(Up:)Something went wrong at semop, errno: %d\n", errno);
			return(-1);
		}
	}
	//If semval = 1 make it 0
	semval = semctl(s->semid, 0, GETVAL); 
	if (semval){
		op.sem_num = 0;
		op.sem_op = -1;
		op.sem_flg = 0;
		if (semop(s->semid,&op,1)){
			printf("\n(Down:)Something went wrong at semop, errno: %d\n", errno);
			return(-1);
		}
		return(1);
	}
	return(0); 
}

int mysem_destroy(bsem_t *s){
	int check;
	
	check = semctl(s->semid,0,IPC_RMID);
	if (check < 0){
		printf("\n(Destroy:)Something went wrong at destroying sem %d, errno: %d\n", s->semid, errno);
		return(-1);
	}
	free(s);
	return(1);
}

