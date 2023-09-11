#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "mybsem.h"

//STRUCTS AND GLOBALS
typedef struct CarData{
	pthread_t *id;
	int direction;     //0 for right and 1 for left
	int cross_duration;
}car_t;

bsem_t *terminate_sem, *R_light, *L_light;
int enter_count = 0, exit_count = 0, NumOfCars = 0, RightCarsBlocked = 0, LeftCarsBlocked = 0;
int bridge_size, light_direction;

//FUNCTIONS
void bridge_enter(car_t *car){
	while (1){
		//Cars block
		if (car->direction) {
			LeftCarsBlocked++;
			mysem_down(L_light);
		}
		else {
			RightCarsBlocked++;
			mysem_down(R_light);
		}
		//If has priority let him pass
		if (light_direction == car->direction){
			//If bridge isn't full continue
			enter_count++;
			if (enter_count < bridge_size){
				//If you are not the last one, wake up the next one
				if (car->direction) {
					if (--LeftCarsBlocked)
						mysem_up(L_light);
				}
				else { 
					if (--RightCarsBlocked)
						mysem_up(R_light);
				}
			}
			else {
				if (car->direction) 
					--LeftCarsBlocked;
				else 
					--RightCarsBlocked;
			}
			return;
		}
	}
}

void bridge_leave(car_t *car){
	exit_count++;
	//Exit process
	if (exit_count == enter_count){
		enter_count = 0;
		exit_count = 0;
		//The last one changes light direction
		if (light_direction && RightCarsBlocked > 0) {	//An to fanari oi Left & iparxoun Right => dose stous Right to fanari
			light_direction = 0;
			mysem_up(R_light);
		}
		else if (!light_direction && LeftCarsBlocked > 0){	//An to fanari oi Right & iparxoun Left => dose stous Left to fanari
			light_direction =1;
			mysem_up(L_light);
		}
		else if (light_direction && LeftCarsBlocked > 0)	//An to fanari oi Left (den iparxoun Right) & iparxoun left => krata to fanari
			mysem_up(L_light);
		else if (!light_direction && RightCarsBlocked > 0)	//An to fanari oi Right(den iparxoun Left) & iparxoun right => krata to fanari
			mysem_up(R_light);
	}
	return;
}

void *routine(void *info){
	car_t *car;
	car = (car_t*)info;
	int me= rand()/100000;
	
	//Enter bridge
	bridge_enter(car);
	//Crossoing Process
	if (car->direction) 
		printf("\nred car %d crossing\n",me);
	else 
		printf("\nblue car %d crossing\n",me);
	sleep(car->cross_duration);
	//leaving bridge
	if (car->direction)
		printf("\nred car %d left the bridge\n", me);
	else  
		printf("\nblue car %d left the bridge\n", me);
	bridge_leave(car);
	//Thread terminating
	free(car->id);
	car->id = NULL;
	free(car);
	NumOfCars -= 1;
	if (NumOfCars == 0){
		mysem_up(terminate_sem);
	}
	return(NULL);
}

int main(int argc, char* argv[]){
	car_t *car;
	int i=0;
	
	//Read bridge size
	if (argc < 2) {
		printf("\nYou forgot to give the size of bridge you want (how many cars can cross the bridge any time)\n");
		return(0);
	}
	bridge_size = atoi(argv[1]);
	if (bridge_size <= 0){
		printf("\nInvalid bridge size\n");
		return(0);
	}
	
	//Initialize Semaphores 
	R_light = mysem_create(R_light, 0);
	L_light = mysem_create(L_light,0);
	terminate_sem = mysem_create(terminate_sem,0);
	
	//Car Generator
	while (1){
		if (feof(stdin))
			break;
		//Initialize a car and read car details
		car = (car_t *)malloc(sizeof(car_t));
		car->id = (pthread_t *)malloc(sizeof(pthread_t));
		scanf("%d", &(car->direction));
		if (car->direction == -1){
			free(car->id);
			car->id = NULL;
			free(car);
			break;
		}
		scanf("%d", &(car->cross_duration));
		NumOfCars++;
		pthread_create(car->id,NULL,routine,car);
		//First car gives the light priority
		if (!i){
			if (car->direction)
				light_direction = 1;
			else 
				light_direction = 0;
			i++;
		}
	}
	//Wake up the car with priority
	if (light_direction)
		mysem_up(L_light);
	else 
		mysem_up(R_light);
	//Wait cars to finish the crossing
	mysem_down(terminate_sem);
	//Destroy semaphores
	mysem_destroy(R_light);
	mysem_destroy(L_light);
	mysem_destroy(terminate_sem);
	return(0);
}
