/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __copy_table_start__
 *   __copy_table_end__
 *   __zero_table_start__
 *   __zero_table_end__
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 */
ENTRY(Reset_Handler)
/*__vector_remap__ = 0;*/
firmware_magic_len = 0x20200;
_ram_fs_size = 0x10000;
_large_heap_size = 0x10000;
_system_stack_size = 0x400;
_sram_code_addr = 0x00080000;
_sram_data_addr = 0x20000000;
_flash_start_addr = 0x10000000 + firmware_magic_len;

PROVIDE(xSramDataBase  = _sram_data_addr );
PROVIDE(xSramBase  = _sram_code_addr );
PROVIDE(xSramSize  = 0x40000 );

SECTIONS
{
    _stext = _flash_start_addr;

    .vectors _flash_start_addr : AT(_flash_start_addr)
    {
        . = ALIGN(512);
        KEEP(*(.isr_vector))               /* vector table */
	. = ALIGN(512);
        *startup_rk2206.o (.text .text.*)
        . = ALIGN(4);
    }

    _stext_ext = _sram_code_addr;
    __vector_remap__ = _sram_code_addr;
    _sram_text_addr = _sram_code_addr + SIZEOF(.vectors);
    .sram.text _sram_text_addr : AT(ADDR(.vectors) + SIZEOF(.vectors))
    {
        . = ALIGN(4);
        KEEP(*(.sram.text))
        *hid.o(.rodata .rodata*)        /* read-only data (constants) */
        /* *libForceSramDrivers.a: (.text .text.* .rodata .rodata*) */
        *hal_base.o(.text .text.* .rodata .rodata*)
        *hal_snor.o(.text .text.* .rodata .rodata*)
        *hal_fspi.o(.text .text.* .rodata .rodata*)
        *drv_fspi.o(.text .text.* .rodata .rodata*)
        *drv_snor.o(.text .text.* .rodata .rodata*)
        . = ALIGN(32);
    }
    _etext_ext = .;

    _xip_text_addr = _flash_start_addr +  SIZEOF(.vectors) + SIZEOF(.sram.text);
    .xip.text _xip_text_addr : AT(_xip_text_addr)
    {
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

        /* section information for modules */
        . = ALIGN(4);
        __rtmsymtab_start = .;
        KEEP(*(RTMSymTab))
        __rtmsymtab_end = .;
        . = ALIGN(4);

        . = ALIGN(32);
    }

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    }

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    }
    __exidx_end = .;
    _sram_data_addr += SIZEOF(.vectors) + SIZEOF(.sram.text);

    .copy.table :
    {
        . = ALIGN(4);
        __copy_table_start__ = .;
        LONG (_flash_start_addr)
        LONG (_sram_code_addr)
        LONG (SIZEOF(.vectors) + SIZEOF(.sram.text))
        LONG (__etext)
        LONG (_sram_data_addr)
        LONG (SIZEOF(.data))
        __copy_table_end__ = .;
    }

    /* To clear multiple BSS sections,
     * uncomment .zero.table section and,
     * define __STARTUP_CLEAR_BSS_MULTIPLE in startup_ARMCMx.S */
    .zero.table :
    {
        . = ALIGN(4);
        __zero_table_start__ = .;
        LONG (__bss_start__)
        LONG (__bss_end__ - __bss_start__)
        __zero_table_end__ = .;
    }

    __etext = .;
    _etext = .;
    .data _sram_data_addr : AT (__etext)
    {
        __data_start__ = .;
        *(vtable)
        *(.data*)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);


        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*))
        . = ALIGN(4);
        /* All data end */
        __data_end__ = .;

    }

    .bss :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    }

    /* region for ramfs, enable if you need */
    /*.ramfs :
    {
        . = ALIGN(4);
        __ramfs_start__ = .;
        . = . + _ram_fs_size;
        __ramfs_end__ = .;
    }*/

    /* region for large memory allocate */
    .large :
    {
        . = ALIGN(8);
        __large_start__ = .;
        . = . + _large_heap_size;
	. = ALIGN(8);
        __large_end__ = .;
    }

    .stack :
    {
        . = ALIGN(8);
        _sstack = .;
        *(.stack*)
        . = . + _system_stack_size;
        _estack = .;
        . = ALIGN(8);
        PROVIDE(__StackTop = .);
    }

    __heap_start__ = .;

}
