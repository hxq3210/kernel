#include <linux/module.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/delay.h>

#define DEVNAME	"spin"

#define LOOPCNT 3

static int glb_cnt = 0;

static spinlock_t spin;

static void print_info(char *str)
{
	int i;

	for (i = 0; i < LOOPCNT; i++) {
		printk("%d -> : %s\n", glb_cnt, str);
		glb_cnt++;
		mdelay(50);
	}
}

static long 
demo_ioctl (struct file *filp, unsigned int request, unsigned long arg)
{
	spin_lock(&spin);
	print_info((char *)arg);
	spin_unlock(&spin);

	return 0;
}

static struct file_operations fops = {
	.owner		=  	THIS_MODULE,
	.unlocked_ioctl	=	demo_ioctl,

};

static struct miscdevice misc = {
	.minor	= 	MISC_DYNAMIC_MINOR,
	.name	=	DEVNAME,
	.fops	=	&fops,
};

static int demo_init(void)
{
	int ret;

	spin_lock_init(&spin);

	ret = misc_register(&misc);
	if (ret < 0) {
		return ret;
	}

	return 0;
}

module_init(demo_init);

static void demo_exit(void)
{
	misc_deregister(&misc);

	printk("goodbye...\n");
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");

MODULE_VERSION("2018-09-30");
MODULE_AUTHOR("millet");
MODULE_DESCRIPTION("a simple demo for module");
