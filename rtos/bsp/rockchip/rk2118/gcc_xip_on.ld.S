/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2022 Rockchip Electronics Co., Ltd.
 */

#include "rtconfig.h"
#ifdef HIFI4_PROJECT_MEM_LAYOUT_FILE
#include HIFI4_PROJECT_MEM_LAYOUT_FILE
#else
#include "default_mem_layout.h"
#endif

/*--------------------- Stack / Heap Configuration ----------------------------
  <h> Stack / Heap Configuration
    <o0> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
  </h>
  -----------------------------------------------------------------------------*/
__STACK_SIZE = 0x00000400;

/* ARMv8-M stack sealing:
   to use ARMv8-M stack sealing set __STACKSEAL_SIZE to 8 otherwise keep 0
 */
__STACKSEAL_SIZE = 0;

__FW_MAGIC_SIZE = 0x800;

/* NPU memory manage by CPU0 */
MEMORY
{
#if defined(ARCH_ARM_CORTEX_SECURE)
  CODE (rx) : ORIGIN = XIP_RECOVERY_BASE - 0x10000000 + __FW_MAGIC_SIZE,  LENGTH = XIP_RECOVERY_SIZE - __FW_MAGIC_SIZE
  DATA (rw) : ORIGIN = SRAM_CPU0_RTT_BASE - 0x10000000,  LENGTH = SRAM_CPU0_RTT_SIZE
  DDR (rwx) : ORIGIN = 0x60000000,  LENGTH = 0x4000000
#else
#if defined(RK2118_CPU_CORE0)
#if defined(RT_BUILD_OSB)
  CODE (rx) : ORIGIN = XIP_CPU0_RTTB_BASE + __FW_MAGIC_SIZE,  LENGTH = XIP_CPU0_RTTB_SIZE - __FW_MAGIC_SIZE
#else
  CODE (rx) : ORIGIN = XIP_CPU0_RTT_BASE + __FW_MAGIC_SIZE,  LENGTH = XIP_CPU0_RTT_SIZE - __FW_MAGIC_SIZE
#endif
  DATA (rw) : ORIGIN = SRAM_CPU0_RTT_BASE,  LENGTH = SRAM_CPU0_RTT_SIZE
  DDR (rwx) : ORIGIN = DRAM_CPU0_RTT_BASE,  LENGTH = DRAM_CPU0_RTT_SIZE + DRAM_NPU_SIZE
#elif defined(RK2118_CPU_CORE1)
  CODE (rx) : ORIGIN = XIP_CPU1_RTT_BASE + __FW_MAGIC_SIZE,  LENGTH = XIP_CPU1_RTT_SIZE - __FW_MAGIC_SIZE
  DATA (rw) : ORIGIN = DRAM_CPU1_RTT_BASE,  LENGTH = DRAM_CPU1_RTT_SIZE
  DDR (rwx) : ORIGIN = DRAM_CPU1_RTT_BASE + DRAM_CPU1_RTT_SIZE,  LENGTH = 0
#else
  #error "must select core0 or core1 in menuconfig"
#endif
#ifdef RT_USING_UNCACHE_HEAP
  SRAM_U (rw)  : ORIGIN = UNCACHE_HEAP_BASE, LENGTH = UNCACHE_HEAP_SIZE
#endif
#endif
}

