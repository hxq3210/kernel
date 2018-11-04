#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/kernel.h>

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
 */

#define N 180

struct phonote{
	char name[SZ_64];
	long long phonum;
	struct list_head list;
};

/*创建一个头结点为head的双向循环链表*/
LIST_HEAD(head);

static int demo_init(void)
{
	int i;
	struct phonote *pesn;
	struct list_head *pos, *n;
	int ret = 0;

	for (i = 0; i < N; i++) {
		//pesn = kmalloc(sizeof(struct phonote), GFP_KERNEL);
		pesn = (void *)__get_free_pages(GFP_KERNEL, 0);
		if (NULL == pesn) {
			ret = -ENOMEM;
			goto error0;
		}
		sprintf(pesn->name, "xiaoli_%d", i+1);
		pesn->phonum = 17782655000 + i;
		list_add_tail(&pesn->list, &head);
	}


	list_for_each(pos, &head) {
		pesn = container_of(pos, struct phonote, list);	
		printk("%s: %lld\n", pesn->name, pesn->phonum);
	}

	return ret;
error0:
	list_for_each_safe(pos, n, &head) {
		pesn = container_of(pos, struct phonote, list);	
		printk("goodbye %s\n", pesn->name);
		free_pages((unsigned long)pesn, 0);
	}

	return ret;
}

module_init(demo_init);

static void demo_exit(void)
{
	struct phonote *pesn;
	struct list_head *pos, *n;

	list_for_each_safe(pos, n, &head) {
		pesn = container_of(pos, struct phonote, list);	
		printk("goodbye %s\n", pesn->name);
		free_pages((unsigned long)pesn, 0);
	}
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");

MODULE_VERSION("2018-09-30");
MODULE_AUTHOR("millet");
MODULE_DESCRIPTION("a simple demo for module");
