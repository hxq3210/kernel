#include <linux/module.h>
#include <linux/init.h>

#include "other.h"

static int cnt = 7788;
EXPORT_SYMBOL_GPL(cnt);

static int demo_init(void)
{
	mydebug(cnt);

	printk("hello, world.\n");

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
