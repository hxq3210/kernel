#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <plat/adc.h>

#define CHANNEL 0

static struct s3c_adc_client *pcli;

/*
 	copy_to_user(usradd, kernel_add, cnt);/put_user();
	copy_from_user(kernel_add, useradd, cnt);/get_user();
 */

static  ssize_t 
resistance_read (struct file *filp, char __user *buf, size_t cnt, loff_t *fpos)
{
	int result;
	int ret;

	if (cnt != sizeof(int)) {
		return -EINVAL;
	}

	result = s3c_adc_read(pcli, CHANNEL);
	if (result < 0) {
		return result;
	}

	result &= 0xfff;
	ret = put_user(result, (int *)buf);
	if (ret < 0) {
		return ret;
	}

	return cnt;
}

static struct file_operations fops = {
	.owner	=  THIS_MODULE,
	.read	=  resistance_read,
};

static struct miscdevice misc = {
	.minor	=  MISC_DYNAMIC_MINOR,
	.name	=  "resistance",
	.fops	=  &fops,
};

static  int resistance_probe(struct platform_device *pdev)
{
	int ret;

	pcli = s3c_adc_register(pdev, NULL, NULL, 0);
	if (IS_ERR(pcli)) {
		return -ENOMEM;
	}

	ret = misc_register(&misc);

	if (ret < 0) {
		goto error0;
	}

	return 0;

error0:
	s3c_adc_release(pcli);

	return ret;
}

static  int resistance_remove(struct platform_device *pdev)
{
	misc_deregister(&misc);
	s3c_adc_release(pcli);

	return 0;
}

static struct platform_driver resistancedrv = {
	.probe	=	resistance_probe,
	.remove	=	resistance_remove,
	.driver	=	{
		.name	=  "resistance",
	}
};

module_platform_driver(resistancedrv);

MODULE_LICENSE("GPL");

MODULE_VERSION("2018-09-30");
MODULE_AUTHOR("millet");
MODULE_DESCRIPTION("a simple demo for module");
