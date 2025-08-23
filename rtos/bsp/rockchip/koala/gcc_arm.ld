OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

_heap_size = DEFINED(__heap_size__) ? __heap_size__ : 128K;
_stack_size = DEFINED(__stack_size__) ? __stack_size__ : 16K;
/*_ram_fs_size = 64K;*/
MEMORY { .sram : ORIGIN = 0x20000000, LENGTH = 512K }
MEMORY { .dtcm : ORIGIN = 0x30240000, LENGTH = 256K }

SECTIONS
{
    . = 0x20000000;
    _stext = .;
    _stext_ext = .;
    .text :
    {
        *(.isr_vector)
        *(.text)                        /* remaining code */
        *(.text.*)                      /* remaining code */

        *(.rodata)                      /* read-only data (constants) */
        *(.rodata*)
        *(.glue_7)
        *(.glue_7t)
        *(.gnu.linkonce.t*)

        /* section information for finsh shell */
        . = ALIGN(4);
        __fsymtab_start = .;
        KEEP(*(FSymTab))
        __fsymtab_end = .;
        . = ALIGN(4);
        __vsymtab_start = .;
        KEEP(*(VSymTab))
        __vsymtab_end = .;
        . = ALIGN(4);

        /* section information for initial. */
        . = ALIGN(4);
        __rt_init_start = .;
        KEEP(*(SORT(.rti_fn*)))
        __rt_init_end = .;
        . = ALIGN(4);

        . = ALIGN(4);
        _etext = .;
        _etext_ext = .;
    } >.sram

    . = ALIGN(4);
    .data :
    {
        *(.data)
        *(.data.*)

        *(.data1)
        *(.data1.*)

        . = ALIGN(8);
        _gp = ABSOLUTE(.);     /* Base of small data */

        *(.sdata)
        *(.sdata.*)
    } >.sram

    . = ALIGN(4);
    .bss (NOLOAD) :
    {
        PROVIDE(__bss_start = .);
        *(.bss)
        *(.bss.*)
        *(.dynbss)
	. = ALIGN(4);
        PROVIDE(__bss_end = .);
    } >.dtcm
    _end = .;

    .heap :
    {
        . = ALIGN(8);
        PROVIDE(__heap_start = .);
	. += _heap_size;
	. = ALIGN(8);
	PROVIDE(__heap_end = .);
	__heap_limit = .; /* Add for _sbrk */
    } >.dtcm

    .stack :
    {
       . = ALIGN(8);
       _sstack = .;
       . += _stack_size;
       . = ALIGN(8);
       _estack = .;
       PROVIDE(__stack_start = .);
    } >.sram

    /* PROVIDE(__ramfs_start__ = .); */
    /* PROVIDE(__ramfs_end__ = . + _ram_fs_size); */

    /* Stabs debugging sections.  */
    .stab          0 (NOLOAD) : { *(.stab) }
    .stabstr       0 (NOLOAD) : { *(.stabstr) }
    .stab.excl     0 (NOLOAD) : { *(.stab.excl) }
    .stab.exclstr  0 (NOLOAD) : { *(.stab.exclstr) }
    .stab.index    0 (NOLOAD) : { *(.stab.index) }
    .stab.indexstr 0 (NOLOAD) : { *(.stab.indexstr) }
    .comment       0 (NOLOAD) : { *(.comment) }
}
