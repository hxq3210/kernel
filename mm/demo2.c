#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/dma-mapping.h>

/*
 	kmalloc/kfree
	kcalloc/kfree
	krealloc/kfree
	kzalloc/kfree

	申请内存空间需要的标记:
	
	GFP_KERNEL:	申请内存的函数接口可以睡眠阻塞，暂时不能满足时阻塞。
	GFP_ATOMIC:	申请内存的函数接口不可以阻塞，失败直接返回。

	vmalloc/vfree
	vzalloc/vfree

	__get_free_pages/free_pages  
	作业：在板子上测试，order可以传递的最大值是多少。

	__get_free_page/free_page

	dma_alloc_coherent/dma_free_coherent


	0xc0000000 -> 3*2^2*16^7 = 3*2^30 = 3G   ~   4G-1
	0x40000000 -> 0x80000000
 */

static dma_addr_t phyadd;
static char *virtueadd;

static int demo_init(void)
{
	virtueadd = dma_alloc_coherent(NULL, SZ_64, &phyadd, GFP_KERNEL);
	if (NULL == virtueadd) {
		return -ENOMEM;
	}

	sprintf(virtueadd, "hello, bye...\n");
	printk("%s", virtueadd);

	printk("virtue: 0x%p, phyadd: %#x\n", virtueadd, phyadd);

	return 0;
}

module_init(demo_init);

static void demo_exit(void)
{
	dma_free_coherent(NULL, SZ_64, virtueadd, phyadd);
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");

MODULE_VERSION("2018-09-30");
MODULE_AUTHOR("millet");
MODULE_DESCRIPTION("a simple demo for module");
