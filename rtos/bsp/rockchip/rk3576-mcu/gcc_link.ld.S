#include "rtconfig.h"

MEMORY
{
    DDR   (rxw) : ORIGIN = 0x00000000, LENGTH = 512K
    LINUX_SHARE_MEM (rxw) : ORIGIN = 0x20000000, LENGTH = 0x2000000 /* shared memory */
}


MAIN_STACK_SIZE     = 0x400;

ENTRY(Reset_Handler)

SECTIONS
{
    .vectors :
    {
        . = ALIGN(4);
        KEEP(*(.vectors))
        *start_m0.o(.text .text.* .rodata .rodata*)
        . = ALIGN(4);
    } > DDR

    .text :
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
    } > DDR

    .data :
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
    } > DDR

    .copy.table :
    {
        . = ALIGN(4);
        __copy_table_start__ = .;
        __copy_table_end__ = .;
    } > DDR

    .zero.table :
    {
        . = ALIGN(4);
        __zero_table_start__ = .;
        LONG (ADDR(.bss))
        LONG (SIZEOF(.bss))
        __zero_table_end__ = .;
    } > DDR

    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        /* This is used by the startup in order to initialize the .data secion */
        _sidata = .;
        __exidx_end = .;
    } > DDR

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > DDR

    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
        . = ALIGN(4);
    } > DDR

    .heap :
    {
        . = ALIGN(4);
        PROVIDE( __sram_heap_start__ = .);
        . = ORIGIN(DDR) + LENGTH(DDR) - MAIN_STACK_SIZE; 
        PROVIDE( __sram_heap_end__ = . );
        . = ALIGN(4);
    } > DDR

    .stack (NOLOAD) :
    {
        . = ALIGN(4);
        _sstack = .;
        . = . + MAIN_STACK_SIZE;
        _estack = .;
        . = ALIGN(4);
        PROVIDE(__StackTop = .);
    } > DDR

    .linux_share_mem(NOLOAD):
    {
        PROVIDE(__linux_share_mem_start__ = .);
        . += LENGTH(LINUX_SHARE_MEM);
        PROVIDE(__linux_share_mem_end__ = .);
    } > LINUX_SHARE_MEM
}
