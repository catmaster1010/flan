QEMUFLAGS ?= -no-reboot -no-shutdown -m 4G -smp cores=4 -serial stdio 

.PHONY: all
all: barebones.iso

limine:
	git clone https://github.com/limine-bootloader/limine.git --branch=v5.x-branch-binary --depth=1
	make -C limine

.PHONY: kernel
kernel: 
	cd src && ./get-deps.sh
	$(MAKE) -C src

initramfs:
	$(MAKE) -C usr


barebones.iso: limine kernel initramfs
	rm -rf iso_root
	mkdir -p iso_root/
	cp src/kernel.elf \
		boot/limine.cfg limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin usr/initramfs.tar iso_root/
	xorriso -as mkisofs -b limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o barebones.iso
	limine/limine bios-install barebones.iso

	$(MAKE) -C src clean

.PHONY: clean
clean:
	rm -rf iso_root barebones.iso initramfs.tar
	$(MAKE) -C src clean
	$(MAKE) -C usr clean

.PHONY: run
run: barebones.iso
	qemu-system-x86_64 $(QEMUFLAGS) barebones.iso

.PHONY: kvm
kvm: barebones.iso
	qemu-system-x86_64 $(QEMUFLAGS) -cpu host --enable-kvm barebones.iso	

.PHONY: gdb
gdb: barebones.iso
	qemu-system-x86_64 $(QEMUFLAGS) -s -S barebones.iso	


