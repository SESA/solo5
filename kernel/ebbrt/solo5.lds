ENTRY(_start)
sv_start = 0xFFFFC00000000000;
sv_last  = 0xFFFFC07FFFFFFFFF;

SECTIONS {
	  . = sv_start ;  
    .reg :
	  {	
	      QUAD(0); /* r0 */
	      QUAD(0); /* r1 */
	      QUAD(0); /* r2 */
	      QUAD(0); /* r3 */
	      QUAD(0); /* r4 */
	      QUAD(0); /* r5 */
	      QUAD(0); /* r6 */
	      QUAD(0); /* r7 */
	      QUAD(0); /* r8 */
	      QUAD(0); /* r9 */
	      QUAD(0); /* r10 */
	      QUAD(0); /* r11 */
	      QUAD(0); /* r12 */
	      QUAD(0); /* r13 */
	      QUAD(0); /* r14 */
	      QUAD(0); /* r15 */
        }	

    /* Code */
    . = ALIGN(0x1000);
    _stext = .;

    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.text)
        *(.text.*)
    }

    . = ALIGN(0x1000);
    _etext = .;

    /* Read-only data */
	  .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
        *(.rodata.*)
    }
    .eh_frame_hdr ALIGN(4K) : {
        *(.eh_frame_hdr)
    }
    .eh_frame ALIGN(4) : ONLY_IF_RO {
        __eh_frame_start = .;
        *(.eh_frame)
        QUAD(0);
    }

    . = ALIGN(0x1000);
    _erodata = .;

    /* Read-write data (initialized) */
    .got BLOCK(4K) : ALIGN(4K)
    {
        *(.got.plt)
        *(.got)
    }

    /* TODO(jmcadden): check if section is required */
    .got.plt BLOCK(4k) : ALIGN(4K)
    {
      *(.got.plt.*)
    }
	  .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
        *(.data.*)
    }

    . = ALIGN(0x1000);
    _edata = .;

    /* Read-write data (uninitialized) */
	  .bss BLOCK(4K) : ALIGN(4K)
    {
        *(.bss)
        *(COMMON)
    }

    . = ALIGN(0x1000);
    _ebss = .;
    _end = .;
}
