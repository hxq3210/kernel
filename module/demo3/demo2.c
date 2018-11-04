#include <linux/module.h>
#include <linux/init.h>

#include "other.h"

extern int cnt;

static int demo_init2(void)
{
	cnt = 123456;

	mydebug(cnt);

	printk("hello, world.\n");

	return 0;
}

module_init(demo_init2);

static void demo_exit2(void)
{
	printk("goodbye...\n");
}

module_exit(demo_exit2);

MODULE_LICENSE("GPL");

MODULE_VERSION("2018-09-30");
MODULE_AUTHOR("millet");
MODULE_DESCRIPTION("a simple demo for module");
