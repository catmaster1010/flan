# **flan-os**
This is a hobby os in development. 

This supports the limine boot protocal and uses limine v5.x

This can be compiled using a cross compiler x86_64-elf-gcc and nasm. Any VM of your choice should work. 

---
# **TODO**
- [X] GDT
- [x] PMM
- [x] VMM
- [x] KHEAP
- [x] IDT
- [x] Parse ACPI tables 
	-  [x] MADT
- [x] Start up other CPUs
- [x] Set up APIC
- [x] Configure the PIT
- [x] Calibrate the APIC timer
- [x] Implement scheduler 
- [x] Implement VFS
- [x] Elf loader
- [ ] Syscalls
- [ ] EXT2
---
# Dependencies
- `x86_64-elf-gcc` 
- `nasm`
- `git`
- `xorriso`
- `make`

To build the iso, just run `make`.
