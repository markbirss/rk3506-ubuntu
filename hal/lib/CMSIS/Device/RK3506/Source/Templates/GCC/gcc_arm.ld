/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2023 Rockchip Electronics Co., Ltd.
 */

/* Program Entry, set to mark it as "used" and avoid gc */
MEMORY
{
    SRAM  (rxw) : ORIGIN = 0xff6c0000, LENGTH = 256K   /* system sram 256k */
    DRAM  (rxw) : ORIGIN = 0x00200000, LENGTH = 1M     /* DRAM */
    TTB   (rw)  : ORIGIN = 0x00300000, LENGTH = 16K    /* MMU table */
}

ENTRY(Reset_Handler)

__STACK_SIZE = 0x400;
__FIQ_STACK_SIZE = 0x400;
__IRQ_STACK_SIZE = 0x400;
__SVC_STACK_SIZE = 0x400;
__ABT_STACK_SIZE = 0x400;
__UND_STACK_SIZE = 0x400;

SECTIONS
{
    .text :
    {

        Image$$VECTORS$$Base = .;
        * (RESET)
        KEEP(*(.isr_vector))
        Image$$VECTORS$$Limit = .;

        *(SVC_TABLE)
        *(.text*)

        KEEP(*(.init))
        KEEP(*(.fini))

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

        Image$$RO_DATA$$Base = .;
        *(.rodata*)
        Image$$RO_DATA$$Limit = .;

        KEEP(*(.eh_frame*))
    } > DRAM

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > DRAM

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > DRAM
    __exidx_end = .;

    .zero.table :
    {
        . = ALIGN(4);
        __zero_table_start__ = .;
        LONG (__bss_start__)
        LONG (__bss_end__ - __bss_start__)
        __zero_table_end__ = .;
    } > DRAM

    __etext = .;


    .data :
    {
        Image$$RW_DATA$$Base = .;
        __data_start__ = .;
        *(vtable)
        *(.data*)
        Image$$RW_DATA$$Limit = .;

        . = ALIGN(4);
        /* preinit data */
        PROVIDE (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE (__init_array_end = .);


        . = ALIGN(4);
        /* finit data */
        PROVIDE (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE (__fini_array_end = .);

        . = ALIGN(4);
        /* All data end */
        __data_end__ = .;

    } > DRAM

    .bss ALIGN(0x400):
    {
        Image$$ZI_DATA$$Base = .;
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
        Image$$ZI_DATA$$Limit = .;
        __end__ = .;
        end = __end__;
    } > DRAM


    .stack (NOLOAD):
    {
        . = ALIGN(8);

        __StackTop = .;
        Image$$SYS_STACK$$ZI$$Base = .;
        . += __STACK_SIZE;
        Image$$SYS_STACK$$ZI$$Limit = .;
        __stack = .;

        Image$$FIQ_STACK$$ZI$$Base = .;
        . += __FIQ_STACK_SIZE;
        Image$$FIQ_STACK$$ZI$$Limit = .;

        Image$$IRQ_STACK$$ZI$$Base = .;
        . += __IRQ_STACK_SIZE;
        Image$$IRQ_STACK$$ZI$$Limit = .;

        Image$$SVC_STACK$$ZI$$Base = .;
        . += __SVC_STACK_SIZE;
        Image$$SVC_STACK$$ZI$$Limit = .;

        Image$$ABT_STACK$$ZI$$Base = .;
        . += __ABT_STACK_SIZE;
        Image$$ABT_STACK$$ZI$$Limit = .;

        Image$$UND_STACK$$ZI$$Base = .;
        . += __UND_STACK_SIZE;
        Image$$UND_STACK$$ZI$$Limit = .;

    } > DRAM

    .heap :
    {
        . = ALIGN(16);
        PROVIDE(__heap_start = .);
        . = ORIGIN(DRAM) + LENGTH(DRAM);
        PROVIDE(__heap_end = .);
        end = __heap_end;
    } > DRAM

    .ttb (NOLOAD):
    {
        Image$$TTB$$ZI$$Base = .;
        PROVIDE(MMUTable = .);
        . += ORIGIN(TTB) + LENGTH(TTB);
        Image$$TTB$$ZI$$Limit = .;
    } > TTB
}
