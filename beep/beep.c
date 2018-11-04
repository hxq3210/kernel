#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/pwm.h>
#include <linux/gpio.h>
#include <plat/gpio-cfg.h>

#include "ioctl.h"

static struct pwm_device *pwm;
static int gpd0;

#define NS_IN_SEC 1000000000UL

static void set_beep_hz(int hz)
{
	int period = NS_IN_SEC/hz;	

	pwm_config(pwm, period >> 1, period);
	s3c_gpio_cfgpin(gpd0, S3C_GPIO_SFN(0x2)); //gpio-cfg.h
	pwm_enable(pwm);
}

static void stop_beep(void)
{
	pwm_disable(pwm);
	gpio_direction_output(gpd0, 0);
}

static long beep_ioctl (struct file *filp, unsigned int request, unsigned long arg)
{
	switch (request) {
		case BEEP_SET_HZ:
			set_beep_hz(arg);
			break;
		case BEEP_STOP:
			stop_beep();
			break;
		default:
			break;
	}

	return 0;
}

static struct file_operations fops = {
	.owner		=  THIS_MODULE,
	.unlocked_ioctl = beep_ioctl,
};

static struct miscdevice misc = {
	.minor	=  MISC_DYNAMIC_MINOR,
	.name	=  "beep",
	.fops	=  &fops,
};

static int beep_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct resource *res;

	pwm = pwm_request(0, "beep");
	if (IS_ERR(pwm)) {
		return PTR_ERR(pwm);
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (NULL == res) {
		ret = -EIO;
		goto error0;
	}

	gpd0 = res->start;
	if ((ret = gpio_request(gpd0, "beep")) < 0) {
		goto error1;
	}

	gpio_direction_output(gpd0, 0);

	ret = misc_register(&misc);
	if (ret < 0) {
		goto error2;
	}

	return 0;
error2:
	gpio_free(gpd0);
error1:
error0:
	pwm_free(pwm);

	return ret;
}

static int beep_remove(struct platform_device *pdev)
{
	stop_beep();	
	gpio_free(gpd0);
	pwm_free(pwm);
	misc_deregister(&misc);

	return 0;
}

static struct platform_driver beepdrv = {
	.probe	=	beep_probe,
	.remove	=	beep_remove,
	.driver	=	{
		.name	=  "beep",
	}
};

module_platform_driver(beepdrv);

MODULE_LICENSE("GPL");

MODULE_VERSION("2018-09-30");
MODULE_AUTHOR("millet");
MODULE_DESCRIPTION("a simple demo for module");
