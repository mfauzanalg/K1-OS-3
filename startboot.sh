#!/bin/sh
echo "running...."
bcc -ansi -c -o kernel.o kernel.c
nasm -f as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel.o kernel_asm.o
dd if=kernel of=system.img bs=512 conv=notrunc seek=3

gcc loadFile.c -o loadFile
./loadFile milestone1	

echo c | bochs -f if2230.config