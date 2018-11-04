#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/sizes.h>

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
	struct phonote *pesn, *pos, *next;
	int ret = 0;

	for (i = 0; i < N; i++) {
		pesn = kmalloc(sizeof(struct phonote), GFP_KERNEL);
		if (NULL == pesn) {
			ret = -ENOMEM;
			goto error0;
		}
		sprintf(pesn->name, "xiaoli_%d", i+1);
		pesn->phonum = 17782655000 + i;
		list_add_tail(&pesn->list, &head);
	}


	list_for_each_entry(pos, &head, list) {
		printk("%s: %lld\n", pos->name, pos->phonum);
	}

	return ret;
error0:
	list_for_each_entry_safe(pos, next, &head, list) {
		printk("goodbye %s\n", pos->name);
		kfree(pos);
	}

	return ret;
}

module_init(demo_init);

static void demo_exit(void)
{
	struct phonote *pos, *next;

	list_for_each_entry_safe(pos, next, &head, list) {
		printk("goodbye %s\n", pos->name);
		kfree(pos);
	}
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");

MODULE_VERSION("2018-09-30");
MODULE_AUTHOR("millet");
MODULE_DESCRIPTION("a simple demo for module");
