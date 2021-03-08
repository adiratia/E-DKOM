#include <linux/module.h>        /* Needed by all modules */
#include <linux/kernel.h>        /* Needed for KERN_INFO */
#include <linux/sched/signal.h>  /* Needed for for_each_process & pr_info */
#include <linux/sched.h>  
#include "cfs_rq.h"
//#include "rq.h"
#include <linux/threads.h>          //used for allow_signal
#include <linux/kthread.h>          //used for kthread_create
#include <linux/delay.h> 

#define __param(type, name, init, msg)	\
	static type name = init;			\
	module_param(name, type, 0444);		\
	MODULE_PARM_DESC(name, msg);

__param(int, pid, 0, "pid number");

struct sched_param {
	int sched_priority;
};

#define WORKER_THREAD_DELAY 2

static struct task_struct *worker_task,*worker_task2 ;
static int get_current_cpu;




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
	



	if(worker_task)
		printk(KERN_INFO "Worker tasks created successfully\n");
	else
		printk(KERN_INFO "Worker tasks error while creating\n");


	/*tasks are now process, start them*/
	wake_up_process(worker_task);
	wake_up_process(worker_task2);






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

	}


	printk(KERN_INFO "Kernel threads stopped\n");
}

MODULE_LICENSE("GPL");