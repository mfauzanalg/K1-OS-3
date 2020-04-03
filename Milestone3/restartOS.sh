echo "Rebuilding"
nasm bootloader.asm -o bootloader

dd if=bootloader of=system.img bs=512 count=1 conv=notrunc

dd if=map.img of=system.img bs=512 count=1 seek=256 conv=notrunc

dd if=dir.img of=system.img bs=512 count=2 seek=257 conv=notrunc

dd if=sectors.img of=system.img bs=512 count=1 seek=259 conv=notrunc

bcc -ansi -c -o kernel.o kernel.c
bcc -ansi -c -o math.o math.c

nasm -f as86 kernel.asm -o kernel_asm.o

ld86 -o kernel -d kernel.o kernel_asm.o math.o

dd if=kernel of=system.img bs=512 conv=notrunc seek=1