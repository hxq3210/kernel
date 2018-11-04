#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/wait.h>
#include <linux/sched.h>

#define CNT 200

#define DEVNAME "kbuf"

struct kbuf_dev {
	char *kbuf;
	size_t cnt;
	struct cdev cdev;
	wait_queue_head_t rdwait;
	wait_queue_head_t wrwait;
};

/*定义了200个设备对应的变量*/
static struct kbuf_dev kbufdevs[CNT];

static dev_t devnum;

static struct class *class;

static int     demo_open (struct inode *inodp, struct file *filp)
{
	printk("----------- open major: %d, minor: %d ------------\n",
			imajor(inodp), iminor(inodp));

	//filp->private_data = kbufdevs + iminor(inodp);
	filp->private_data = container_of(inodp->i_cdev, struct kbuf_dev, cdev);

	return 0;
}

static ssize_t demo_read (struct file *filp, char __user *buf, size_t cnt, loff_t *fpos)
{
	int ret;
	struct kbuf_dev *kbufdev = filp->private_data;

	if (cnt == 0) {
		return cnt;
	}

	/*没有资源可读时，阻塞*/
	ret = wait_event_interruptible(kbufdev->rdwait, kbufdev->cnt != 0);
	if (ret < 0) {
		return ret;
	}

	cnt = min(cnt, kbufdev->cnt);

	if (copy_to_user(buf, kbufdev->kbuf, cnt)) {
		return -EIO;	
	}

	memmove(kbufdev->kbuf, kbufdev->kbuf + cnt, kbufdev->cnt - cnt);

	kbufdev->cnt -= cnt;
	wake_up(&kbufdev->wrwait);

	return cnt;
}

static ssize_t 
demo_write (struct file *filp, const char __user *buf, size_t cnt, loff_t *fpos)
{
	int ret;

	struct kbuf_dev *kbufdev = filp->private_data;

	if (cnt == 0) {
		return cnt;
	}	

	ret = wait_event_interruptible(kbufdev->wrwait, kbufdev->cnt != SZ_512);
	if (ret < 0) {
		return ret;
	}

	cnt = min(cnt, SZ_512 - kbufdev->cnt);

	if (copy_from_user(kbufdev->kbuf+kbufdev->cnt,  buf, cnt)) {
		return -EIO;
	}

	kbufdev->cnt += cnt;
	wake_up(&kbufdev->rdwait); //唤醒阻塞的读进程

	return cnt;
}

static int     demo_release (struct inode *inodp, struct file *filp)
{
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
	int i = 0;

	class = class_create(THIS_MODULE, DEVNAME);
	if (IS_ERR(class)) {
		ret  = PTR_ERR(class);
		goto error0;
	}

	ret = alloc_chrdev_region(&devnum, 0, CNT, DEVNAME);
	if (ret < 0) {
		goto error1;
	}

	for (i = 0; i < CNT; i++) {
		kbufdevs[i].kbuf = kmalloc(SZ_512, GFP_KERNEL);
		if (NULL == kbufdevs[i].kbuf) {
			ret = -ENOMEM;
			goto error2;
		}

		cdev_init(&kbufdevs[i].cdev, &fops);
		ret = cdev_add(&kbufdevs[i].cdev, devnum+i, 1);
		if (ret < 0) {
			kfree(kbufdevs[i].kbuf);
			goto error2;
		}
	
		/*用户生成设备文件*/
		dev = device_create(class, NULL, devnum+i, NULL, "kbuf%d", i);
		if (IS_ERR(dev)) {
			cdev_del(&kbufdevs[i].cdev);
			kfree(kbufdevs[i].kbuf);
			goto error2;
		}

		init_waitqueue_head(&kbufdevs[i].rdwait);
		init_waitqueue_head(&kbufdevs[i].wrwait);
	}


	return 0;
error2:
	unregister_chrdev_region(devnum, CNT);

	while (i--) {
		device_destroy(class, devnum+i);
		cdev_del(&kbufdevs[i].cdev);
		kfree(kbufdevs[i].kbuf);
	}
error1:
	class_destroy(class);
error0:
	return ret;
}

module_init(demo_init);

static void demo_exit(void)
{
	int i = CNT;

	unregister_chrdev_region(devnum, CNT);
	while (i--) {
		device_destroy(class, devnum+i);
		cdev_del(&kbufdevs[i].cdev);
		kfree(kbufdevs[i].kbuf);
	}
	class_destroy(class);
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");

MODULE_VERSION("2018-09-30");
MODULE_AUTHOR("millet");
MODULE_DESCRIPTION("a simple demo for module");
