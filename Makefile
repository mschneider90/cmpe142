obj-m += cmpefs_kernelmod.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
