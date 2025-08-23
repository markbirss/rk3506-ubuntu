#include "rtconfig.h"

MEMORY
{
    XIP   (rx)  : ORIGIN = 0x18000000, LENGTH = 16M
    SRAM  (rxw) : ORIGIN = 0x04000000, LENGTH = 1M
    PSRAM (rxw) : ORIGIN = 0x1C000000, LENGTH = 8M
    DTCM  (rxw) : ORIGIN = 0x30200000, LENGTH = 512K
}

FW_MAGIC_LEN        = FIRMWARE_MAGIC_LEN;
XIP_DATA_BASE       = 0x60000000;
SRAM_DATA_BASE      = 0x20000000;
PSRAM_DATA_BASE     = 0x64000000;

RAM_FS_SIZE         = 0x10000;
MAIN_STACK_SIZE     = 0x400;

PROVIDE(xSramDataBase  = SRAM_DATA_BASE );
PROVIDE(xSramBase  = ORIGIN(SRAM) );
PROVIDE(xSramSize  = LENGTH(SRAM) );

PROVIDE(xPsramDataBase = PSRAM_DATA_BASE );
PROVIDE(xPsramBase = ORIGIN(PSRAM) );
PROVIDE(xPsramSize = LENGTH(PSRAM) );

ENTRY(Reset_Handler)

