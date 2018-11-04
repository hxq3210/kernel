#include <linux/module.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/interrupt.h>

#define DEVNAME	"spin"

#define LOOPCNT 3

static int glb_cnt = 0;

spinlock_t spin;

extern void unregister_key_irq(void);
extern int register_key_irq(void);
extern struct tasklet_struct task;
extern void do_key_bh(unsigned long data);

void print_info(char *str)
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
	spin_lock_bh(&spin);
	print_info((char *)arg);
	spin_unlock_bh(&spin);

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
	
	/*初始化下半部的任务对象*/
	tasklet_init(&task, do_key_bh, 0);
	
	ret = register_key_irq();
	if (ret < 0) {
		goto error0;
	}

	return 0;
error0:
	misc_deregister(&misc);

	return ret;
}

module_init(demo_init);

static void demo_exit(void)
{
	misc_deregister(&misc);
	tasklet_kill(&task);
	unregister_key_irq();

	printk("goodbye...\n");
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");

MODULE_VERSION("2018-09-30");
MODULE_AUTHOR("millet");
MODULE_DESCRIPTION("a simple demo for module");