SECTIONS
{
    .vectors :
    {
        . = ALIGN(4);
        __ISR_Vector_start__ = .;
        KEEP(*(.vectors))               /* vector table */
        __ISR_Vector_end__ = .;
        *startup_rk2118.o (.text .text.*)
        . = ALIGN(4);
    } > CODE

    __vector_remap__ = ORIGIN(DATA);
    SRAM_TEXT_DST = ORIGIN(DATA) +  SIZEOF(.vectors);
    SRAM_TEXT_SRC  = LOADADDR(.vectors) + SIZEOF(.vectors);
    .sram.text SRAM_TEXT_DST : AT(SRAM_TEXT_SRC)
    {
#ifdef RK2118_CPU_CORE0
        . = ALIGN(4);
        KEEP(*(.sram.text))
        KEEP(*(.sram_code)) /* HAL sram text */
        KEEP(*(.sram_data)) /* HAL sram data */
        *hid.o(.rodata .rodata*)        /* read-only data (constants) */
        /* *libForceSramDrivers.a: (.text .text.* .rodata .rodata*) */
        *hal_cache.o(.text .text.* .rodata .rodata*)
        *hal_cpu_sleep.o(.text .text.* .rodata .rodata*)
        *hal_base.o(.text .text.* .rodata .rodata*)
        *hal_base_ex.o(.text .text.* .rodata .rodata*)
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
        *hal_pm_rk2118.o(.text .text.* .rodata .rodata*)
        *snor_test.o(.text .text.* .rodata .rodata*)
        *hal_qpipsram.o(.text .text.* .rodata .rodata*)
        *drv_qpipsram.o(.text .text.* .rodata .rodata*)
        *drv_psram.o(.text .text.* .rodata .rodata*)
        *context_gcc.o(.text .text.* .rodata .rodata*)
        . = ALIGN(32);
#endif
    }

    TEXT_SRC = LOADADDR(.sram.text) + SIZEOF(.sram.text);
    TEXT_DST = TEXT_SRC;
    .text TEXT_DST : AT(TEXT_SRC)
    {
        . = ALIGN(4);
        PROVIDE(_stext = .);
        KEEP(*(.text.entrypoint))       /* The entry point */
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

        . = ALIGN(32);
        PROVIDE(_etext = .);
    }

    EXTAB_SRC = LOADADDR(.text) + SIZEOF(.text);
    EXTAB_DST = EXTAB_SRC;
    .ARM.extab EXTAB_DST : AT(EXTAB_SRC)
    {
        *(.ARM.extab*)
    }

    __exidx_start = .;
    EXIDX_SRC = LOADADDR(.ARM.extab) + SIZEOF(.ARM.extab);
    EXIDX_DST = EXIDX_SRC;
    .ARM.exidx EXIDX_DST : AT(EXIDX_SRC)
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)

        /* This is used by the startup in order to initialize the .data secion */
        _sidata = .;
    }
    __exidx_end = .;

    ZERO_TABLE_SRC = LOADADDR(.ARM.exidx) + SIZEOF(.ARM.exidx);
    ZERO_TABLE_DST = ZERO_TABLE_SRC;
    .zero.table ZERO_TABLE_DST : AT(ZERO_TABLE_SRC)
    {
        . = ALIGN(4);
        __zero_table_start__ = .;
        LONG (ADDR(.bss))
        LONG (SIZEOF(.bss) / 4)
        LONG (ADDR(.ddr.bss))
        LONG (SIZEOF(.ddr.bss) / 4)
        __zero_table_end__ = .;
    }

    COPY_TABLE_SRC = LOADADDR(.zero.table) + SIZEOF(.zero.table);
    COPY_TABLE_DST = COPY_TABLE_SRC;
    .copy.table COPY_TABLE_DST : AT(COPY_TABLE_SRC)
    {
        . = ALIGN(4);
        __copy_table_start__ = .;
        LONG (__Vectors)
        LONG (__vector_remap__)
        LONG (__Vectors_Size / 4)
        LONG (LOADADDR(.sram.text))
        LONG (ADDR(.sram.text))
        LONG (SIZEOF(.sram.text) / 4)
        LONG (LOADADDR(.data))
        LONG (ADDR(.data))
        LONG (SIZEOF(.data) / 4)
        LONG (LOADADDR(.ddr.text))
        LONG (ADDR(.ddr.text))
        LONG (SIZEOF(.ddr.text) / 4)
        LONG (LOADADDR(.ddr.data))
        LONG (ADDR(.ddr.data))
        LONG (SIZEOF(.ddr.data) / 4)
        __copy_table_end__ = .;
    }

    CTORS_SRC = LOADADDR(.copy.table) + SIZEOF(.copy.table);
    CTORS_DST = CTORS_SRC;
    .ctors CTORS_DST : AT(CTORS_SRC)
    {
        . = ALIGN(16);
        PROVIDE(__ctors_start__ = .);
        KEEP(*(SORT(.ctors.*)))
        KEEP(*(.ctors))
        /* new GCC version uses .init_array */
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        PROVIDE(__ctors_end__ = .);
    }

    DTORS_SRC = LOADADDR(.ctors) + SIZEOF(.ctors);
    DTORS_DST = DTORS_SRC;
    .dtors DTORS_DST : AT(DTORS_SRC)
    {
        PROVIDE(__dtors_start__ = .);
        KEEP(*(SORT(.dtors.*)))
        KEEP(*(.dtors))
        PROVIDE(__dtors_end__ = .);
    }

    GOT_SRC = LOADADDR(.dtors) + SIZEOF(.dtors);
    GOT_DST = GOT_SRC;
    .got GOT_DST : AT(GOT_SRC)
    {
        *(.got)
        *(.got.plt)
    }

    DDR_TEXT_SRC = LOADADDR(.got) + SIZEOF(.got);
    DDR_TEXT_DST = ORIGIN(DDR);
    .ddr.text DDR_TEXT_DST : AT(DDR_TEXT_SRC)
    {
        . = ALIGN(4);
        *(.ddr.text)
        . = ALIGN(4);
    }

    DATA_DST = ORIGIN(DATA) + SIZEOF(.vectors) + SIZEOF(.sram.text);
    DATA_SRC = LOADADDR(.ddr.text) + SIZEOF(.ddr.text);
    .data DATA_DST : AT(DATA_SRC)
    {
        . = ALIGN(32);
        *(.data)
        *(.data.*)

        . = ALIGN(16);
    }

    DDR_DATA_SRC = LOADADDR(.data) + SIZEOF(.data);
    DDR_DATA_DST = ADDR(.ddr.text) + SIZEOF(.ddr.text);
    .ddr.data DDR_DATA_DST : AT(DDR_DATA_SRC)
    {
        . = ALIGN(4);
        *(.ddr.data)
        . = ALIGN(4);
    }

    BSS_DST = ADDR(.data) + SIZEOF(.data);
    .bss BSS_DST (NOLOAD) :
    {
        . = ALIGN(16);
        PROVIDE(__bss_start__ = .);
        *(.bss)
        *(.bss.*)
        *(.dynbss)
        *(COMMON)
        . = ALIGN(16);
        PROVIDE(__bss_end__ = .);
    }

    DDR_BSS_DST = ADDR(.ddr.data) + SIZEOF(.ddr.data);
    .ddr.bss DDR_BSS_DST (NOLOAD) :
    {
        . = ALIGN(32);
        *(.ddr.bss)
        . = ALIGN(32);
    }

    HEAP_DST = ADDR(.bss) + SIZEOF(.bss);
    .heap HEAP_DST (NOLOAD) :
    {
        . = ALIGN(32);
        PROVIDE(__sram_heap_begin__ = .);
        . = ORIGIN(DATA) + LENGTH(DATA) - __STACK_SIZE - __STACKSEAL_SIZE;
        PROVIDE(__sram_heap_end__ = .);
        . = ALIGN(32);
    }

   .stack (ORIGIN(DATA) + LENGTH(DATA) - __STACK_SIZE - __STACKSEAL_SIZE) (COPY) :
   {
        . = ALIGN(32);
        __StackLimit = .;
        . = . + __STACK_SIZE;
        __StackTop = .;
        . = ALIGN(32);
        PROVIDE(__stack = __StackTop);
        PROVIDE(_sstack = __StackLimit);
        PROVIDE(_estack = __StackTop);
    }

