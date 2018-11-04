#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

#define USECPERSEC 1000000U

static int demo_init(void)
{
	struct timeval t0, t1;
	u32 us;

	do_gettimeofday(&t0);	
	mdelay(8);	
	do_gettimeofday(&t1);	

	if (t1.tv_usec < t0.tv_usec) {
		us = (t1.tv_sec-1-t0.tv_sec)*USECPERSEC + (t1.tv_usec+1000000 - t0.tv_usec);
	} else {
		us = (t1.tv_sec-t0.tv_sec)*USECPERSEC + (t1.tv_usec - t0.tv_usec);
	}

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
