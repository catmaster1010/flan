section	.text
	    global load_idt
load_idt:
        cli
        MOV   [idtr], DI
        MOV  [idtr+2], RSI
        LIDT  [idtr] ;Load idt into cpu.
        ret



section	.data
idtr DW 0
DQ 0 
