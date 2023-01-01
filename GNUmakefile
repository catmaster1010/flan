.PHONY: all
all: barebones.iso

.PHONY: all-hdd
all-hdd: barebones.hdd
limine:
	git clone https://github.com/limine-bootloader/limine.git --branch=v4.x-branch-binary --depth=1
	make -C limine

.PHONY: kernel
kernel:
	make -C src

barebones.iso: limine kernel
	rm -rf iso_root
	mkdir -p iso_root
	cp src/kernel.elf \
		boot/background.bmp boot/limine.cfg limine/limine.sys limine/limine-cd.bin limine/limine-cd-efi.bin iso_root/
	xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-cd-efi.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o barebones.iso
	limine/limine-deploy barebones.iso
	rm -rf iso_root
	$(MAKE) -C src clean
.PHONY: clean
clean:
	rm -rf iso_root barebones.iso
	$(MAKE) -C src clean

.PHONY: distclean
distclean: clean
	rm -rf limine ovmf-x64
	$(MAKE) -C src distclean
.PHONY: run
run: barebones.iso
	qemu-system-x86_64 barebones.iso
.PHONY: debug
debug: barebones.iso
	qemu-system-x86_64 -no-reboot -no-shutdown -monitor stdio -m 2G -smp cores=4 --enable-kvm barebones.iso
	
