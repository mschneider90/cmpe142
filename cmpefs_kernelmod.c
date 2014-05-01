#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>

extern int register_filesystem(struct file_system_type*);
extern int unregister_filesystem(struct file_system_type*);

int cmpefs_get_sb(struct file_system_type*, int, const char*, void*, struct vfsmount*);
void cmpefs_kill_sb(struct super_block*);
int cmpefs_fill_sb(struct super_block*, void*, int);

struct file_system_type cmpefs_fstype = {
	.name = "cmpefs",
	.owner = THIS_MODULE,
	.get_sb = cmpefs_get_sb,
	.kill_sb = cmpefs_kill_sb,
	.next = NULL
};

struct super_operations cmpefs_sop;

int cmpefs_fill_sb(struct super_block* sb, void* data, int silent)
{
	sb->s_blocksize = 1024;
	sb->s_blocksize_bits = 8;
	sb->s_magic = 0x534A;
	sb->s_op = &cmpefs_sop;
	return 0; //no error
}

int cmpefs_get_sb(struct file_system_type * fstype,
                  int flags,
                  const char* dev_name,
                  void* data,
                  struct vfsmount* mount)
{
	return get_sb_nodev(fstype, flags, data, cmpefs_fill_sb, mount);
}

void cmpefs_kill_sb(struct super_block* sb)
{
	kill_block_super(sb);
}

static int __init cmpefs_init()
{
	return register_filesystem(&cmpefs_fstype);
}

static void __exit cmpefs_exit()
{
	unregister_filesystem(&cmpefs_fstype);
}

module_init(cmpefs_init);
module_exit(cmpefs_exit);


