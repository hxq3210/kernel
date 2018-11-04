#include <linux/module.h>
#include <linux/init.h>

#include "mydevice.h"

extern struct bus_type platform_uplook;

/*当总线匹配设备和驱动对象成功时，此函数由内核自动调用并拿到了驱动
  对象所匹配成功的设备对象*/
static int mydrv_probe (struct device *dev)
{
	struct mydevice *pdev = container_of(dev, struct mydevice, dev);	

	printk("%s driver deal with %s device...\n", 
			dev->driver->name, pdev->name);

	return 0;
}

static int mydrv_remove (struct device *dev)
{
	struct mydevice *pdev = container_of(dev, struct mydevice, dev);	

	printk("%s driver remove %s device...\n", 
			dev->driver->name, pdev->name);

	return 0;

}

static struct device_driver mydrv2 = {
	.name	=	"shy_bee2",
	.bus	=	&platform_uplook,
	.probe	=	mydrv_probe,
	.remove	=	mydrv_remove,
};

module_driver(mydrv2, driver_register, driver_unregister);

MODULE_LICENSE("GPL");

MODULE_VERSION("2018-09-30");
MODULE_AUTHOR("millet");
MODULE_DESCRIPTION("a simple demo for module");
