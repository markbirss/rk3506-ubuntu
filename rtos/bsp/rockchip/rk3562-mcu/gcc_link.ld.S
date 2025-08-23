#include "rtconfig.h"

MEMORY
{
    DDR   (rxw) : ORIGIN = 0x00000000, LENGTH = 512K
}


MAIN_STACK_SIZE     = 0x400;

ENTRY(Reset_Handler)

SECTIONS
{
    DDR_VECTOR_DEST = ORIGIN(DDR);
    DDR_VECTOR_SRC = DDR_VECTOR_DEST;
    .ddr.vectors DDR_VECTOR_DEST : AT (DDR_VECTOR_SRC)
    {
        . = ALIGN(4);
        KEEP(*(.vectors))
        *start_m0.o(.text .text.* .rodata .rodata*)
        . = ALIGN(4);
    }

    DDR_TEXT_SRC  = LOADADDR(.ddr.vectors) + SIZEOF(.ddr.vectors);
    DDR_TEXT_DEST = DDR_TEXT_SRC;
    .ddr.text DDR_TEXT_DEST : AT (DDR_TEXT_SRC)
    {
        KEEP(*(.init))
        KEEP(*(.fini))

        *(.text)                        /* remaining code */
        *(.text.*)                      /* remaining code */
        *(.rodata)                      /* read-only data (constants) */
        *(.rodata*)
        *(.glue_7)
        *(.glue_7t)
        *(.gnu.linkonce.t*)

        . = ALIGN(4);
        PROVIDE(__ctors_start__ = .);
        /* old GCC version uses .ctors */
        KEEP(*(SORT(.ctors.*)))
        KEEP(*(.ctors))
        /* new GCC version uses .init_array */
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        PROVIDE(__ctors_end__ = .);

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

    DDR_DATA_SRC  = LOADADDR(.ddr.text) + SIZEOF(.ddr.text);
    DDR_DATA_DEST = DDR_DATA_SRC;
    .ddr.data DDR_DATA_DEST : AT (DDR_DATA_SRC)
    {
        . = ALIGN(32);
        __data_start__ = .;
        KEEP(*(.sram.data))
        *(vtable)
        *(.data*)

        . = ALIGN(4);
        PROVIDE(__dtors_start__ = .);
        KEEP(*(SORT(.dtors.*)))
        KEEP(*(.dtors))
        PROVIDE(__dtors_end__ = .);

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

    COPY_TABLE_DEST = LOADADDR(.ddr.data) + SIZEOF(.ddr.data);
    COPY_TABLE_SRC  = COPY_TABLE_DEST;
    .copy.table COPY_TABLE_DEST : AT (COPY_TABLE_SRC)
    {
        . = ALIGN(4);
        __copy_table_start__ = .;
        __copy_table_end__ = .;
    }

    ZERO_TABLE_DEST = LOADADDR(.copy.table) + SIZEOF(.copy.table);
    ZERO_TABLE_SRC  = ZERO_TABLE_DEST;
    .zero.table ZERO_TABLE_DEST : AT (ZERO_TABLE_SRC)
    {
        . = ALIGN(4);
        __zero_table_start__ = .;
        LONG (ADDR(.ddr.bss))
        LONG (SIZEOF(.ddr.bss))
        __zero_table_end__ = .;
    }

    .ARM.exidx (ADDR(.zero.table) + SIZEOF(.zero.table)) : AT (LOADADDR(.zero.table) + SIZEOF(.zero.table))
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        /* This is used by the startup in order to initialize the .data secion */
        _sidata = .;
        __exidx_end = .;
    }

    .ARM.extab (ADDR(.ARM.exidx) + SIZEOF(.ARM.exidx)) : AT (LOADADDR(.ARM.exidx) + SIZEOF(.ARM.exidx))
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    }

    SRAM_BSS_DEST = ADDR(.ARM.extab) + SIZEOF(.ARM.extab);
    .ddr.bss SRAM_BSS_DEST (NOLOAD) :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
        . = ALIGN(4);
    }

    PROVIDE( __sram_heap_start__ = ADDR(.ddr.bss) + SIZEOF(.ddr.bss) );
    MAIN_STACK_DEST =  ORIGIN(DDR) + LENGTH(DDR) - MAIN_STACK_SIZE;
    .stack MAIN_STACK_DEST (NOLOAD) :
    {
        PROVIDE( __sram_heap_end__ = . );
        . = ALIGN(4);
        _sstack = .;
        . = . + MAIN_STACK_SIZE;
        _estack = .;
        . = ALIGN(4);
        PROVIDE(__StackTop = .);
    }
}
