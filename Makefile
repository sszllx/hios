all:
	make -C kernel
	nasm -o boot.bin Boot.asm
	dd if=boot.bin of=os.ima bs=512 count=1 conv=notrunc
	sudo mount -o loop os.ima mnt
	sudo cp loader.bin mnt
	sudo cp kernel.bin mnt
	sudo umount mnt

clean:
	rm boot.bin *~