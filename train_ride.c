#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "mybsem.h"

//Global variables
int enter = 0, leave = 0, tickets = 0, turn_off_train = 0;
int NumOfPassengers, capacity;
bsem_t *wait, *board, *train_start, *finish;

//Functions
void *train_ride(void *info){

	while (tickets <= NumOfPassengers){
		//If train not full, then block
// 		printf("\ntrain blocked, tickets %d\n", tickets);
		mysem_down(train_start);
		if (turn_off_train)
			break;
		//Make the ride
		printf("\ntrain starting, have fun\n");
		sleep(3);
		printf("\nRide finished\n");
		//Tell one passenger to leave the train
		mysem_up(wait);
	}
	printf("\n$$$$$$$$$ Have a good day $$$$$$$$\n");
	mysem_up(finish);
	return(NULL);
}

void *wanna_ride(void *passenger){
	int me = rand()/10000;
	
	//passengers wait to get onboard
	mysem_down(board);
	
	//Board the train
	++enter;
	printf("\n%d entering %d\n", me, enter);
	if (enter < capacity){
		mysem_up(board);
	}
	++tickets;
	if (enter == capacity){
 	//	printf("\nI %d start the train\n", me);
		mysem_up(train_start);
	}
	mysem_down(wait);
	
	//Leave the train
	++leave;
	printf("\n%d leaving %d\n", me, leave);
	if (leave < capacity)
		mysem_up(wait);
	else if (leave == capacity && tickets != NumOfPassengers){
		leave = 0;
		enter = 0;
// 		printf("\n%d Okey it free get inside, enter %d and leave %d\n", me, enter , leave);
		mysem_up(board);
	}
	else {
		turn_off_train = 1;
		mysem_up(train_start);
	}
	free(passenger);
	return(NULL);
}

int main(int argc, char* argv[]){
	pthread_t *train, *passenger;
	int i/*,valid_num*/;
	
	//Read everything needs for the ride
	printf("\nPlease give number of passengers you want to do the ride\n");
	scanf("%d", &NumOfPassengers);
	printf("\nPlease tell me how many passengers can carry the train for one ride\n");
	scanf("%d", &capacity);
// 	valid_num = NumOfPassengers % capacity;
// 	printf("\ncapacity = %d, NumOfPassengers = %d, valid_num = %d\n",capacity,NumOfPassengers, valid_num);
	
	//Create Semaphores
	finish = mysem_create(finish,0);
	board = mysem_create(board,0);
	wait = mysem_create(wait,0);
	train_start = mysem_create(train_start,0);
	
	//create train
	train = (pthread_t *)malloc(sizeof(pthread_t));
	pthread_create(train,NULL,train_ride,NULL);
	
	//create passengers
	for (i=0; i < NumOfPassengers; i++){
		passenger = (pthread_t *)malloc(sizeof(pthread_t));
		pthread_create(passenger,NULL,wanna_ride,NULL);
	}
	mysem_up(board);
	
	//wait train to finish all the rides
	mysem_down(finish);
	//destroy semaphores and free space
	mysem_destroy(finish);
	mysem_destroy(board);
	mysem_destroy(wait);
	mysem_destroy(train_start);
	free(train);
	return(0);
} 
