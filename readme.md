Compile with:

make all

This should give you a file: cmpefs_kernelmodule.ko. To put this into the kernel, use the command (as root):

insmod cmpefs_kernelmodule.ko

To verify that it's in the kernel, use the command:

cat /proc/modules

To verify that the file system has been registered with VFS, use the command:

cat /proc/filesystems
