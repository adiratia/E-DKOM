#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>





int main(void){


  while(1){
    int msec = 0, trigger = 1000; /* 10ms */
		clock_t before = clock();
		do {
		/*
		* Do something to busy the CPU just here while you drink a coffee
		* Be sure this code will not take more than `trigger` ms
		*/

		clock_t difference = clock() - before;
		msec = difference * 1000 / CLOCKS_PER_SEC;
		} while ( msec < trigger );
    printf("current pid =  %ld\n", (long)getpid());
  }
}