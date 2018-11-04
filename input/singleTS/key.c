#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/timer.h>
#include <linux/input.h>

struct tiny4412_key {
	int code;
	int keynum;
	int irqnum;
	int cnt;
}keys[4] = {
	{KEY_UP   , 0, IRQ_EINT(26), 0},
	{KEY_DOWN , 1, IRQ_EINT(27), 0},
	{KEY_SPACE, 2, IRQ_EINT(28), 0},
	{KEY_ESC  , 3, IRQ_EINT(29), 0},
};

/*定义一个下半部的任务对象，选择tasklet机制*/
static struct tasklet_struct task;

/*使用内核定时器消除抖动*/
static struct timer_list timer;

/*四个按键看成是一个输入设备，定义输入设备对应的结构体变量*/
static struct input_dev *tsdev;

static int x = 400, y = 240;

static void do_timer(unsigned long data)
{
	struct tiny4412_key *k = (void *)data;	
	int index;

	k->cnt ^= 1;
	index = k->keynum;

	if (k->cnt) {
		x++; 
		x %= 800;
		y++; 
		y %= 480;
		input_report_abs(tsdev, ABS_PRESSURE, 1);
	}  else {
		input_report_abs(tsdev, ABS_PRESSURE, 0);
		input_report_abs(tsdev, ABS_X, x);
		input_report_abs(tsdev, ABS_Y, y);
	}

	input_sync(tsdev);
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

static int demo_init(void)
{
	int ret;

	tsdev = input_allocate_device();
	if (!tsdev) {
		return -ENOMEM;
	}

	/*设置事件分类*/
	set_bit(EV_ABS, tsdev->evbit);

	/*设置ABS的编码及坐标的最小值及最大值和误差*/
 	input_set_abs_params(tsdev, ABS_X, 0, 799, 0, 0);
 	input_set_abs_params(tsdev, ABS_Y, 0, 479, 0, 0);
 	input_set_abs_params(tsdev, ABS_PRESSURE, 0, 1, 0, 0);

	ret = input_register_device(tsdev);
	if (ret < 0) {
		input_free_device(tsdev);	
		return ret;
	}

	/*初始化下半部的任务对象*/
	tasklet_init(&task, do_key_bh, 0);

	setup_timer(&timer, do_timer, 0);
	//mod_timer();

	ret = register_key_irq();
	if (ret < 0) {
		goto error0;
	}

	return 0;
error0:
	input_unregister_device(tsdev);

	return ret;
}

module_init(demo_init);

static void demo_exit(void)
{
	input_unregister_device(tsdev);
	tasklet_kill(&task);
	unregister_key_irq();
	del_timer_sync(&timer);
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");

MODULE_VERSION("2018-09-30");
MODULE_AUTHOR("millet");
MODULE_DESCRIPTION("a simple demo for module");
