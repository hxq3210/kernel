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
static struct input_dev *mttsdev;

static int xy[3][2] = {
	{200, 100},
	{300, 200},
	{400, 300}
};

static void do_timer(unsigned long data)
{
	struct tiny4412_key *k = (void *)data;	
	int index;
	int i;

	k->cnt ^= 1;
	index = k->keynum;

	if (k->cnt) {
		input_report_abs(mttsdev, ABS_MT_PRESSURE, 1);
	}  else {
		for (i = 0; i < 3; i++) {
			input_report_abs(mttsdev, ABS_MT_POSITION_X, xy[i][0]);
			input_report_abs(mttsdev, ABS_MT_POSITION_Y, xy[i][1]);
			xy[i][0]++;  xy[i][0] %= 800;
			xy[i][1]++;  xy[i][1] %= 480;
			input_mt_sync(mttsdev);
		}
		/*代表本时刻当前某一个点的坐标上报结束*/
		input_report_abs(mttsdev, ABS_MT_PRESSURE, 0);
	}

	/*代表本时刻所有数据全部上报结束*/
	input_sync(mttsdev);
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

	mttsdev = input_allocate_device();
	if (!mttsdev) {
		return -ENOMEM;
	}

	/*设置事件分类*/
	set_bit(EV_ABS, mttsdev->evbit);

	/*设置ABS的编码及坐标的最小值及最大值和误差*/
	set_bit(ABS_MT_POSITION_X, mttsdev->absbit);
	set_bit(ABS_MT_POSITION_Y, mttsdev->absbit);
	set_bit(ABS_MT_PRESSURE  , mttsdev->absbit);

	ret = input_register_device(mttsdev);
	if (ret < 0) {
		input_free_device(mttsdev);	
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
	input_unregister_device(mttsdev);

	return ret;
}

module_init(demo_init);

static void demo_exit(void)
{
	input_unregister_device(mttsdev);
	tasklet_kill(&task);
	unregister_key_irq();
	del_timer_sync(&timer);
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");

MODULE_VERSION("2018-09-30");
MODULE_AUTHOR("millet");
MODULE_DESCRIPTION("a simple demo for module");
