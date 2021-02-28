#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>


// Global variable:
int i = 2;

void* foo(void* p){
  // Print value received as argument:

  while(1){
    int msec = 0, trigger = 4000; /* 10ms */
		clock_t before = clock();
		do {
		/*
		* Do something to busy the CPU just here while you drink a coffee
		* Be sure this code will not take more than `trigger` ms
		*/


    printf("Thread \n");
		clock_t difference = clock() - before;
		msec = difference * 1000 / CLOCKS_PER_SEC;
		} while ( msec < trigger );
    usleep(0.1);
  }
  // Return reference to global variable:
  pthread_exit(&i);
}

int main(void){
  // Declare variable for thread's ID:
  pthread_t t1,t2;


  int j = 1, i=2;
  pthread_create(&t1, NULL, foo, &j);
    pthread_create(&t2, NULL, foo, &i);

    
  int* ptr1,*ptr2;

  // Wait for foo() and retrieve value in ptr;
  pthread_join(t1, (void**)&ptr1);
    pthread_join(t2, (void**)&ptr2);
}