#include <linux/module.h>
#include <linux/init.h>

void mydebug(int n)
{
	printk("---------- %d -----------\n", n);
}
EXPORT_SYMBOL_GPL(mydebug);
