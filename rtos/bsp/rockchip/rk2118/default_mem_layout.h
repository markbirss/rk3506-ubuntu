/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef _MEM_LAYOUT_H_
#define _MEM_LAYOUT_H_

/* -------------------------------------------------------------------------- */
/* Notes:                                                                     */
/* 1. Users are encouraged to modify the "size" definitions as needed,        */
/*    the base addresses will automatically adjust to these changes.          */
/* 2. Always ensure that total allocated sizes do not exceed the physical     */
/*    memory size available on the device.                                    */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Physical Memory Sizes                                                      */
/* -------------------------------------------------------------------------- */
XIP_SIZE       =         0x00400000; /* 4 MB - Total size of XIP memory */
SRAM_SIZE      =         0x00100000; /* 1 MB - Total size of SRAM */
DRAM_SIZE      =         0x04000000; /* 64 MB - Total size of DRAM */

/* -------------------------------------------------------------------------- */
/* XIP Memory Layout                                                          */
/* -------------------------------------------------------------------------- */
XIP_BASE       =         0x11000000;

/* Sizes - User-modifiable */
XIP_RKPARTITIONTABLE_SIZE  =  0x00010000; /* 64 KB */
XIP_IDBLOCK_SIZE           =  0x00020000; /* 128 KB */
XIP_CPU0_TFM_SIZE          =  0x00020000; /* 128 KB */
XIP_CPU1_LOADER_SIZE       =  0x00010000; /* 64 KB */
XIP_CPU0_RTT_SIZE          =  0x00040000; /* 256 KB */
XIP_DSP0_FIRMWARE_SIZE     =  0x00100000; /* 1 MB */
XIP_DSP1_FIRMWARE_SIZE     =  0x00100000; /* 1 MB */
XIP_DSP2_FIRMWARE_SIZE     =  0x00100000; /* 1 MB */
XIP_CPU1_RTT_SIZE          =  0x00040000; /* 256 KB */
XIP_USER_DATA_SIZE         =  0x00020000; /* 128 KB */

/* Automatically calculated base addresses */
XIP_RKPARTITIONTABLE_BASE  = XIP_BASE;
XIP_IDBLOCK_BASE           = (XIP_RKPARTITIONTABLE_BASE + XIP_RKPARTITIONTABLE_SIZE);
XIP_CPU0_TFM_BASE          = (XIP_IDBLOCK_BASE + XIP_IDBLOCK_SIZE);
XIP_CPU1_LOADER_BASE       = (XIP_CPU0_TFM_BASE + XIP_CPU0_TFM_SIZE);
XIP_CPU0_RTT_BASE          = (XIP_CPU1_LOADER_BASE + XIP_CPU1_LOADER_SIZE);
XIP_DSP0_FIRMWARE_BASE     = (XIP_CPU0_RTT_BASE + XIP_CPU0_RTT_SIZE);
XIP_DSP1_FIRMWARE_BASE     = (XIP_DSP0_FIRMWARE_BASE + XIP_DSP0_FIRMWARE_SIZE);
XIP_DSP2_FIRMWARE_BASE     = (XIP_DSP1_FIRMWARE_BASE + XIP_DSP1_FIRMWARE_SIZE);
XIP_CPU1_RTT_BASE          = (XIP_DSP2_FIRMWARE_BASE + XIP_DSP2_FIRMWARE_SIZE);
XIP_USER_DATA_BASE         = (XIP_CPU1_RTT_BASE + XIP_CPU1_RTT_SIZE);

/* -------------------------------------------------------------------------- */
/* SRAM Memory Layout                                                         */
/* -------------------------------------------------------------------------- */
SRAM_BASE               = 0x30200000;

/* User-modifiable sizes */
SRAM_CPU0_TFM_SIZE      = 0x00004000; /* 16 KB */
SRAM_CPU0_RTT_SIZE      = 0x0001B000; /* 108 KB */
SRAM_DSP0_SIZE          = 0x00020000; /* 128 KB */
SRAM_DSP1_SIZE          = 0x00060000; /* 384 KB */
SRAM_DSP2_SIZE          = 0x00060000; /* 384 KB */
SRAM_SPI2APB_SIZE       = 0x00001000; /* 4 KB */

/* Automatically calculated base addresses for SRAM */
SRAM_CPU0_TFM_BASE     = SRAM_BASE;
SRAM_CPU0_RTT_BASE     = (SRAM_CPU0_TFM_BASE + SRAM_CPU0_TFM_SIZE);
SRAM_DSP0_BASE         = (SRAM_CPU0_RTT_BASE + SRAM_CPU0_RTT_SIZE);
SRAM_DSP1_BASE         = (SRAM_DSP0_BASE + SRAM_DSP0_SIZE);
SRAM_DSP2_BASE         = (SRAM_DSP1_BASE + SRAM_DSP1_SIZE);
SRAM_SPI2APB_BASE      = (SRAM_DSP2_BASE + SRAM_DSP2_SIZE);
SRAM_END               = (SRAM_SPI2APB_BASE + SRAM_SPI2APB_SIZE);

ASSERT(SRAM_END <= 0x30300000, "SRAM overflow")
ASSERT(SRAM_SPI2APB_BASE == 0x302FF000, "the addr for spi2apb must be 0x302ff000")

/* -------------------------------------------------------------------------- */
/* DRAM Memory Layout                                                         */
/* -------------------------------------------------------------------------- */
DRAM_BASE              = 0xA0000000;

/* User-modifiable sizes */
DRAM_CPU0_TFM_SIZE       =   0x00100000; /* 1 MB */
DRAM_CPU0_RTT_SIZE       =   0x00100000; /* 1 MB */
DRAM_NPU_SIZE            =   0x00400000; /* 4 MB */
DRAM_DSP0_SIZE           =   0x01300000; /* 19 MB */
DRAM_DSP1_SIZE           =   0x01300000; /* 19 MB */
DRAM_DSP2_SIZE           =   0x01300000; /* 19 MB */
DRAM_CPU1_LOADER_SIZE    =   0x00008000; /* 32 KB */
DRAM_CPU1_RTT_SIZE       =   0x000F8000; /* 992 KB */

/* Automatically calculated base addresses for DRAM */
DRAM_CPU0_TFM_BASE       =   DRAM_BASE;
DRAM_CPU0_RTT_BASE       =   (DRAM_CPU0_TFM_BASE + DRAM_CPU0_TFM_SIZE);
DRAM_NPU_BASE            =   (DRAM_CPU0_RTT_BASE + DRAM_CPU0_RTT_SIZE);
DRAM_DSP0_BASE           =   (DRAM_NPU_BASE + DRAM_NPU_SIZE);
DRAM_DSP1_BASE           =   (DRAM_DSP0_BASE + DRAM_DSP0_SIZE);
DRAM_DSP2_BASE           =   (DRAM_DSP1_BASE + DRAM_DSP1_SIZE);
DRAM_CPU1_LOADER_BASE    =   (DRAM_DSP2_BASE + DRAM_DSP2_SIZE);
DRAM_CPU1_RTT_BASE       =   (DRAM_CPU1_LOADER_BASE + DRAM_CPU1_LOADER_SIZE);

/* -------------------------------------------------------------------------- */
/* DSP Stack Sizes                                                            */
/* -------------------------------------------------------------------------- */
/* User-modifiable sizes */
DSP0_STACK_SIZE          = 0x00001000; /* 4 KB */
DSP1_STACK_SIZE          = 0x00001000; /* 4 KB */
DSP2_STACK_SIZE          = 0x00001000; /* 4 KB */

#endif /* _MEM_LAYOUT_H_ */
