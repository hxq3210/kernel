#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>

#include "mydevice.h"

static int uplook_match (struct device *dev, struct device_driver *drv)
{
	struct mydevice *pdev = container_of(dev, struct mydevice, dev);	

	printk("%s device try to match %s driver...\n", pdev->name, drv->name);

	return !strcmp(pdev->name, drv->name);
	//匹配成功返回1， 否则0	
}

static struct bus_type platform_uplook = {
	.name	=	"platform_uplook",
	.match	=	uplook_match,
};
EXPORT_SYMBOL_GPL(platform_uplook);

module_driver(platform_uplook, bus_register, bus_unregister);

MODULE_LICENSE("GPL");

MODULE_VERSION("2018-09-30");
MODULE_AUTHOR("millet");
MODULE_DESCRIPTION("a simple demo for module");
