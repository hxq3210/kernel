#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
/*
 	time_before/time_before_eq
	time_after/time_after_eq
 */

#define SEC 5

static struct timer_list timer;

static void do_timer(unsigned long data)
{
	char *s = (void *)data;

	printk("In %s: %s\n", __func__, s);
	if (in_interrupt()) {
		printk("interrupt ...\n");
	}

	mod_timer(&timer, jiffies+SEC*HZ);
}

static int demo_init(void)
{
	char *str = "lol...";
	//init_timer(&timer);
	//timer.function = do_timer;
	//timer.expires  = jiffies + 2*HZ;
	//timer.data     = &xx;

	setup_timer(&timer, do_timer, (unsigned long)str);
	timer.expires = jiffies + SEC*HZ;

	add_timer(&timer);

	return 0;
}

module_init(demo_init);

static void demo_exit(void)
{
	//del_timer(&timer);
	del_timer_sync(&timer);
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");

MODULE_VERSION("2018-09-30");
MODULE_AUTHOR("millet");
MODULE_DESCRIPTION("a simple demo for module");
