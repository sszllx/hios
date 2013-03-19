all:
	nasm -o loader.bin loader.asm
	cp loader.bin ../
	nasm -f elf -o kernel.o kernel.asm
	nasm -f elf -o mem.o mem.asm
	nasm -f elf -o io_api.o io_api.asm
	gcc -fno-stack-protector -m32 -c -o bootpack.o bootpack.c
	gcc -fno-stack-protector -m32 -c -o dsctbl.o dsctbl.c
	gcc -fno-stack-protector -m32 -c -o fifo.o fifo.c
	gcc -fno-stack-protector -m32 -c -o graphic.o graphic.c
	gcc -fno-stack-protector -m32 -c -o int.o int.c
	gcc -fno-stack-protector -m32 -c -o keyboard.o keyboard.c
	gcc -fno-stack-protector -m32 -c -o memory.o memory.c
	gcc -fno-stack-protector -m32 -c -o mouse.o mouse.c
	gcc -fno-stack-protector -m32 -c -o sheet.o sheet.c
	gcc -fno-stack-protector -m32 -c -o timer.o timer.c
# -L/usr/lib/i386-linux-gnu -lglib-2.0
# 0x00280000
	ld -m elf_i386 -s -Ttext 0xC0000000 -o kernel.bin kernel.o mem.o io_api.o bootpack.o dsctbl.o fifo.o graphic.o int.o keyboard.o memory.o mouse.o sheet.o timer.o 
	cp kernel.bin ../
clean:
	rm loader.bin *~ *.o kernel.bin