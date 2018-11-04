#include <linux/init.h>
#include <linux/module.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE2(ledcontrol, int, num, int, cmd)
{
	if (num < 1 || num > 4) {
		return -EINVAL;
	}

	if ((cmd != 0) && (cmd != 1)) {
		return -EINVAL;
	}

	printk("[kernel]: led%d is %s\n", num, cmd?"on":"off");

	return 0;
}
