ENTRY(_start)

SECTIONS 
{
	PROVIDE(payload_start = 0x40003000);
	. = payload_start;
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
	payload_end = .;
	.bss : 
	{
		__bss_start = .;
		*(COMMON)
		*(.bss*)
		__bss_end = .;
	}
}
