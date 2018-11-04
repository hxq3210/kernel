#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/device.h>

#define DEVNAME "kbuf"

struct kbuf_dev {
	char *kbuf;
	size_t cnt;
};

struct kbuf_dev kbufdev0 = {.cnt = 0};

static int major = 111;

static struct class *class;

static int     demo_open (struct inode *inodp, struct file *filp)
{
	printk("----------- open major: %d, minor: %d ------------\n",
			imajor(inodp), iminor(inodp));

	return 0;
}

static ssize_t demo_read (struct file *filp, char __user *buf, size_t cnt, loff_t *fpos)
{
	if (cnt == 0) {
		return cnt;
	}

	if (kbufdev0.cnt == 0) {
		return -EIO;
	}

	cnt = min(cnt, kbufdev0.cnt);

	if (copy_to_user(buf, kbufdev0.kbuf, cnt)) {
		return -EIO;	
	}

	memmove(kbufdev0.kbuf, kbufdev0.kbuf + cnt, kbufdev0.cnt - cnt);

	kbufdev0.cnt -= cnt;

	return cnt;
}

static ssize_t 
demo_write (struct file *filp, const char __user *buf, size_t cnt, loff_t *fpos)
{
	if (cnt == 0) {
		return cnt;
	}	

	/*非阻塞型IO*/
	if (kbufdev0.cnt == SZ_512) {
		return -EIO;
	}

	cnt = min(cnt, SZ_512 - kbufdev0.cnt);

	if (copy_from_user(kbufdev0.kbuf+kbufdev0.cnt,  buf, cnt)) {
		return -EIO;
	}

	kbufdev0.cnt += cnt;

	return cnt;
}

static int     demo_release (struct inode *inodp, struct file *filp)
{
	printk("----------- close major: %d, minor: %d ------------\n",
			imajor(inodp), iminor(inodp));

	return 0;
}

static struct file_operations fops = {
	.owner		=   THIS_MODULE,
	.open		=   demo_open,
	.read		=   demo_read,
	.write		=   demo_write,
	.release 	=   demo_release,	
};

static int demo_init(void)
{
	int ret = 0;
	struct device *dev;

	kbufdev0.kbuf = kmalloc(SZ_512, GFP_KERNEL);
	if (NULL == kbufdev0.kbuf) {
		ret = -ENOMEM;
		goto error0;
	}

	ret = register_chrdev(major, DEVNAME, &fops);
	if (ret < 0) {
		goto error1;
	}

	class = class_create(THIS_MODULE, DEVNAME);
	if (IS_ERR(class)) {
		goto error2;
	}

	/*用户生成设备文件*/
	dev = device_create(class, NULL, MKDEV(major, 0), NULL, DEVNAME);
	if (IS_ERR(dev)) {
		goto error3;
	}

	return 0;
error3:
	class_destroy(class);
error2:
	unregister_chrdev(major, DEVNAME);	
error1:
	kfree(kbufdev0.kbuf);
error0:
	return ret;
}

module_init(demo_init);

static void demo_exit(void)
{
	device_destroy(class, MKDEV(major, 0));
	class_destroy(class);
	unregister_chrdev(major, DEVNAME);	
	kfree(kbufdev0.kbuf);
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");

MODULE_VERSION("2018-09-30");
MODULE_AUTHOR("millet");
MODULE_DESCRIPTION("a simple demo for module");
