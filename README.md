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

#
