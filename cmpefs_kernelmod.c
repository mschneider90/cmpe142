#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>

extern int register_filesystem(struct file_system_type*);
extern int unregister_filesystem(struct file_system_type*);

struct file_system_type sjfsInfo = {
	.name = "cmpefs",
	.owner = THIS_MODULE,
	.get_sb = cmpefs_get_sb;
	.kill_sb = cmpefs_kill_sb;
	.next = NULL
};

struct super_block* cmpefs_sb;

int cmpefs_get_sb(struct file_system_type * fstype,
                  int flags,
                  const char* dev_name,
                  void* data,
                  struct vfsmount* mount);
{
	return get_sb_nodev(fstype, flags, data, cmpefs_sb, mount);
}

void cmpefs_kill_sb(struct super_block* sb)
{
	kill_block_super(cmpefs_sb);
}

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


