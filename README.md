# **cat-kernel**
This is a hobby os in development. I will work on it when I get the chance to.  probably during winter break or summer break as I am very busy during school time

This can be compiled ussing a cross compiler x86_64-elf-gcc (just edit makefile) and nasm. Any VM of your choice should work. 

---
# **TODO**
- [ ] MM stuffs
	- [X] `GDT`
	- [X] `malloc()`
	- [ ] `free()`
	- [ ] `vmm.c`
- [x] IDT
- [ ] parse ACPI tables 
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
