#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

static int __init fs_test_init(void)
{
	printk("fs_test_init func\n");

	return 0;
}

static void __exit fs_test_exit(void)
{
	printk("fs_test_exit func\n");
}

MODULE_LICENSE("GPL");
module_init(fs_test_init);
module_exit(fs_test_exit);
