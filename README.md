# Project-Semaphores
Project uses our semaphores for simulation of a bridge.

# bridge
Program bridge.c takes as argument the numbers of cars that bridge can carry (bridge_size)
	Also takes with redirect as input one file.
	Program read the file and generate cars (threads). Then sychronize them in order to cross the bridge with the following laws:
		1. At most "bridge_size" cars on the bridge
		2. Next car start only when bridge is empty
	Program must end when reach the end of the file. We couldn't make it work so we set as termination case the -1
	Program use bsem.h in order to synchronize threads 

Semaphores
  Program uses 3 binary semaphores:
    terminate_sem  : Initialize to 0 and cotrols the waitting of main 
		R_light        : Initialize to 0 and controls the blue cars 
		L_light        : Initialize to 0 and controls the red cars

# train_ride
Program train_ride read two numbers: 
		the number of passengers you want to ride the train (NumOfPassengers)
		the number of passengers that can carry the train in every ride (capacity)
	
	Train starts only when "capacity" number of passengers board the train
	Next group of passengers board only when all the previous unboard the train
	If the "NumOfPassengers" % "capacity" isn't zero then the program waitting (so be carefull)
	
	Program use bsem.h in order to synchronize threads 

#Semaphores
Program uses 4 binary semaphores:
	wait: I initialize it to 0 and we use it when a thread has board the train, end wait to make the ride
	board: I initialize it to 0 and controls the waitting for all the threads-passengers(before board the train)
	train_start: I initialize it to 0 and controls the train ride. If train will wait or start
	finish: I initialize it to 0 and controls the termination of main. Main waits all the passengers to finish their rides
