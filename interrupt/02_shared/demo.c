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

/*当按键中断发生后，内核自动调用到注册的这个处理函数，并将
  注册中断时的第五个参数传给此处理函数的第二形参*/
static irqreturn_t do_key1(int irqnum, void *data)
{
	struct tiny4412_key *k = data;	

	k->cnt ^= 1;
	printk("key %d is %s.\n", k->keynum, (k->cnt)?"down":"up");

	return IRQ_HANDLED; //IRQ_NONE;
}

static irqreturn_t do_key2(int irqnum, void *data)
{
	int n = (int)data;

	printk("\nn = %d\n", n);

	return IRQ_HANDLED; //IRQ_NONE;
}

static int register_key_irq(void)
{
	int i;
	int ret;

	for (i = 0; i < 4; i++) {
		ret = request_irq(keys[i].irqnum, do_key1, 
				IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING | IRQF_SHARED,
						"keys1", keys+i);
		if (ret < 0) {
			goto error0;
		}
		ret = request_irq(keys[i].irqnum, do_key2, 
				IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING | IRQF_SHARED,
						"keys2", (void *)(i+1));
		if (ret < 0) {
			free_irq(keys[i].irqnum, keys+i);
			goto error0;
		}
	}

	return 0;
error0:
	while (i--) {
		free_irq(keys[i].irqnum, keys+i);
		free_irq(keys[i].irqnum, (void *)(1+i));
	}

	return ret;
}

static void unregister_key_irq(void)
{
	int i = 4;

	while (i--) {
		free_irq(keys[i].irqnum, keys+i);
		free_irq(keys[i].irqnum, (void *)(1+i));
	}
}

static int demo_init(void)
{
	return register_key_irq();
}

module_init(demo_init);

static void demo_exit(void)
{
	unregister_key_irq();
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");

MODULE_VERSION("2018-09-30");
MODULE_AUTHOR("millet");
MODULE_DESCRIPTION("a simple demo for module");
