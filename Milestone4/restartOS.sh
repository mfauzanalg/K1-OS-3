echo "Rebuilding"
nasm bootloader.asm -o bootloader

dd if=bootloader of=system.img bs=512 count=1 conv=notrunc

dd if=map.img of=system.img bs=512 count=1 seek=256 conv=notrunc

dd if=dir.img of=system.img bs=512 count=2 seek=257 conv=notrunc

dd if=sectors.img of=system.img bs=512 count=1 seek=259 conv=notrunc

bcc -ansi -c -o kernel.o kernel.c
bcc -ansi -c -o lib/math.o lib/math.c
bcc -ansi -c -o lib/bootlogo.o lib/bootlogo.c
bcc -ansi -c -o lib/teks.o lib/teks.c
bcc -ansi -c -o lib/folderIO.o lib/folderIO.c
bcc -ansi -c -o lib/fileIO.o lib/fileIO.c
bcc -ansi -c -o shell.o shell.c

nasm -f as86 kernel.asm -o kernel_asm.o

ld86 -o kernel -d kernel.o kernel_asm.o lib/math.o lib/bootlogo.o lib/teks.o lib/fileIO.o lib/folderIO.o
ld86 -o shell -d shell.o lib_asm.o lib/fileIO.o lib/folderIO.o lib/math.o

dd if=kernel of=system.img bs=512 conv=notrunc seek=1