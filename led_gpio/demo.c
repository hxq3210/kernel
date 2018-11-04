#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>

/*
 	一个主设备号对应内核的一套驱动，也就是说同一个主设备号，对应的
	多个次设备号的设备文件他们的驱动是一样的。(主设备号为10的设备文件除过)

	主设备号为10的设备文件，不同的次设备号对应各自的一套驱动。
	在内核注册主设备号为10的字符设备驱动时，只能一个次设备号对应一个设备
	并对应一套驱动。

	内核注册主设备号为10的字符设备驱动方法为混杂字符设备驱动:

	misc_register();/misc_deregister();
 */
#include "ioctl.h"

#define DEVNAME(x) #x

static int ledgpio[4] = {
	EXYNOS4X12_GPM4(0),
	EXYNOS4X12_GPM4(1),
	EXYNOS4X12_GPM4(2),
	EXYNOS4X12_GPM4(3)
};

static void ledctl(int cmd, int num)
{
	if (cmd == LEDON) {
		gpio_set_value(ledgpio[num-1], 0);
	} else {
		gpio_set_value(ledgpio[num-1], 1);
	}	
}

static int ledstat(char *buf)
{
	int i;
	char kbuf[4];

	for (i = 0; i < 4; i++) {
		kbuf[i] = !gpio_get_value(ledgpio[i]);
	}
	
	if (copy_to_user(buf, kbuf, 4)) {
		return -EIO;
	}

	return 0;
}

	static long 
led_ioctl (struct file *filp, unsigned int request, unsigned long arg)
{
	switch (request) {
		case LEDON:
			if ((arg < 1) || (arg > 4)) {
				return -EINVAL;
			}
			ledctl(LEDON, arg);
			break;
		case LEDOFF:
			if ((arg < 1) || (arg > 4)) {
				return -EINVAL;
			}
			ledctl(LEDOFF, arg);
			break;
		case LEDSTAT:
			return ledstat((char *)arg);
		default:
			break;
	}	

	return 0;
}

static struct file_operations fops = {
	.unlocked_ioctl	=  led_ioctl,	
};

static struct miscdevice misc = {
	.minor	=	MISC_DYNAMIC_MINOR,	
	.name	=	DEVNAME(leds),
	.fops	=	&fops,
};

static int demo_init(void)
{
	int ret;
	int i;

	for (i = 0; i < 4; i++) {
		ret = gpio_request(ledgpio[i], "led");
		if (ret < 0) {
			goto error0;
		}
		gpio_direction_output(ledgpio[i], 1);
	}

	/*设置GPM4[0:3]为输出功能*/

	/*让GPM4[0:3]输出高电平，让LED灭*/

	ret = misc_register(&misc);
	if (ret < 0) {
		goto error0;
	}

	printk("insmod %s driver ok.\n", DEVNAME(leds));

	return 0;

error0:
	while (i--) {
		gpio_free(ledgpio[i]);
	}
	return ret;
}

module_init(demo_init);

static void demo_exit(void)
{
	int i;

	for (i = 0; i < 4; i++) {
		gpio_set_value(ledgpio[i], 1);
		gpio_free(ledgpio[i]);
	}

	misc_deregister(&misc);

	printk("rmmod %s driver ok.\n", DEVNAME(leds));
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");

MODULE_VERSION("2018-09-30");
MODULE_AUTHOR("millet");
MODULE_DESCRIPTION("a simple demo for module");
