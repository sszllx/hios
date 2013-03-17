all:
	nasm -o loader.bin loader.asm
	cp loader.bin ../
	nasm -f elf -o kernel.o kernel.asm
	nasm -f elf -o mem.o mem.asm
	nasm -f elf -o io_api.o io_api.asm
	gcc -fno-stack-protector -m32 -c -o bootpack.o bootpack.c
	gcc -fno-stack-protector -m32 -c -o fifo.o fifo.c
	gcc -fno-stack-protector -m32 -c -o int.o int.c
	gcc -fno-stack-protector -m32 -c -o timer.o timer.c
	gcc -fno-stack-protector -m32 -c -o mouse.o mouse.c
	gcc -fno-stack-protector -m32 -c -o keyboard.o keyboard.c
# -L/usr/lib/i386-linux-gnu -lglib-2.0
	ld -m elf_i386 -s -Ttext 0xC0000000 -o kernel.bin kernel.o mem.o bootpack.o fifo.o int.o timer.o mouse.o keyboard.o io_api.o
	cp kernel.bin ../
clean:
	rm loader.bin *~ *.o kernel.bin