SECTIONS
{
    OS_PART_OFFSET = ORIGIN(XIP) + FW_MAGIC_LEN;
    _stext = OS_PART_OFFSET;
    .vectors OS_PART_OFFSET : AT (OS_PART_OFFSET)
    {
        . = ALIGN(4);
        __ISR_Vector_start__ = .;
        KEEP(*(.vectors))               /* vector table */
        __ISR_Vector_end__ = .;
        *startup_rk2108.o (.text .text.*)
        . = ALIGN(4);
    }

    __vector_remap__ = ORIGIN(SRAM);
    SRAM_TEXT_DEST = ORIGIN(SRAM) +  SIZEOF(.vectors);
    SRAM_TEXT_SRC  = LOADADDR(.vectors) + SIZEOF(.vectors);
    _stext_ext = SRAM_TEXT_DEST;
    .sram.text SRAM_TEXT_DEST : AT (SRAM_TEXT_SRC)
    {
        . = ALIGN(4);
        KEEP(*(.sram.text))
        *hid.o(.rodata .rodata*)        /* read-only data (constants) */
        /* *libForceSramDrivers.a: (.text .text.* .rodata .rodata*) */
        *hal_cache.o(.text .text.* .rodata .rodata*)
        *hal_cpu_sleep.o(.text .text.* .rodata .rodata*)
        *hal_base.o(.text .text.* .rodata .rodata*)
        *hal_snor.o(.text .text.* .rodata .rodata*)
        *hal_fspi.o(.text .text.* .rodata .rodata*)
        *hal_systick.o(.text .text.* .rodata .rodata*)
        *hal_timer.o(.text .text.* .rodata .rodata*)
        *drv_fspi.o(.text .text.* .rodata .rodata*)
        *drv_snor.o(.text .text.* .rodata .rodata*)
        *drv_dsp.o(.text .text.* .rodata .rodata*)
        *drv_pm.o(.text .text.* .rodata .rodata*)
        *hal_mbox.o(.text .text.* .rodata .rodata*)
        *hal_pm_cpu.o(.text .text.* .rodata .rodata*)
        *hal_pm_rk2108.o(.text .text.* .rodata .rodata*)
        *snor_test.o(.text .text.* .rodata .rodata*)
        *hal_qpipsram.o(.text .text.* .rodata .rodata*)
        *drv_qpipsram.o(.text .text.* .rodata .rodata*)
        *drv_psram.o(.text .text.* .rodata .rodata*)
        . = ALIGN(32);
    }
    _etext_ext = .;

    PSRAM_TEXT_DEST = ORIGIN(PSRAM);
    PSRAM_TEXT_SRC  = LOADADDR(.sram.text) + SIZEOF(.sram.text);
    .psram.text PSRAM_TEXT_DEST : AT (PSRAM_TEXT_SRC)
    {
        . = ALIGN(4);
        KEEP(*(.psram.text))
        . = ALIGN(32);
    }

    XIP_TEXT_DEST = LOADADDR(.psram.text) + SIZEOF(.psram.text);
    XIP_TEXT_SRC  = XIP_TEXT_DEST;
    .xip.text XIP_TEXT_DEST : AT (XIP_TEXT_SRC)
    {
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

    SRAM_DATA_DEST = SRAM_DATA_BASE + SIZEOF(.vectors) + SIZEOF(.sram.text);
    SRAM_DATA_SRC  = LOADADDR(.xip.text) + SIZEOF(.xip.text);
    _etext = SRAM_DATA_SRC;
    .sram.data SRAM_DATA_DEST : AT (SRAM_DATA_SRC)
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

    PSRAM_DATA_DEST = PSRAM_DATA_BASE + SIZEOF(.psram.text);
    PSRAM_DATA_SRC  = LOADADDR(.sram.data) + SIZEOF(.sram.data);
    .psram.data PSRAM_DATA_DEST : AT (PSRAM_DATA_SRC)
    {
        . = ALIGN(32);
        KEEP(*(.psram.data))
        . = ALIGN(32);
    }

    COPY_TABLE_DEST = LOADADDR(.psram.data) + SIZEOF(.psram.data);
    COPY_TABLE_SRC  = COPY_TABLE_DEST;
    .copy.table COPY_TABLE_DEST : AT (COPY_TABLE_SRC)
    {
        . = ALIGN(4);
        __copy_table_start__ = .;
        LONG (LOADADDR(.vectors))
        LONG (ORIGIN(SRAM))
        LONG (SIZEOF(.vectors))
        LONG (LOADADDR(.sram.text))
        LONG (ADDR(.sram.text))
        LONG (SIZEOF(.sram.text))
        LONG (LOADADDR(.sram.data))
        LONG (ADDR(.sram.data))
        LONG (SIZEOF(.sram.data))
        LONG (LOADADDR(.psram.text))
        LONG (ADDR(.psram.text))
        LONG (SIZEOF(.psram.text))
        LONG (LOADADDR(.psram.data))
        LONG (ADDR(.psram.data))
        LONG (SIZEOF(.psram.data))
        __copy_table_end__ = .;
    }

    ZERO_TABLE_DEST = LOADADDR(.copy.table) + SIZEOF(.copy.table);
    ZERO_TABLE_SRC  = ZERO_TABLE_DEST;
    .zero.table ZERO_TABLE_DEST : AT (ZERO_TABLE_SRC)
    {
        . = ALIGN(4);
        __zero_table_start__ = .;
        LONG (ADDR(.sram.bss))
        LONG (SIZEOF(.sram.bss))
        LONG (ADDR(.psram.bss))
        LONG (SIZEOF(.psram.bss))
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

    SRAM_BSS_DEST = ADDR(.sram.data) + SIZEOF(.sram.data);
    .sram.bss SRAM_BSS_DEST (NOLOAD) :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
        . = ALIGN(4);
    }

    PSRAM_BSS_DEST = ADDR(.psram.data) + SIZEOF(.psram.data);
    .psram.bss PSRAM_BSS_DEST (NOLOAD) :
    {
        . = ALIGN(32);
        KEEP(*(.psram.bss))
        . = ALIGN(32);
    }

    . = ADDR(.sram.bss) + SIZEOF(.sram.bss);
    RAM_FS_DEST = .;
    /* region for ramfs, enable if you need */
#ifdef RT_USING_DFS_RAMFS
    .ramfs RAM_FS_DEST (NOLOAD) :
    {
        . = ALIGN(4);
        __ramfs_start__ = .;
        . = . + RAM_FS_SIZE;
        __ramfs_end__ = .;
        . = ALIGN(4);
    }
#endif

#ifdef RT_LARGE_HEAP_SIZE
    LARGE_HEAP_DEST = .;
    .large LARGE_HEAP_DEST (NOLOAD) :
    {
        . = ALIGN(4);
        __large_start__ = .;
        . = . + RT_LARGE_HEAP_SIZE;
        __large_end__ = .;
    }
#endif

    MAIN_STACK_DEST = .;
    .stack MAIN_STACK_DEST (NOLOAD) :
    {
        . = ALIGN(4);
        _sstack = .;
        . = . + MAIN_STACK_SIZE;
        _estack = .;
        . = ALIGN(4);
        PROVIDE(__StackTop = .);
    }

    __heap_start__ = .;

#ifdef RT_USING_DTCM_HEAP
    /* region for dtcm memory allocate */
    .dtcm ORIGIN(DTCM) (NOLOAD):
    {
        . = ALIGN(4);
        __dtcm_start__ = .;
        . = . + LENGTH(DTCM);
        __dtcm_end__ = .;
    }
#endif

#ifdef RT_USING_PSRAM_HEAP
    /* region for psram memory allocate */
    . = ADDR(.psram.bss) + SIZEOF(.psram.bss);
    PSRAM_HEAP_DEST = .;
    .psramheap PSRAM_HEAP_DEST (NOLOAD) :
    {
        . = ALIGN(32);
        __psramheap_start__ = .;
#ifdef RT_PSRAM_END_RESERVE
        . = . + LENGTH(PSRAM) - SIZEOF(.psram.text) - SIZEOF(.psram.data) - SIZEOF(.psram.bss) - RT_PSRAM_END_RESERVE;
#else
        . = . + LENGTH(PSRAM) - SIZEOF(.psram.text) - SIZEOF(.psram.data) - SIZEOF(.psram.bss);
#endif
        __psramheap_end__ = .;
        . = ALIGN(32);
    }
#endif
}
