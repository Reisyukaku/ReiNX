ENTRY(_start)

SECTIONS 
{
	PROVIDE(__payload_start = 0x40003000);
    PROVIDE(__heap_start = 0x90020000);
    
	. = __payload_start;
	.text.start : 
	{ 
		*(.text.start)
	}
	.text : 
	{
		*(.text*);
	}
	.data : 
	{
		*(.data*);
		*(.rodata*);
	}
	. = ALIGN(0x10);
	__payload_end = .;
	.bss : 
	{
		__bss_start = .;
		*(COMMON)
		*(.bss*)
		__bss_end = .;
	}
}
