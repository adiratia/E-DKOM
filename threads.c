#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>




#define WORKER_THREAD_DELAY 1

int main(void){
	fork(); 
    fork(); 

  while(1){
    int msec = 0, trigger = 100; /* 10ms */
		clock_t before = clock();
		do {
		/*
		* Do something to busy the CPU just here while you drink a coffee
		* Be sure this code will not take more than `trigger` ms
		*/

		clock_t difference = clock() - before;
		msec = difference * 1000 / CLOCKS_PER_SEC;
		} while ( msec < trigger );
		//usleep(WORKER_THREAD_DELAY);

    printf("current pid =  %ld\n", (long)getpid());
  }
}