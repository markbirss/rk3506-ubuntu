/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2022 Rockchip Electronics Co., Ltd.
 */

#include "rtconfig.h"

MEMORY
{
    SRAM  (rxw) : ORIGIN = 0x3FF80000, LENGTH = 512K             /* SYSTEM SRAM */
    DRAM  (rxw) : ORIGIN = FIRMWARE_BASE, LENGTH = DRAM_SIZE    /* DRAM */
    SHMEM (rxw) : ORIGIN = SHMEM_BASE, LENGTH = SHMEM_SIZE      /* shared memory for all cpu */
    LINUX_RPMSG (rxw) : ORIGIN = LINUX_RPMSG_BASE, LENGTH = LINUX_RPMSG_SIZE /* shared memory for linux rpmsg */
}

__SPINLOCK_MEM_SIZE = 0x100;
__SHARE_RPMSG_SIZE = 0x80000;
__STACK_SIZE = 0x4000;

SECTIONS
{
    .text :
    {
        _stext = .;
        KEEP(*(.text.entrypoint))       /* The entry point */
        *(.vectors)
        *(.text)                        /* remaining code */
        *(.text.*)                      /* remaining code */

        *(.rodata)                      /* read-only data (constants) */
        *(.rodata*)
        *(.glue_7)
        *(.glue_7t)
        *(.gnu.linkonce.t*)

        *(COMMON)

        /* section information for finsh shell */
        . = ALIGN(16);
        __fsymtab_start = .;
        KEEP(*(FSymTab))
        __fsymtab_end = .;
        . = ALIGN(16);
        __vsymtab_start = .;
        KEEP(*(VSymTab))
        __vsymtab_end = .;
        . = ALIGN(16);

        /* section information for initial. */
        . = ALIGN(16);
        __rt_init_start = .;
        KEEP(*(SORT(.rti_fn*)))
        __rt_init_end = .;
        . = ALIGN(16);

        . = ALIGN(16);
        _etext = .;
    } > DRAM

    __exidx_start = .;
    .ARM.exidx   :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > DRAM
    __exidx_end = .;

    __rodata_start = .;
    .rodata   :
    {
        *(.rodata) *(.rodata.*)
    } > DRAM
    __rodata_end = .;

    .zero.table :
    {
        . = ALIGN(4);
        __zero_table_start__ = .;
        LONG (ADDR(.bss))
        LONG (SIZEOF(.bss) / 4)
#ifdef PRIMARY_CPU
        LONG (ADDR(.share_lock))
        LONG (SIZEOF(.share_lock) / 4)
#endif
        __zero_table_end__ = .;
    } > DRAM

    .copy.table :
    {
        . = ALIGN(4);
        __copy_table_start__ = .;
#ifdef PRIMARY_CPU
       LONG (LOADADDR(.share_data))
       LONG (ADDR(.share_data))
       LONG (SIZEOF(.share_data) / 4)
#endif
        __copy_table_end__ = .;
    } > DRAM

    . = ALIGN(16);

    .data :
    {
        *(.data)
        *(.data.*)

        . = ALIGN(16);
        _gp = ABSOLUTE(.);     /* Base of small data */
    } > DRAM

    . = ALIGN(16);

    .ctors :
    {
        PROVIDE(__ctors_start__ = .);
        KEEP(*(SORT(.ctors.*)))
        KEEP(*(.ctors))
        PROVIDE(__ctors_end__ = .);
    } > DRAM

    .dtors :
    {
        PROVIDE(__dtors_start__ = .);
        KEEP(*(SORT(.dtors.*)))
        KEEP(*(.dtors))
        PROVIDE(__dtors_end__ = .);
    } > DRAM

    .share_lock (NOLOAD) :
    {
        . = ALIGN(64);
        PROVIDE(__spinlock_mem_start__ = .);
        . += __SPINLOCK_MEM_SIZE;
        PROVIDE(__spinlock_mem_end__ = .);
        . = ALIGN(64);
    } > SHMEM

    .share_rpmsg (NOLOAD):
    {
        . = ALIGN(0x1000);
        PROVIDE(__share_rpmsg_start__ = .);
        . += __SHARE_RPMSG_SIZE;
        PROVIDE(__share_rpmsg_end__ = .);
        . = ALIGN(0x1000);
    } > SHMEM

    .share_data :
    {
        . = ALIGN(64);
        PROVIDE(__share_data_start__ = .);
        KEEP(*(.share_data))
        PROVIDE(__share_data_end__ = .);
        . = ALIGN(64);
    } > SHMEM AT > DRAM

    .linux_share_rpmsg (NOLOAD):
    {
        PROVIDE(__linux_share_rpmsg_start__ = .);
        . += LINUX_RPMSG_SIZE;
        PROVIDE(__linux_share_rpmsg_end__ = .);
    } > LINUX_RPMSG

    . = ALIGN(16);
    .bss (NOLOAD) :
    {
        PROVIDE(__bss_start = .);
        *(.bss)
        *(.bss.*)
        *(.dynbss)
        *(COMMON)
        . = ALIGN(16);
        PROVIDE(__bss_end = .);
    } > DRAM

    .heap :
    {
        . = ALIGN(16);
        PROVIDE(__heap_begin = .);
#ifdef RT_USING_UNCACHE_HEAP
        . = ORIGIN(DRAM) + LENGTH(DRAM) - __STACK_SIZE - RT_UNCACHE_HEAP_SIZE;
#else
        . = ORIGIN(DRAM) + LENGTH(DRAM) - __STACK_SIZE;
#endif
        PROVIDE(__heap_end = .);
    } > DRAM

   .stack (ORIGIN(DRAM) + LENGTH(DRAM) - __STACK_SIZE) (COPY):
   {
       . = ALIGN(16);
       PROVIDE(__stack_limit = .);
       . = . + __STACK_SIZE;
       PROVIDE(__stack_top = .);
   } > DRAM
}

