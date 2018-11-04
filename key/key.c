#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/timer.h>

#define DEVNAME "keys"

struct tiny4412_key {
	int keynum;
	int irqnum;
	int cnt;
}keys[4] = {
	{0, IRQ_EINT(26), 0},
	{1, IRQ_EINT(27), 0},
	{2, IRQ_EINT(28), 0},
	{3, IRQ_EINT(29), 0},
};

/*定义一个下半部的任务对象，选择tasklet机制*/
static struct tasklet_struct task;

/*用于实现读进程的阻塞*/
static wait_queue_head_t wait;

/*此变量用于标记按键的状态*/
static int dnup_flg = 0;

/*使用内核定时器消除抖动*/
static struct timer_list timer;

static char keystat[4] = { 0 };

static void do_timer(unsigned long data)
{
	struct tiny4412_key *k = (void *)data;	
	int index;

	k->cnt ^= 1;
	index = k->keynum;

	keystat[index] = k->cnt;

	dnup_flg = 1;
	wake_up_interruptible(&wait);
}

static void do_key_bh(unsigned long data)
{
	timer.data = data;

	mod_timer(&timer, jiffies + msecs_to_jiffies(50));
}

/*当按键中断发生后，内核自动调用到注册的这个处理函数，并将
  注册中断时的第五个参数传给此处理函数的第二形参*/
static irqreturn_t do_key(int irqnum, void *data)
{

	task.data = (unsigned long)data;

	/*将下半部任务交给内核调度器调度*/
	tasklet_schedule(&task);

	return IRQ_HANDLED; //IRQ_NONE;
}

static int register_key_irq(void)
{
	int i;
	int ret;

	for (i = 0; i < 4; i++) {
		ret = request_irq(keys[i].irqnum, do_key, 
					IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
						"keys", keys+i);
		if (ret < 0) {
			goto error0;
		}
	}

	return 0;
error0:
	while (i--) {
		free_irq(keys[i].irqnum, keys+i);
	}

	return ret;
}

static void unregister_key_irq(void)
{
	int i = 4;

	while (i--) {

		free_irq(keys[i].irqnum, keys+i);
	}
}

static ssize_t 
key_read (struct file *filp, char __user *buf, size_t cnt, loff_t *fpos)
{
	if (cnt != 4) {
		return -EINVAL;
	}

	wait_event_interruptible(wait, dnup_flg == 1);

	if (copy_to_user(buf, keystat, 4)) {
		return -EIO;	
	}

	dnup_flg = 0;

	return cnt;
}

static struct file_operations fops = {
	.owner 	= THIS_MODULE,
	.read	= key_read,
};

static struct miscdevice misc = {
	.minor	=  MISC_DYNAMIC_MINOR,
	.name	=  DEVNAME,
	.fops	=  &fops,
};

static int demo_init(void)
{
	int ret;

	ret = misc_register(&misc);
	if (ret < 0) {
		return ret;
	}

	init_waitqueue_head(&wait);

	/*初始化下半部的任务对象*/
	tasklet_init(&task, do_key_bh, 0);

	setup_timer(&timer, do_timer, 0);
	//mod_timer();

	return register_key_irq();
}

module_init(demo_init);

static void demo_exit(void)
{
	tasklet_kill(&task);
	unregister_key_irq();
	del_timer_sync(&timer);
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");

MODULE_VERSION("2018-09-30");
MODULE_AUTHOR("millet");
MODULE_DESCRIPTION("a simple demo for module");
