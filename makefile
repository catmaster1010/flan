override IMAGENAME := flan

QEMUFLAGS ?= -no-reboot -no-shutdown -m 4G -smp cores=4 -serial stdio -M q35
BUILDDIR :=$(PWD)/build/


.PHONY: all
all: $(IMAGENAME).iso

limine:
	git clone https://github.com/limine-bootloader/limine.git --branch=v5.x-branch-binary --depth=1
	make -C limine

export BUILDDIR
.PHONY: kernel
kernel: 
	cd src && ./get-deps.sh
	$(MAKE) -C src

initramfs:
	$(MAKE) -C usr


$(IMAGENAME).iso: limine kernel initramfs
	mkdir -p iso_root/
	cp build/kernel.elf \
		boot/limine.cfg limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin usr/initramfs.tar iso_root/
	xorriso -as mkisofs -b limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o $(IMAGENAME).iso
	limine/limine bios-install $(IMAGENAME).iso


.PHONY: clean
clean:
	rm -rf iso_root $(IMAGENAME).iso initramfs.tar
	$(MAKE) -C src clean
	$(MAKE) -C usr clean

.PHONY: run
run: $(IMAGENAME).iso
	qemu-system-x86_64 $(QEMUFLAGS) $(IMAGENAME).iso

.PHONY: kvm
kvm: $(IMAGENAME).iso
	qemu-system-x86_64 $(QEMUFLAGS) -cpu host --enable-kvm $(IMAGENAME).iso	

.PHONY: gdb
gdb: $(IMAGENAME).iso
	qemu-system-x86_64 $(QEMUFLAGS) -s -S $(IMAGENAME).iso	


