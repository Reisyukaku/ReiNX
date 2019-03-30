ENTRY(_start)

SECTIONS
{
	PROVIDE(__payload_start = 0x40008000);
    PROVIDE(__heap_start = 0x90020000);
    PROVIDE(__stack_start = 0x90010000);

	. = __payload_start;
	.text :
	{
		*(.text.start);
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