#ifdef RT_USING_UNCACHE_HEAP
    .uncache.heap (NOLOAD) :
    {
        . = ALIGN(UNCACHE_HEAP_SIZE);
        PROVIDE(__uncache_heap_start__ = .);
        . = . + UNCACHE_HEAP_SIZE;
        PROVIDE(__uncache_heap_end__ = .);
        . = ALIGN(UNCACHE_HEAP_SIZE);
    } > SRAM_U
#endif

#ifdef RK2118_CPU_CORE0
    DDR_BSS_DST = ADDR(.ddr.bss) + SIZEOF(.ddr.bss);
    .ddr.heap DDR_BSS_DST (NOLOAD) :
    {
        . = ALIGN(32);
        PROVIDE(__ddr_heap_begin__ = .);
        . = ORIGIN(DDR) + LENGTH(DDR);
        PROVIDE(__ddr_heap_end__ = .);
        . = ALIGN(32);
    }
#endif

   PROVIDE(__code_start__ = ORIGIN(CODE));
   PROVIDE(__code_end__ = ORIGIN(CODE) + LENGTH(CODE) - 1);
   PROVIDE(__data_start__ = ORIGIN(DATA));
   PROVIDE(__data_end__ = ORIGIN(DATA) + LENGTH(DATA) - 1);
#if defined(ARCH_ARM_CORTEX_SECURE)
   PROVIDE(__device_start__ = 0x40000000);
   PROVIDE(__device_end__ = 0x40E00000 - 1);
#else
   PROVIDE(__device_start__ = 0x50000000);
   PROVIDE(__device_end__ = 0x50E00000 - 1);
#endif
#ifdef RK2118_CPU_CORE0
   PROVIDE(__ext_mem_start__ = ORIGIN(DDR));
   PROVIDE(__ext_mem_end__ = ORIGIN(DDR) + LENGTH(DDR) - 1);
#endif
   PROVIDE(__large_heap_start__ = DRAM_NPU_BASE);
   PROVIDE(__large_heap_end__ = DRAM_NPU_BASE + DRAM_NPU_SIZE - 1);
#ifdef RK2118_CPU_CORE0
   PROVIDE(__spi2apb_buffer_start__ = SRAM_SPI2APB_BASE);
   PROVIDE(__spi2apb_buffer_end__ = SRAM_SPI2APB_BASE + SRAM_SPI2APB_SIZE - 1);
#endif
}
