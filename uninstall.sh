#!/bin/bash

sudo rmmod cmpefs_kernelmod.ko
cat /proc/modules | grep -q "cmpe"
if [ $? -eq 0 ]; then
	echo "Uninstall failed :("
fi
cat /proc/filesystems | grep -q "cmpe"
if [ $? -eq 0 ]; then
	echo "Uninstall failed :("
fi
echo "Uninstall successful!"
