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

extern void print_info(char *str);
extern spinlock_t spin;

/*当按键中断发生后，内核自动调用到注册的这个处理函数，并将
  注册中断时的第五个参数传给此处理函数的第二形参*/
static irqreturn_t do_key(int irqnum, void *data)
{
	struct tiny4412_key *k = data;	
	char kbuf[SZ_64];
	unsigned long flag;

	k->cnt ^= 1;
	sprintf(kbuf, "key %d is %s.", k->keynum, (k->cnt)?"down":"up");

	spin_lock_irqsave(&spin, flag);
	print_info(kbuf);
	spin_unlock_irqrestore(&spin, flag);

	return IRQ_HANDLED; //IRQ_NONE;
}

int register_key_irq(void)
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

void unregister_key_irq(void)
{
	int i = 4;

	while (i--) {

		free_irq(keys[i].irqnum, keys+i);
	}
}

