/* OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64", "elf64-x86-64") */
/* OUTPUT_ARCH(i386:x86-64) */
ENTRY(_start)

sv_start = 0xFFFFC00000000000;
sv_last  = 0xFFFFC07FFFFFFFFF;

SECTIONS
{
	/* . = 0x100000; */
	. = sv_start ;  
	_begin = . ;

	.bootstrap :
	{
		*(.bootstrap)
	}

	. = ALIGN(0x1000);

        /* Code */
	_stext = . ;

	.text :
	AT (ADDR(.text))
	{
		*(.text)
		*(.text.*)
		*(.stub)
		*(.note*)
	}
	_etext = . ;

  . = ALIGN(0x1000);
	.rodata :
	/* AT (LOADADDR(.text) + (ADDR(.rodata) - ADDR(.text))) */
	AT (ADDR(.rodata))
	{
		*(.rodata)
		*(.rodata.*)
	}

    _erodata = .;

        .eh_frame_hdr ALIGN(4K) : {
        *(.eh_frame_hdr)
        }

        .eh_frame ALIGN(4) : ONLY_IF_RO {
        __eh_frame_start = .;
        *(.eh_frame)
        QUAD(0);
        }

. = ALIGN(0x1000);
	.initfini :
	AT (LOADADDR(.text) + (ADDR(.initfini) - ADDR(.text)))
	{
		__init_array_start = . ;
		*(SORT_BY_INIT_PRIORITY(.init_array.*))
		*(SORT_BY_INIT_PRIORITY(.ctors*))
		*(.init_array)
		__init_array_end = . ;
		__fini_array_start = . ;
		*(SORT_BY_INIT_PRIORITY(.fini_array.*))
		*(SORT_BY_INIT_PRIORITY(.dtors*))
		*(.fini_array)
		__fini_array_end = . ;
	}

	. = ALIGN(0x1000);

	_data_start = .;
	.data :
	AT (LOADADDR(.text) + (ADDR(.data) - ADDR(.text)))
	{
		*(.data)
		*(.data.*)
	}
	. = ALIGN(0x1000);
	.tdata : {
		_tdata_start = . ;
		*(.tdata)
		_tdata_end = . ;
	}
	_edata = . ;
	. = ALIGN(0x1000);
	.tbss : {
		_tbss_start = . ;
		*(.tbss)
		_tbss_end = . ;
	}

.got BLOCK(4K) : ALIGN(4K)
{
*(.got)
}

  .got.plt BLOCK(4k) : ALIGN(4K)
  {
*(.got.plt.*)
}

. = ALIGN(0x1000);
	__bss_start = . ;
	.bss :
	AT (LOADADDR(.text) + (ADDR(.bss) - ADDR(.text)))
	{
		*(.bss)
		*(.bss.*)
		*(COMMON)
	}
	. = ALIGN(0x1000);
	.lbss :
	AT (LOADADDR(.text) + (ADDR(.lbss) - ADDR(.text)))
	{
		*(.lbss)
		*(.lbss.*)
		*(LARGE_COMMON)
	}
	_ebss = . ;
	_end = . ;
}
