run:
	@qemu-system-i386 -hda hd.img -serial null -parallel stdio 
debug:
	@qemu-system-i386 -s -S -hda hd.img -serial null -parallel stdio &
	@sleep 1
	@gnome-terminal -e "gdb -q -x gdbinit"
build:
	@nasm -g -f elf32 mbr.asm -o mbr.o
	@ld -o mbr.symbol -melf_i386 -N mbr.o -Ttext 0x7c00
	@ld -o mbr.bin -melf_i386 -N mbr.o -Ttext 0x7c00 --oformat binary
	@nasm -g -f elf32 bootloader.asm -o bootloader.o
	@gcc -g -Wall -march=i386 -m32 -nostdlib -fno-builtin -ffreestanding -fno-pic  -c read.c
	@ld -o boot.o -melf_i386 -N bootloader.o read.o -e start_tag -Ttext 0x00007e00
	@ld -o boot.bin -melf_i386 -N bootloader.o read.o -e start_tag -Ttext 0x00007e00 --oformat binary
	@nasm -o kernel.o -g -f elf32 kernel.asm 
	@ld -o kernel.symbol -melf_i386 -N kernel.o -Ttext 0x20000
	@ld -o kernel.bin -melf_i386 -N kernel.o -Ttext 0x20000 --oformat binary
	@dd if=mbr.bin of=hd.img bs=512 count=1 seek=0 conv=notrunc
	@dd if=boot.bin of=hd.img bs=512 count=5 seek=1 conv=notrunc
	@dd if=kernel.bin of=hd.img bs=512 count=2 seek=6 conv=notrunc
clean:
	@rm -fr *.bin *.o
