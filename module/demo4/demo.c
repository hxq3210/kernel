#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

static int lednum = 4;
module_param(lednum, int, 0644);

static char *str = "how sleep you are!";
module_param(str, charp, 0644);

static int demo_init(void)
{
	printk("total led : %d , %s\n", lednum, str);

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
