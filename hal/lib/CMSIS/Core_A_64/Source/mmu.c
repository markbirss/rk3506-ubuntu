/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "core_ca.h"

#define TTB_L1_LIMIT              (4)                       // only support 4GB by default

#define TTB_SIZE                  ((512 + 512 * TTB_L1_LIMIT) * 8)
                                                            // level 1: 512 entrys; level 2: 512 entrys
                                                            // table layout like this:
                                                            // |----- 512 level 1 entrys -----|
                                                            // |----- 512 level 2 entrys for entry0 of level1 -----|
                                                            // |----- 512 level 2 entrys for entry1 of level1 -----|
                                                            // |----- 512 level 2 entrys for entry2 of level1 -----|
                                                            // |----- 512 level 2 entrys for entry3 of level1 -----|

#define L2_INDEX_SHIFT            (21)
#define L2_INDEX_MASK             (0x1ff << L2_INDEX_SHIFT) // bit29-21

#define TT_S1_NORMAL_NO_CACHE     0x00000000000000401    // Index = 0, AF=1
#define TT_S1_NORMAL_WBWA         0x00000000000000405    // Index = 1, AF=1
#define TT_S1_NORMAL_SHARED       0x00000000000000605    // Index = 1, AF=1, Outer Shareable
#define TT_S1_DEVICE_nGnRnE       0x00600000000000409    // Index = 2, AF=1, PXN=1, UXN=1

void MMU_Enable(void)
{
    __ASM volatile(
    // Enable MMU
    // -----------
    "MRS      x0, SCTLR_EL1                  \n"
    "ORR      x0, x0, #(1 << 0)              \n"   // M=1           Enable the stage 1 MMU
    "ORR      x0, x0, #(1 << 2)              \n"   // C=1           Enable data and unified caches
    "ORR      x0, x0, #(1 << 12)             \n"   // I=1           Enable instruction fetches to allocate into unified caches
                                                   // A=0           Strict alignment checking disabled
                                                   // SA=0          Stack alignment checking disabled
                                                   // WXN=0         Write permission does not imply XN
                                                   // EE=0          EL3 data accesses are little endian
    "MSR      SCTLR_EL1, x0                  \n"
    "ISB                                     \n"

    :
    :
    : "x0"
    );
}

void MMU_Disable(void)
{
    __ASM volatile(
    // Disable MMU
    // -----------
    "MRS      x0, SCTLR_EL1                  \n"
    "BIC      x0, x0, #1                     \n"   // M=0           Disable the stage 1 MMU
    "MSR      SCTLR_EL1, x0                  \n"
    "ISB                                     \n"

    :
    :
    : "x0"
    );
}

