#!/bin/bash

sudo insmod cmpefs_kernelmod.ko
cat /proc/modules | grep -q "cmpe"
if [ $? -ne 0 ]; then
	echo "Installation failed :("
fi
cat /proc/filesystems | grep -q "cmpe"
if [ $? -ne 0 ]; then
	echo "Installation failed :("
fi
echo "Installation successful!"
