#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/rwlock.h>
#include <linux/rwlock_types.h>
#include <linux/mm.h>
#include <linux/highmem.h>
#include <linux/fs.h>
#include <linux/mount.h>
#include <linux/slab.h>


#define PATH_BUF_LEN 8192

static struct file_system_type *file_systems;
static rwlock_t *file_systems_lock;

static struct file_system_type *ext4_type;

char path_buf[PATH_BUF_LEN];

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

static void traversal_mem_dentry(int len,struct dentry *dentry,char *buf)//traversal dentry in memory
{
	struct dentry *d;

	if(dentry == NULL){
		printk("%s\n",buf);
		return ;
	}
	
	if(len + 1 + strlen(dentry->d_name.name) >= PATH_BUF_LEN){
		traversal_mem_dentry(len,NULL,buf);
		printk("path_buf overflow\n");
		return ;
	}

	if(len == 0){
		memcpy(buf+len,dentry->d_name.name,strlen(dentry->d_name.name));
		len +=strlen(dentry->d_name.name);
		buf[len] = 0;
	}else{
		buf[len]='/';
		len++;
		memcpy(buf+len,dentry->d_name.name,strlen(dentry->d_name.name));
		len +=strlen(dentry->d_name.name);
		buf[len] = 0;
	}

	if(list_empty(&dentry->d_subdirs)){
		traversal_mem_dentry(len,NULL,buf);
		return ;
	}

	list_for_each_entry(d,&dentry->d_subdirs,d_child){
		traversal_mem_dentry(len,d,buf);
	}
}

static int __init fs_test_init(void)
{
	struct super_block *s;
	struct dentry *root_dentry;


	if(get_symbols() != 0)
		return -1;
	
	print_fs_type();

	ext4_type = get_fs_type("ext4");
	if(!ext4_type){
			printk("get ext4 fs type error\n");
			return -1;
	}

	hlist_for_each_entry(s,&ext4_type->fs_supers,s_instances){
		if((root_dentry = s->s_root) == NULL){
			continue;
		}
		
		traversal_mem_dentry(0,root_dentry,path_buf);
	}
	
	return 0;
}

static void __exit fs_test_exit(void)
{
	if(ext4_type)
			module_put(ext4_type->owner);
	printk("fs_test_exit func\n");
}

MODULE_LICENSE("GPL");
module_init(fs_test_init);
module_exit(fs_test_exit);
