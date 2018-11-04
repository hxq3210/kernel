#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>


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

#define GPM4BASE 0x11000000
#define GPM4CON  0x2e0
#define GPM4DAT  0x2e4

#define DEVNAME(x) #x

static char *gpm4base;

static void ledctl(int cmd, int num)
{
	u8 val;

	val = ioread8(gpm4base+GPM4DAT);

	if (cmd == LEDON) {
		val &= ~(1 << (num-1));	
	} else {
		val |= (1 << (num - 1));
	}	

	iowrite8(val, gpm4base+GPM4DAT);
}

static int ledstat(char *buf)
{
	u8 val;
	int i;

	char kbuf[4];

	val = ioread8(gpm4base+GPM4DAT);
	for (i = 0; i < 4; i++) {
		kbuf[i] = !((val>>i)&0x1);
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
	u32 val;

	gpm4base = ioremap(GPM4BASE, SZ_4K);
	if (NULL == gpm4base) {
		ret = -ENOMEM;
		return ret;
	}

	/*设置GPM4[0:3]为输出功能*/
	val = ioread32(gpm4base+GPM4CON);
	val &= ~0xffff;
	val |= 0x1111;
	iowrite32(val, gpm4base+GPM4CON);

	/*让GPM4[0:3]输出高电平，让LED灭*/
	val = ioread8(gpm4base+GPM4DAT);
	val |= 0xf;
	iowrite8(val, gpm4base+GPM4DAT);

	ret = misc_register(&misc);
	if (ret < 0) {
		goto error;
	}

	printk("insmod %s driver ok.\n", DEVNAME(leds));

	return 0;
error:
	iounmap(gpm4base);

	return ret;
}

module_init(demo_init);

static void demo_exit(void)
{
	u8 val;

	val = ioread8(gpm4base+GPM4DAT);
	val |= 0xf;
	iowrite8(val, gpm4base+GPM4DAT);

	iounmap(gpm4base);
	misc_deregister(&misc);

	printk("rmmod %s driver ok.\n", DEVNAME(leds));
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");

MODULE_VERSION("2018-09-30");
MODULE_AUTHOR("millet");
MODULE_DESCRIPTION("a simple demo for module");
