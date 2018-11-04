#include <linux/init.h>
#include <linux/module.h>

static int output_init(void)
{
	int i;

	for (i = 0; i < 200; i++) {
		printk("!~.~! no pain, no gain...\n");
	}

	return 0;
}

module_init(output_init);

MODULE_LICENSE("GPL");
