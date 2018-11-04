#include <linux/module.h>
#include <linux/init.h>

#include "mydevice.h"

extern struct bus_type platform_uplook;

static void	mydev_release (struct device *dev)
{
	struct mydevice *pdev = container_of(dev, struct mydevice, dev);

	printk("%s device is release...\n",
			pdev->name);
}

static struct mydevice mydev2 = {
	.dev = {
		.init_name 	= "mydev2",
		.bus		= &platform_uplook,
		.release	= mydev_release, 
	},	
	.name	=	"shy_bee",
};

static int demo_init(void)
{
	return device_register(&mydev2.dev);
}

module_init(demo_init);

static void demo_exit(void)
{
	device_unregister(&mydev2.dev);
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");

MODULE_VERSION("2018-09-30");
MODULE_AUTHOR("millet");
MODULE_DESCRIPTION("a simple demo for module");
