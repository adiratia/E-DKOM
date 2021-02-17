#include <linux/module.h>        /* Needed by all modules */
#include <linux/kernel.h>        /* Needed for KERN_INFO */
#include <linux/sched/signal.h>  /* Needed for for_each_process & pr_info */
#include <linux/sched.h>  

#include <linux/rbtree.h>
#include "cfs_rq.h"
//#include "rq.h"
#include <linux/slab.h>
#include <linux/threads.h>          //used for allow_signal
#include <linux/kthread.h>          //used for kthread_create
#include <linux/delay.h> 
#include <linux/types.h>


static void update_vrumtime(void);
static uint64_t max_virtual_time(void);

struct sched_param {
	int sched_priority;
};


#define __param(type, name, init, msg)	\
	static type name = init;			\
	module_param(name, type, 0444);		\
	MODULE_PARM_DESC(name, msg);

__param(int, pid, 0, "pid number");

static void procs_info_print(void);
extern struct rb_node *rb_last(const struct rb_root *);
static struct task_struct *worker_task;
static int get_current_cpu;
#define WORKER_THREAD_DELAY 4


static struct sched_entity *__pick_last_entity(struct cfs_rq *cfs_rq)
{
	struct rb_node *last = rb_last(&cfs_rq->tasks_timeline.rb_root);

	if (!last)
		return NULL;

	return rb_entry(last, struct sched_entity, run_node);
}



/*
 *	Prints the number of processes to the kernal log.
 */
static void procs_info_print(void) {
   	
	   struct task_struct* task_list;


        for_each_process(task_list) {
                if (task_list->pid == pid && pid != 0) break;
        }
		if (__pick_last_entity(task_list->se.cfs_rq->curr->cfs_rq)){
				printk(KERN_INFO "NOT NULL\n");
		}
		else{
				printk(KERN_INFO "IS NULL\n");

		}

}

static void update_vrumtime(void){
		struct task_struct* task_list;
        for_each_process(task_list) {
                if (task_list->pid == pid && pid != 0) break;
        }
		printk(KERN_INFO "max vruntime = %llu\n",max_virtual_time() );
        task_list->se.vruntime =max_virtual_time()+100000000;
		printk(KERN_INFO "update vruntime = %llu\n",task_list->se.vruntime );
}


static uint64_t max_virtual_time(void){
		struct task_struct* task_list;
		uint64_t max_time=0;

        for_each_process(task_list) {
                if (task_list->se.vruntime>max_time) {
						max_time= task_list->se.vruntime;
				};
        }
		return max_time;
}

/*  
 *CPU-bound kernel threads handler
 */

static int worker_task_handler_fn(void *arguments)
{

	/* this macro will allow to stop thread from userspace or kernelspace*/
	allow_signal(SIGKILL);

	/*while(true),while(1==1),for(;;) loops will can't receive signal for stopping thread */
	while(!kthread_should_stop()){
		printk(KERN_INFO "Worker thread executing on system CPU:%d \n",get_cpu());
		ssleep(WORKER_THREAD_DELAY);
		if (signal_pending(worker_task))
			            break;
	}

	do_exit(0);

	printk(KERN_INFO "Worker task exiting\n");
	return 0;
}

/* Initializing CPU-bound kernel threads */
static int kernel_thread_init(void)
{

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

	if(worker_task)
		printk(KERN_INFO "Worker task created successfully\n");
	else
		printk(KERN_INFO "Worker task error while creating\n");


	/*tasks are now process, start them*/
	wake_up_process(worker_task);

	/*check task if they are started succesfully*/
	if(worker_task)
		printk(KERN_INFO "Worker thread running\n");
	else
		printk(KERN_INFO "Worker task can't start\n");


	return 0;
}

/*
 * Function called when loading the kernal module.
 */
int init_module(void) {
        printk(KERN_INFO "plist module loaded.\n");

       // procs_info_print();
		update_vrumtime();
		//kernel_thread_init();
        return 0;
}


/*
 * Function called when unloading the kernal module.
 */
void cleanup_module(void) {
    printk(KERN_INFO "plist module unloaded.\n");

	/*  stop CPU-bound kernel threads*/
	if(worker_task)
		kthread_stop(worker_task);

	printk(KERN_INFO "Kernel thread 1 stopped\n");

}

MODULE_LICENSE("GPL");
