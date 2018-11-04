#include <linux/module.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/delay.h>

#define DEVNAME	"mutex"

/*
 	产生竞争的全局变量，用于表示读写的满足条件，读一次
	更新成false表示空，写一次更新成true,表示满，只有可
	读写时方可读写。
 */
static int is_full = false;

static struct mutex mutex;

static int demo_open (struct inode *inodp, struct file *filp)
{
	if (!mutex_trylock(&mutex)) {
		return -EBUSY;
	}	


	return 0;
}

static int demo_release (struct inode *inodp, struct file *filp)
{
	mutex_unlock(&mutex);

	return 0;
}

ssize_t 
demo_read (struct file *filp, char __user *buf, size_t cnt, loff_t *fpos)
{
	int ret;

	if (cnt == 0) {
		return cnt;
	}	


	if (is_full == true) {
		printk("[kernel]: reading...\n");
		mdelay(50);
		is_full = false;
		ret = cnt;
		printk("[kernel]: read done.\n");
	} else {
		printk("In %s at %dline...\n", __func__, __LINE__);
		ret = -EIO;
	}


	return ret;
}

ssize_t 
demo_write (struct file *filp, const char __user *buf, size_t cnt, loff_t *fpos)
{
	int ret;

	if (cnt == 0) {
		return cnt;
	}

	if (is_full == false) {

		printk("[kernel]: writing...\n");
		mdelay(50);
		is_full = true;
		printk("[kernel.]: write done.\n");

		ret = cnt;
	} else {
		ret = -EIO;
	}

	return ret;
}

static struct file_operations fops = {
	.owner		=  	THIS_MODULE,
	.open		=	demo_open,
	.read		=	demo_read,
	.write		=	demo_write,
	.release	= 	demo_release,	
};

static struct miscdevice misc = {
	.minor	= 	MISC_DYNAMIC_MINOR,
	.name	=	DEVNAME,
	.fops	=	&fops,
};

static int demo_init(void)
{
	int ret;

	/*初始化信号量*/
	mutex_init(&mutex);

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
