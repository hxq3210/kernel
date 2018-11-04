#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>

static void	mydev_release (struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);

	printk("release %s device...\n", pdev->name);
}

/*此类型的数组代表了硬件的资源，与具体要操作
  的硬件寄存器，中断号GPIO的编号等等相关。*/
static struct resource	res[] = {
	{
		.start 	= 0x10060000,	 
		.end	= 0x10060000 + 0xc + 3,
		.name	= "watchdog",
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= 75,
		.end	= 75,
		.name	= "wdtirq",
		.flags	= IORESOURCE_IRQ,
	}
};

static struct platform_device mydev1 = {
	.name			=	"sunshine",
	.id			=	-1,
	.dev			=	{
		.release	=	mydev_release,
	},	
	.num_resources		=	ARRAY_SIZE(res),
	.resource		=	res,
};

module_driver(mydev1, platform_device_register, platform_device_unregister);

MODULE_LICENSE("GPL");

MODULE_VERSION("2018-09-30");
MODULE_AUTHOR("millet");
MODULE_DESCRIPTION("a simple demo for module");
