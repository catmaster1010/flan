OUTPUT_FORMAT(elf64-x86-64)

ENTRY(_start)


PHDRS
{
    text    PT_LOAD    FLAGS((1 << 0) | (1 << 2)) ; /* Execute + Read */
    rodata  PT_LOAD    FLAGS((1 << 2)) ;            /* Read only */
    data    PT_LOAD    FLAGS((1 << 1) | (1 << 2)) ; /* Write + Read */
}

SECTIONS
{
    . = 0xffffffff80000000;

   
    .text : {
        *(.text .text.*)
    } :text

    . += CONSTANT(MAXPAGESIZE);


    PROVIDE(symbol_table = 1);

    .rodata : {
        *(.rodata .rodata.*)
    } :rodata

    . += CONSTANT(MAXPAGESIZE);

    .data : {
        *(.data .data.*)
    } :data

    .bss : {
        *(COMMON)
        *(.bss .bss.*)
    } :data

    kernel_end_addr = .;

    /DISCARD/ : {
        *(.eh_frame)
        *(.note .note.*)
    }
}