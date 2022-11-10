section	.text
	    global load_idt
load_idt:
        cli
        MOV   [idtr], DI
        MOV  [idtr+2], RSI
        LIDT  [idtr] ;Load IDT into cpu.
        sti ; Enable interrupts again.
        ret



section	.data
idtr DW 0
DQ 0 
