#include <linux/module.h>        /* Needed by all modules */
#include <linux/kernel.h>        /* Needed for KERN_INFO */
#include <linux/sched/signal.h>  /* Needed for for_each_process & pr_info */
#include <linux/sched.h>  
//#include <asm/current.h>
#include <linux/rbtree.h>
#include "cfs_rq.h"
//#include "rq.h"
#include <linux/slab.h>
#include <linux/threads.h>          //used for allow_signal
#include <linux/kthread.h>          //used for kthread_create
#include <linux/delay.h> 
#include <linux/types.h>

#include <linux/timer.h>
#include <linux/jiffies.h>

#define __param(type, name, init, msg)	\
	static type name = init;			\
	module_param(name, type, 0444);		\
	MODULE_PARM_DESC(name, msg);

__param(int, pid, 0, "pid number");

struct sched_param {
	int sched_priority;
};

#define WORKER_THREAD_DELAY 0.1

static struct task_struct *worker_task,*worker_task2, *worker_task3,*worker_task4,*worker_task5,*worker_task6;
static int get_current_cpu;
static int count=0;




/*  
 *CPU-bound kernel threads handler
 */

static int worker_task_handler_fn(void *arguments)
{

	/* this macro will allow to stop thread from userspace or kernelspace*/
	allow_signal(SIGKILL);

	/*while(true),while(1==1),for(;;) loops will can't receive signal for stopping thread */
	while(!kthread_should_stop()){

		msleep(WORKER_THREAD_DELAY);
		//printk(KERN_INFO "%s executing on system CPU:%d \n",current->comm,get_cpu());

	}


	do_exit(0);

	printk(KERN_INFO "Worker task exiting\n");
	return 0;
}


/* Initializing CPU-bound kernel threads */
static int kernel_thread_init(void)
{

	//sem_init(&mutex, 0, 1); 

	/*scheduler priority structs to set task priority*/
	struct sched_param task_sched_params =
	{
			.sched_priority = MAX_RT_PRIO
	};

	task_sched_params.sched_priority = 90;

	printk(KERN_INFO "Initializing kernel mode thread example module\n");
	printk(KERN_INFO "Creating Threads\n");

	/*get current cpu to bind over task
	 */
	get_current_cpu = get_cpu();
	printk(KERN_DEBUG "Getting current CPU %d to binding worker thread\n",get_current_cpu);

	/*initialize worker task with arguments, thread_name and cpu*/
	worker_task = kthread_create(worker_task_handler_fn,(void*)"arguments as char pointer","Kernel Thread 1");
	kthread_bind(worker_task,get_current_cpu);

	worker_task2 = kthread_create(worker_task_handler_fn,(void*)"arguments as char pointer","Kernel Thread 2");
	kthread_bind(worker_task2,get_current_cpu);

	worker_task3 = kthread_create(worker_task_handler_fn,(void*)"arguments as char pointer","Kernel Thread 3");
	kthread_bind(worker_task3,get_current_cpu);

	worker_task4 = kthread_create(worker_task_handler_fn,(void*)"arguments as char pointer","Kernel Thread 4");
	kthread_bind(worker_task4,get_current_cpu);


	worker_task5 = kthread_create(worker_task_handler_fn,(void*)"arguments as char pointer","Kernel Thread 5");
	kthread_bind(worker_task5,get_current_cpu);


	worker_task6 = kthread_create(worker_task_handler_fn,(void*)"arguments as char pointer","Kernel Thread 6");
	kthread_bind(worker_task6,get_current_cpu);




	if(worker_task)
		printk(KERN_INFO "Worker tasks created successfully\n");
	else
		printk(KERN_INFO "Worker tasks error while creating\n");


	/*tasks are now process, start them*/
	wake_up_process(worker_task);
	ssleep(2); 
	wake_up_process(worker_task2);
	ssleep(2); 
	wake_up_process(worker_task3);
	ssleep(2); 
	wake_up_process(worker_task4);
	ssleep(2); 
	wake_up_process(worker_task5);
	ssleep(2); 
	wake_up_process(worker_task6);





	/*check task if they are started succesfully*/
	if(worker_task)
		printk(KERN_INFO "Worker threads running\n");
	else
		printk(KERN_INFO "Worker tasks can't start\n");



	return 0;
}

/*
 * Function called when loading the kernal module.
 */
int init_module(void) {
        printk(KERN_INFO "init_kthread module loaded.\n");

		kernel_thread_init();

        return 0;
}


/*
 * Function called when unloading the kernal module.
 */
void cleanup_module(void) {
    printk(KERN_INFO "init_kthread module unloaded.\n");

	/*  stop CPU-bound kernel threads*/
	if(worker_task){
		kthread_stop(worker_task);
		kthread_stop(worker_task2);
		kthread_stop(worker_task3);
		kthread_stop(worker_task4);
		kthread_stop(worker_task5);
		kthread_stop(worker_task6);
	}


	printk(KERN_INFO "Kernel threads stopped\n");

//	sem_destroy(&mutex); 



}

MODULE_LICENSE("GPL");