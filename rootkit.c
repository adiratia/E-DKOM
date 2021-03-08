#include <linux/module.h>        /* Needed by all modules */
#include <linux/kernel.h>        /* Needed for KERN_INFO */
#include <linux/sched/signal.h>  /* Needed for for_each_process & pr_info */
#include <linux/sched.h>  
#include <linux/rbtree.h>
#include "cfs_rq.h"
#include <linux/threads.h>          //used for allow_signal
#include <linux/kthread.h>          //used for kthread_create
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/proc_fs.h>
#include <linux/pid.h>

#include <linux/pid_namespace.h>

typedef u64 uint64_t;

static struct task_struct* max_virtual_time(void);
static void find_rightmost(void);
extern struct rb_node *rb_next(const struct rb_node *);
extern struct rb_node *rb_last(const struct rb_root *);
enum hrtimer_restart timer_callback( struct hrtimer *timer_for_restart );

#define __param(type, name, init, msg)	\
	static type name = init;			\
	module_param(name, type, 0444);		\
	MODULE_PARM_DESC(name, msg);

__param(int, pid, 0, "pid number");


static int get_current_cpu;
unsigned long timer_interval_ns = 100;
static struct hrtimer hr_timer;
struct pid *pid_struct;




enum hrtimer_restart timer_callback( struct hrtimer *timer_for_restart )
{
  	ktime_t currtime , interval;
  	currtime  = ktime_get();
  	interval = ktime_set(0,timer_interval_ns); 
  	hrtimer_forward(timer_for_restart, currtime , interval);
	find_rightmost();
	return HRTIMER_RESTART;
}


/*
 *	Prints the number of processes to the kernal log.
 */
static void find_rightmost(void) {
   	
		struct task_struct* task_list, *rightmost_task, *max;
		struct rb_node* node;
		struct rb_node *rightmost=NULL;

		//Find task by pid
		pid_struct = find_get_pid(pid);
		task_list = pid_task(pid_struct,PIDTYPE_PID);

		//Find rightmost task
		//rightmost= rb_last(&task_list->se.cfs_rq->tasks_timeline.rb_root);
		

		max = max_virtual_time();
		task_list->se.vruntime = max->se.vruntime+1000000;
		
		/* node is the rightmost node */
		/*if(rightmost!=NULL){
			for_each_process(rightmost_task) {
					if ( &rightmost_task->se.run_node == rightmost) {
						printk("rightmost task: : %s ; rightmost vruntime: %llu\n", rightmost_task->comm, rightmost_task->se.vruntime);
						break;

					}
			}
		}*/

			
}

static struct task_struct* max_virtual_time(void) {
		struct task_struct* task_list, *max;
		uint64_t max_time = 0;

        for_each_process(task_list) {
                if (task_list->se.vruntime > max_time && task_list->state == TASK_RUNNING) {
						max_time = task_list->se.vruntime;
						max = task_list;
				};
        }
		return max;
}


/*
 * Function called when loading the kernal module.
 */
int init_module(void) {
        printk(KERN_INFO "plist module loaded.\n");
		    ktime_t ktime;
		ktime = ktime_set( 0, timer_interval_ns );
		hrtimer_init( &hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL );
		hr_timer.function = &timer_callback;
		hrtimer_start( &hr_timer, ktime, HRTIMER_MODE_REL );

		
        return 0;
}


/*
 * Function called when unloading the kernal module.
 */
void cleanup_module(void) {
		printk(KERN_INFO "plist module unloaded.\n");
		struct task_struct *task;
		int ret;
		ret = hrtimer_cancel( &hr_timer );
		if (ret) printk("The timer was still in use...\n");
		printk("HR Timer module uninstalling\n");
				
		//Find task by pid
		pid_struct = find_get_pid(pid);
		task = pid_task(pid_struct,PIDTYPE_PID);
		task->se.vruntime = task->se.cfs_rq->min_vruntime;
	}


MODULE_LICENSE("GPL");
