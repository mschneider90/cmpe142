#!/bin/bash

sudo insmod cmpefs_kernelmod.ko
cat /proc/modules | grep -q "cmpe"
if [ $? -ne 0 ]; then
	echo "Installation failed :("
	exit
fi
cat /proc/filesystems | grep -q "cmpe"
if [ $? -ne 0 ]; then
	echo "Installation failed :("
	exit
fi
echo "Installation successful!"
