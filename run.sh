#!/bin/sh

make && qemu-system-i386 -fda ./bin/floppy.img
