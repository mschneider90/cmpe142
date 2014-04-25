#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>

extern int register_filesystem(struct file_system_type*);
extern int unregister_filesystem(struct file_system_type*);

struct file_system_type sjfsInfo = {
	.name = "cmpefs",
	.owner = THIS_MODULE,
	.next = NULL
};

static int __init hello_init()
{
	return register_filesystem(&sjfsInfo);
}

static int __exit hello_exit()
{
	return unregister_filesystem(&sjfsInfo);
}

module_init(hello_init);
module_exit(hello_exit);


