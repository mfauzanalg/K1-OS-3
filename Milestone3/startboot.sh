#!/bin/sh
echo "Running"
nasm bootloader.asm -o bootloader
bcc -ansi -c -o kernel.o kernel.c
bcc -ansi -c -o math.o math.c
nasm -f as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel.o kernel_asm.o math.o
dd if=kernel of=system.img bs=512 conv=notrunc seek=1
bcc -ansi -c -o shell.o shell.c
nasm -f as86 lib.asm -o lib_asm.o
ld86 -o shell -d shell.o lib_asm.o
./loadFile shell FF

echo c | bochs -f if2230.config