// ttb must be 4KB align
void MMU_InitTable(uint64_t *ttb)
{
    __ASM volatile(

    // Set the Base address
    "MOV      x7, %0                           \n" // Save address of level 1
    "MSR      TTBR0_EL1, %0                    \n"

    // Set up memory attributes
    // This equates to:
    // 0 = b01000100 = Normal, Inner/Outer Non-Cacheable
    // 1 = b11111111 = Normal, Inner/Outer WB/WA/RA
    // 2 = b00000000 = Device-nGnRnE
    "MOV      x0, #0x000000000000FF44          \n"
    "MSR      MAIR_EL1, x0                     \n"

    // Set up TCR_EL1
    "MOV      x0, #0x19                        \n" // T0SZ=0b011001 Limits VA space to 39 bits, translation starts @ l1
    "ORR      x0, x0, #(0x1 << 8)              \n" // IGRN0=0b01    Walks to TTBR0 are Inner WB/WA
    "ORR      x0, x0, #(0x1 << 10)             \n" // OGRN0=0b01    Walks to TTBR0 are Outer WB/WA
    "ORR      x0, x0, #(0x3 << 12)             \n" // SH0=0b11      Inner Shareable
    "ORR      x0, x0, #(0x1 << 23)             \n" // EPD1=0b1      Disable table walks from TTBR1
                                                   // TBI0=0b0
                                                   // TG0=0b00      4KB granule for TTBR0 (Note: Different encoding to TG0)
                                                   // A1=0          TTBR0 contains the ASID
                                                   // AS=0          8-bit ASID
                                                   // IPS=0         32-bit IPA space
    "MSR      TCR_EL1, x0                      \n"
    "ISB                                       \n"

    // NOTE: We don't need to set up T1SZ/TBI1/ORGN1/IRGN1/SH1, as we've set EPD==1 (disabling walks from TTBR1)

    // Invalidate TLBs
    "TLBI     VMALLE1                          \n"
    "DSB      SY                               \n"
    "ISB                                       \n"

    // fill level1 entrys
    "MOV      x0, x7                           \n" // Address of L1 table
    "MOV      x1, x7                           \n" // Store L2 table address

    // [0]: 0x0000,0000 - 0x3FFF,FFFF
    "ADD x1, x1, #4096                         \n" // Must be a 4KB align address.
    "LDR x2, =0xFFFFF000                       \n"
    "AND x2, x1, x2                            \n" // NSTable=0 APTable=0 XNTable=0 PXNTable=0.
    "ORR x2, x2, 0x3                           \n"
    "STR x2, [x0], #8                          \n"

    // [1]: 0x4000,0000 - 0x7FFF,FFFF
    "ADD x1, x1, #4096                         \n" // Must be a 4KB align address.
    "LDR x2, =0xFFFFF000                       \n"
    "AND x2, x1, x2                            \n" // NSTable=0 APTable=0 XNTable=0 PXNTable=0.
    "ORR x2, x2, 0x3                           \n"
    "STR x2, [x0], #8                          \n"

    // [2]: 0x8000,0000 - 0xBFFF,FFFF
    "ADD x1, x1, #4096                         \n" // Must be a 4KB align address.
    "LDR x2, =0xFFFFF000                       \n"
    "AND x2, x1, x2                            \n" // NSTable=0 APTable=0 XNTable=0 PXNTable=0.
    "ORR x2, x2, 0x3                           \n"
    "STR x2, [x0], #8                          \n"

    // [3]: 0xC000,0000 - 0xFFFF,FFFF
    "ADD x1, x1, #4096                         \n" // Must be a 4KB align address.
    "LDR x2, =0xFFFFF000                       \n"
    "AND x2, x1, x2                            \n" // NSTable=0 APTable=0 XNTable=0 PXNTable=0.
    "ORR x2, x2, 0x3                           \n"
    "STR x2, [x0], #8                          \n"

    // Add more l1 entry if you want to support over 4GB memory

    :
    : "r" (ttb)
    : "memory", "x0", "x1", "x2", "x7"
    );
}

void MMU_Map(uint64_t *ttb, uint64_t base_address, uint64_t bytes_count, mmu_memory_Type type)
{
    uint64_t *l2_table, attr = TT_S1_NORMAL_WBWA;
    uint32_t index = 0, count;

    index = base_address >> 30;
    if (index >= TTB_L1_LIMIT)
    {
        return;
    }

    l2_table = &ttb[512 + index * 512];
    index = (base_address & L2_INDEX_MASK) >> L2_INDEX_SHIFT;
    l2_table = &l2_table[index];

    // fill level2 entrys
    count = bytes_count >> L2_INDEX_SHIFT;
    for (uint32_t i = 0; i < count; i++)
    {
        switch (type)
        {
            case NORMAL:
                attr = TT_S1_NORMAL_WBWA;
                break;
            case SHARED_NORMAL:
                attr = TT_S1_NORMAL_SHARED;
                break;
            case UNCACHE_NORMAL:
                attr = TT_S1_NORMAL_NO_CACHE;
                break;
            case DEVICE:
                attr = TT_S1_DEVICE_nGnRnE;
                break;
        }
        l2_table[i] = (base_address + (i << L2_INDEX_SHIFT)) | attr;
    }
}
