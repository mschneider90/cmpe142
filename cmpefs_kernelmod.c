//CMPE 142 file system (get rid of these? put into a bibliography?)
//Sources:
// > documentation/filesystem/vfs.txt
// > documentation/kbuild/modules.txt
// > documentation/kbuild/makefiles.txt
// > https://casper.berkeley.edu/svn/trunk/roach/sw/linux/security/inode.c
// > http://www.win.tue.nl/~aeb/linux/lk/lk-8.html
// > http://www.tldp.org/LDP/khg/HyperNews/get/fs/vfstour.html
// > http://lwn.net/Articles/57369/
/*INCLUDES*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cred.h>
#include <linux/init.h>
#include <linux/pagemap.h> 
#define CMPEFS_MAGIC 0x06159881
#define TEMP_SIZE 100
/*START PROTOTYPES*/
extern int register_filesystem(struct file_system_type*);
extern int unregister_filesystem(struct file_system_type*);
int cmpefs_get_sb(struct file_system_type*, int, const char*, void*, struct vfsmount*);
void cmpefs_kill_sb(struct super_block*);
int cmpefs_fill_sb(struct super_block*, void*, int);
static int __init cmpefs_init(void);
static void __exit cmpefs_exit(void);
static const struct inode_operations iops;
static const struct file_operations fops;
/*END PROTOTYPES*/
/*GET SUPERBLOCK*/
int cmpefs_get_sb(struct file_system_type * fstype, int flags, const char* dev_name, void* data, struct vfsmount* mount)
{
	return get_sb_nodev(fstype, flags, data, cmpefs_fill_sb, mount);
}
/*GET INODE*/
struct inode *cmpefs_get_inode(struct super_block *sb, const struct inode *dir, umode_t mode, dev_t dev)
{
	struct inode *node = new_inode(sb);
	if (node) {
	node->i_mode = mode;			//format of file
	node->i_uid = 0;				//user 0 has access 
	node->i_gid = 0;				//group 0 has access
	node->i_blocks = 10;			
	node->i_atime = CURRENT_TIME;	//accessed time: current time
	node->i_mtime = CURRENT_TIME;	//modified time: current time
	node->i_ctime = CURRENT_TIME;	//creation time: current time
	if ((mode & S_IFMT) == S_IFREG)	//if file
	{
		node->i_op = &iops;	//use inode operations
		node->i_fop = &fops;	//use file operations
	}
	else if ((mode & S_IFMT) == S_IFDIR)	//if directory
	{
		node->i_op = &iops;	//use inode operations
		node->i_fop = &simple_dir_operations;	//use simple directory operations
		inc_nlink(node);
	}
	else init_special_inode(node, mode, dev);
	}
	return node;
}
/*MAKE INODE*/
static int cmpefs_mknod(struct inode *dir, struct dentry *dentry, umode_t mode, dev_t dev)
{
	struct inode * inode = cmpefs_get_inode(dir->i_sb, dir, mode, dev);	
	int error = -ENOSPC;
	if (inode) {		//if inode exists
		d_instantiate(dentry, inode);	//fill in information for dentry
		dget(dentry);	
		error = 0;
		dir->i_mtime = CURRENT_TIME;	//modified time: current time
		dir->i_ctime = CURRENT_TIME;	//created time: current time
		}
	return error;
}
/*FS TYPE INFORMATION*/
struct file_system_type cmpefs_fstype = {
	.name = "cmpefs",
	.fs_flags = 0,
	.get_sb = cmpefs_get_sb,
	.kill_sb = cmpefs_kill_sb,
	.owner = THIS_MODULE,
	.next = NULL
};
/*SUPERBLOCK OPERATIONS*/
struct super_operations sop =
{
	.drop_inode = generic_delete_inode,
	.statfs = simple_statfs,
};

/*OPEN FILE*/
static int cmpefs_open(struct inode *inode, struct file *fp)
{
	//fp->private_data = inode->i_private;
	fp->private_data = (char)'a';
	return 0;
}

/*READ FILE*/
static ssize_t cmpefs_read(struct file* fp, char* buf, size_t count, loff_t* offset)
{
	char temp_buf[TEMP_SIZE];
	int length = 3;
	temp_buf[0] = (char)fp->private_data;
	temp_buf[1] = '\n';
	temp_buf[2] = '\0';
	if (*offset > length)
		return 0;
	if (count > length - *offset)
		count = length - *offset;
	if (copy_to_user(buf, temp_buf + *offset, count))
		return -1;
	*offset += count;
	return count;
}

/*WRITE FILE*/
static ssize_t cmpefs_write(struct file* fp, const char* buf, size_t count, loff_t* offset)
{
	char temp_buf[TEMP_SIZE];
	copy_from_user(temp_buf, buf, count);
	fp->private_data = temp_buf[0];
	return count;
}

/*FILE OPERATIONS*/
static const struct file_operations fops = { .open = cmpefs_open,
				             .write = cmpefs_write,
					     .read = cmpefs_read
};
/*FILL SUPERBLOCK*/
int cmpefs_fill_sb(struct super_block* sb, void* data, int silent)	
{
	struct inode *root;	//root
	sb->s_blocksize = 1024;	
	sb->s_blocksize_bits = 8;
	sb->s_magic = CMPEFS_MAGIC;
	sb->s_op = &sop;
	//root dir inode config
	root = cmpefs_get_inode (sb, NULL, S_IFDIR | 0755, 0);
	if (!root) return -ENOMEM;
	root->i_op = &iops;				//use inode ops
	root->i_fop = &simple_dir_operations;	//use simple dir ops
	sb->s_root = d_alloc_root(root);		//root dentry
	return 0;
}
/*CREATE FILE (touch)*/
static int cmpefs_create(struct inode *dir, struct dentry *dentry, umode_t mode) {return cmpefs_mknod(dir, dentry, mode | S_IFREG, 0);}
/*CREATE DIRECTORY (mkdir)*/
static int cmpefs_mkdir(struct inode *dir, struct dentry *dentry, umode_t mode)
{
	int node;
	mode = (mode & (S_IRWXUGO | S_ISVTX)) | S_IFDIR;
	node = cmpefs_mknod(dir, dentry, mode, 0);
	if (!node)
		inc_nlink(dir);
	return node;
}

/*INODE OPERATIONS*/
static const struct inode_operations iops = {
		.create		= cmpefs_create,
		.lookup		= simple_lookup,
		.link		= simple_link,
		.unlink		= simple_unlink,
		.mkdir		= cmpefs_mkdir,
		.rmdir		= simple_rmdir,
		.mknod		= cmpefs_mknod,
		.rename		= simple_rename,};
/*KILL SUPERBLOCK*/
void cmpefs_kill_sb(struct super_block* sb){kill_block_super(sb);}
/*INITIALIZE FS*/
static int __init cmpefs_init(){return register_filesystem(&cmpefs_fstype);}
/*EXIT FS*/
static void __exit cmpefs_exit(){unregister_filesystem(&cmpefs_fstype);}
/*INITS*/
module_init(cmpefs_init);
module_exit(cmpefs_exit);
