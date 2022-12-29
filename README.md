# **cat-os**
This is a hobby os in development. 

This supports the limine boot protocall and uses limine v4.2

This can be compiled ussing a cross compiler x86_64-elf-gcc and nasm. Any VM of your choice should work. 

---
# **TODO**
- [x] MM stuffs
	- [X] `GDT`
	- [x] `PMM`
		- [X] `malloc()`
		- [x] `free()`
	- [x] `VMM`
	- [x] `KHEAP`
- [x] IDT
- [x] Parse ACPI tables 
	-  [x] MADT
- [ ] Start up other CPUs
- [ ] Set up APIC
- [ ] configure timer
- [ ] Implment scheduler 
- [ ] implement VFS
---
# Dependencies
- `x86_64-elf-gcc` (or you can use your host system's toolchain if compatible) 
- `nasm`
- `git`
- `xorriso`
- `make`

To compile, just run `make`.
