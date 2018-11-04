#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>

static void	mydev_release (struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);

	printk("release %s device...\n", pdev->name);
}

static struct platform_device mydev2 = {
	.name	=	"summer",
	.id	=	-1,
	.dev	=	{
		.release	=	mydev_release,
	},	
};

module_driver(mydev2, platform_device_register, platform_device_unregister);

MODULE_LICENSE("GPL");

MODULE_VERSION("2018-09-30");
MODULE_AUTHOR("millet");
MODULE_DESCRIPTION("a simple demo for module");
