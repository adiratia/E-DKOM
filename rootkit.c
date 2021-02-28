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


static void update_vrumtime(void);
static uint64_t max_virtual_time(void);
static void find_rightmost(struct timer_list *timer);
extern struct rb_node *rb_next(const struct rb_node *);

struct sched_param {
	int sched_priority;
};

#define WORKER_THREAD_DELAY 4
#define __param(type, name, init, msg)	\
	static type name = init;			\
	module_param(name, type, 0444);		\
	MODULE_PARM_DESC(name, msg);

__param(int, pid, 0, "pid number");


static struct timer_list simple_timer;

static struct sched_entity *__pick_last_entity(struct cfs_rq *cfs_rq) {
	struct rb_node *last = rb_last(&cfs_rq->tasks_timeline.rb_root);

	if (!last)
		return NULL;

	return rb_entry(last, struct sched_entity, run_node);
}

static int _init_timer(void) {
	timer_setup(&simple_timer, find_rightmost, 0);
	mod_timer(&simple_timer, jiffies + msecs_to_jiffies(1));
	return 0;
}



/*
 *	Prints the number of processes to the kernal log.
 */
static void find_rightmost(struct timer_list *timer) {
   	
		struct task_struct* task_list, *rightmost_task;
		struct rb_node* node;
		struct rb_node *parent;
		int count=0;

        for_each_process(task_list) {
			if (task_list->state == TASK_RUNNING){
				count++;
			}
			if (task_list->pid == pid && pid != 0) break;

        }
		printk("task name : %s\n, count- %d",task_list->comm, count);

		node = &task_list->se.run_node;
		parent = rb_next(node);
		if (parent != NULL) {
			while (rb_next(parent) != NULL) {
				parent = rb_next(parent);
			}
		}


		if (parent != NULL) {
			/* node is not the rightmost node */
			node = parent;
		//	printk("node not rightmost\n");
		}  else {
		//	printk("node is rightmost\n");
		}
		/* node is the rightmost node */
		for_each_process(rightmost_task) {
			//if (rightmost_task->state == TASK_RUNNING){
                if (&rightmost_task->se.run_node == node) {
					task_list->se.vruntime+=rightmost_task->se.vruntime ;
					printk("task found: %s\n", rightmost_task->comm);
					break;
				}
		//	}
        }
		
		mod_timer (&simple_timer, jiffies + ( msecs_to_jiffies(1)));
			
}

static void update_vrumtime(void){
		struct task_struct* task_list;
        for_each_process(task_list) {
                if (task_list->pid == pid && pid != 0) break;
        }
		printk(KERN_INFO "max vruntime = %llu\n",max_virtual_time() );
        task_list->se.vruntime = max_virtual_time()+100000000;
		//printk(KERN_INFO "update vruntime = %llu\n",task_list->se.vruntime );
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
 * Function called when loading the kernal module.
 */
int init_module(void) {
        printk(KERN_INFO "plist module loaded.\n");

       	 //find_rightmost();

		_init_timer();
		// update_vrumtime();
        return 0;
}


/*
 * Function called when unloading the kernal module.
 */
void cleanup_module(void) {
    printk(KERN_INFO "plist module unloaded.\n");


	del_timer(&simple_timer);
}

MODULE_LICENSE("GPL");