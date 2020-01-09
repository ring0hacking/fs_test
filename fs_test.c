#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/rwlock.h>
#include <linux/rwlock_types.h>
#include <linux/mm.h>
#include <linux/highmem.h>
#include <linux/fs.h>

static struct file_system_type *file_systems;
static rwlock_t *file_systems_lock;

static int get_symbols(void)
{
	file_systems = *(struct file_system_type **)kallsyms_lookup_name("file_systems");
	if(file_systems == NULL){
		printk("Not found file_systems symbol");
		return -1;
	}
	
	file_systems_lock = (rwlock_t *)kallsyms_lookup_name("file_systems_lock");
	if(file_systems == NULL){
		printk("Not found file_systems_lock symbol");
		return -1;
	}
	return 0;
}

static void print_fs_type(void)
{
	struct file_system_type *tmp;
	
	read_lock(file_systems_lock);

	tmp = file_systems;
	while(tmp){
		printk("fs type name:%s\n",tmp->name);
		tmp = tmp->next;
	}
	
	read_unlock(file_systems_lock);
}

static int __init fs_test_init(void)
{

	if(get_symbols() != 0){
		return -1;
	}
	
	print_fs_type();

	return 0;
}

static void __exit fs_test_exit(void)
{
	printk("fs_test_exit func\n");
}

MODULE_LICENSE("GPL");
module_init(fs_test_init);
module_exit(fs_test_exit);
