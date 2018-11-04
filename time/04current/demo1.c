#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

#define USECPERSEC 1000000U

//mdelay, udelay, ndelay
//msleep, msleep_interruptible, ssleep

static int demo_init(void)
{
	struct timespec t0, t1;
	u32 us;

	t0 = current_kernel_time();
	mdelay(8);	
	t1 = current_kernel_time();

	us = (t1.tv_sec - t0.tv_sec)*USECPERSEC + t1.tv_nsec/1000
					- t0.tv_nsec/1000;

	printk("us = %u\n", us);

	return 0;
}

module_init(demo_init);

static void demo_exit(void)
{
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");

MODULE_VERSION("2018-09-30");
MODULE_AUTHOR("millet");
MODULE_DESCRIPTION("a simple demo for module");
