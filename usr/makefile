.PHONY: clean

all: initramfs.tar

initramfs.tar: init
	tar -cf initramfs.tar build

init: test.c
	mkdir -p build 
	$(CC) -ffreestanding -masm=intel -c $< -o test.o
	$(LD) test.o -nostdlib -static  -z max-page-size=0x1000 -T linker.ld -o build/init
	rm test.o
clean: 
	rm -rf build
	rm initramfs.tar
