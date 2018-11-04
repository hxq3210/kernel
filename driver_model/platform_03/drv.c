#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>

static int myprobe(struct platform_device *pdev)
{
	printk("%s device match ok %s driver...\n",
			pdev->name, pdev->dev.driver->name);

	return 0;
}

static int myremove(struct platform_device *pdev)
{
	printk("%s device dismatch ok %s driver...\n",
			pdev->name, pdev->dev.driver->name);

	return 0;
}

const struct platform_device_id tables[] = {
	{"lele", },
	{"sunshine", },
	{"summer", },
	{} //哨兵
};

static struct platform_driver mydrv = {
	.probe		= myprobe,
	.remove		= myremove,
	.id_table 	= tables,
	.driver		= {
			.name = "flower",
	}
};

module_platform_driver(mydrv);

MODULE_LICENSE("GPL");

MODULE_VERSION("2018-09-30");
MODULE_AUTHOR("millet");
MODULE_DESCRIPTION("a simple demo for module");
