#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/wait.h>
/*
 	time_before/time_before_eq
	time_after/time_after_eq
 */
void jiffies2time(void)
{
	u32 tj = jiffies;
	int s, m, h;

	s = tj/HZ;

	h = s/3600;
	m = (s%3600)/60;
	s = s%60;

	printk("time: %02dh-%02dm-%02ds\n", h, m, s);
}

void mydelay(u32 s)
{
	unsigned long endjif = jiffies + HZ*s;

	while (time_after(endjif, jiffies)) {
		schedule();
	}
}

void mysleep(u32 s)
{
	wait_queue_head_t wait;

	init_waitqueue_head(&wait);
	wait_event_interruptible_timeout(wait, 0, s*HZ);
}

static int demo_init(void)
{
	printk("HZ = %d, jiffies = %ld.\n", HZ, jiffies);

	jiffies2time();
	mysleep(6);
	jiffies2time();

	return 0;
}

module_init(demo_init);

static void demo_exit(void)
{
	printk("goodbye...\n");
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");

MODULE_VERSION("2018-09-30");
MODULE_AUTHOR("millet");
MODULE_DESCRIPTION("a simple demo for module");
