section	.text

	global func

func:
        cli
           MOV   [gdtr], DI
MOV   [gdtr+2], RSI
   LGDT  [gdtr]

        ret

section	.data
gdtr DW 0
DQ 0 
