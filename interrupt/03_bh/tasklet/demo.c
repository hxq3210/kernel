#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>

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

static void do_key_bh(unsigned long data)
{
	struct tiny4412_key *k = (void *)data;	

	k->cnt ^= 1;
	printk("key %d is %s.\n", k->keynum, (k->cnt)?"down":"up");

	if (in_interrupt()) {
		printk("%s function run interrupt text...\n", __func__);
	} else {
		printk("%s function run process text...\n", __func__);
	}
}

/*当按键中断发生后，内核自动调用到注册的这个处理函数，并将
  注册中断时的第五个参数传给此处理函数的第二形参*/
static irqreturn_t do_key(int irqnum, void *data)
{

	task.data = (unsigned long)data;

	printk("In %s...\n", __func__);

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
	/*初始化下半部的任务对象*/
	tasklet_init(&task, do_key_bh, 0);

	return register_key_irq();
}

module_init(demo_init);

static void demo_exit(void)
{
	tasklet_kill(&task);

	unregister_key_irq();
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");

MODULE_VERSION("2018-09-30");
MODULE_AUTHOR("millet");
MODULE_DESCRIPTION("a simple demo for module");
