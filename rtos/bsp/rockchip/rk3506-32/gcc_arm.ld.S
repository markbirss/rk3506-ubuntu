/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2023 Rockchip Electronics Co., Ltd.
 */

#include "rtconfig.h"

MEMORY
{
    SRAM  (rxw) : ORIGIN = 0xfff80000, LENGTH = 64K            /* SYSTEM SRAM */
    DRAM  (rxw) : ORIGIN = FIRMWARE_BASE, LENGTH = DRAM_SIZE    /* DRAM */
#ifndef RT_USING_SMP
    LINUX_RPMSG (rxw) : ORIGIN = LINUX_RPMSG_BASE, LENGTH = LINUX_RPMSG_SIZE
#endif
}

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

        _etext = .;
    } > DRAM

    __rodata_start = .;
    .rodata   :
    {
        *(.rodata) *(.rodata.*)
        __start_rockit_md = .;
        KEEP(*(.rockit_md))
        __end_rockit_md = .;
    } > DRAM
    __rodata_end = .;

    .ARM.exidx   :
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        /* This is used by the startup in order to initialize the .data secion */
        _sidata = .;
        __exidx_end = .;
    } > DRAM

    .zero.table :
    {
        . = ALIGN(4);
        __zero_table_start__ = .;
        LONG (ADDR(.bss))
        LONG (SIZEOF(.bss) / 4)
        __zero_table_end__ = .;
    } > DRAM

    .copy.table :
    {
        . = ALIGN(4);
        __copy_table_start__ = .;
        __copy_table_end__ = .;
    } > DRAM

    . = ALIGN(16);

    .data :
    {
        *(.data)
        *(.data.*)
        /* new GCC version uses .init_array */
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))

        . = ALIGN(16);
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

#ifndef RT_USING_SMP
    .linux_share_rpmsg (NOLOAD):
    {
        PROVIDE(__linux_share_rpmsg_start__ = .);
        . += LINUX_RPMSG_SIZE;
        PROVIDE(__linux_share_rpmsg_end__ = .);
    } > LINUX_RPMSG
#endif

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
        . = ORIGIN(DRAM) + LENGTH(DRAM);
        PROVIDE(__heap_end = .);
    } > DRAM
}
