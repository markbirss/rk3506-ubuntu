/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2023 Rockchip Electronics Co., Ltd.
 */

#ifndef __CORE_CORE_CA_H
#define __CORE_CORE_CA_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined ( __GNUC__ )
  #if defined (__ARM_FP) && (__ARM_FP > 1)
    #if defined (__FPU_PRESENT) && (__FPU_PRESENT == 1)
      #define __FPU_USED       1U
    #else
      #warning "Compiler generates FPU instructions for a device without an FPU (check __FPU_PRESENT)"
      #define __FPU_USED       0U
    #endif
  #else
    #define __FPU_USED         0U
  #endif
#endif

#include "cmsis_compiler.h"               /* CMSIS compiler specific defines */

/* ##########################  MMU functions  ###################################### */

/* Region type attributes */
typedef enum
{
   NORMAL,
   SHARED_NORMAL,
   UNCACHE_NORMAL,
   DEVICE,
} mmu_memory_Type;

void MMU_Enable(void);
void MMU_Disable(void);
// ttb must be 4KB align
void MMU_InitTable(uint64_t *ttb);
void MMU_Map(uint64_t *ttb, uint64_t base_address, uint64_t bytes_count, mmu_memory_Type type);

/* ##########################  Cache functions  ###################################### */
#define SCTLR_I_Pos 12U                                                         /*!< \brief SCTLR: I Position */
#define SCTLR_I_Msk (1UL << SCTLR_I_Pos)                                        /*!< \brief SCTLR: I Mask */

#define SCTLR_C_Pos 2U                                                          /*!< \brief SCTLR: C Position */
#define SCTLR_C_Msk (1UL << SCTLR_C_Pos)                                        /*!< \brief SCTLR: C Mask */

#define SCTLR_A_Pos 1U                                                          /*!< \brief SCTLR: A Position */
#define SCTLR_A_Msk (1UL << SCTLR_A_Pos)                                        /*!< \brief SCTLR: A Mask */

#define SCTLR_M_Pos 0U                                                          /*!< \brief SCTLR: M Position */
#define SCTLR_M_Msk (1UL << SCTLR_M_Pos)                                        /*!< \brief SCTLR: M Mask */

/*******************************************************************************
  *                Hardware Abstraction Layer
   Core Function Interface contains:
   - L1 Cache Functions
   - PL1 Timer Functions
   - GIC Functions
   - MMU Functions
  ******************************************************************************/

/** \brief  Set CSSELR
 */
__STATIC_FORCEINLINE void __set_CSSELR(uint64_t value)
{
    asm volatile ("MSR CSSELR_EL1, %0" : : "r" (value) : "memory");
}

/** \brief  Get CSSELR
    \return CSSELR Register value
 */
__STATIC_FORCEINLINE uint64_t __get_CSSELR(void)
{
    uint64_t csselr;

    asm volatile ("MRS %0, CSSELR_EL1" : "=r" (csselr) :: "memory");

    return csselr;
}

/** \brief  Get CCSIDR
    \return CCSIDR Register value
 */
__STATIC_FORCEINLINE uint64_t __get_CCSIDR(void)
{
    uint64_t ccsidr;

    asm volatile ("MRS %0, CCSIDR_EL1" : "=r" (ccsidr) :: "memory");

    return ccsidr;
}

/** \brief  Get CLIDR
    \return CLIDR Register value
 */
__STATIC_FORCEINLINE uint64_t __get_CLIDR(void)
{
    uint64_t clidr;

    asm volatile ("MRS %0, CLIDR_EL1" : "=r" (clidr) :: "memory");

    return clidr;
}

/* ##########################  L1 Cache functions  ################################# */

/** \brief Enable Caches by setting I and C bits in SCTLR register.
*/
__STATIC_FORCEINLINE void L1C_EnableCaches(void)
{
    __set_SCTLR(__get_SCTLR() | SCTLR_I_Msk | SCTLR_C_Msk);
    __ISB();
}

/** \brief Disable Caches by clearing I and C bits in SCTLR register.
*/
__STATIC_FORCEINLINE void L1C_DisableCaches(void)
{
    __set_SCTLR(__get_SCTLR() & (~SCTLR_I_Msk) & (~SCTLR_C_Msk));
    __ISB();
}

/** \brief  Invalidate the whole instruction cache
*/
__STATIC_FORCEINLINE void L1C_InvalidateICacheAll(void)
{
    asm volatile ("IC IALLU" ::: "memory");
    __DSB();   //ensure completion of the invalidation
    __ISB();   //ensure instruction fetch path sees new I cache state
}

/** \brief  Clean data cache line by address.
* \param [in] va Pointer to data to clear the cache for.
*/
__STATIC_FORCEINLINE void L1C_CleanDCacheMVA(void *va)
{
    asm volatile ("DC CVAC, %0" : : "r" (va) : "memory");
    __DMB();   //ensure the ordering of data cache maintenance operations and their effects
}

/** \brief  Invalidate data cache line by address.
* \param [in] va Pointer to data to invalidate the cache for.
*/
__STATIC_FORCEINLINE void L1C_InvalidateDCacheMVA(void *va)
{
    asm volatile ("DC IVAC, %0" : : "r" (va) : "memory");
    __DMB();   //ensure the ordering of data cache maintenance operations and their effects
}

/** \brief  Clean and Invalidate data cache by address.
* \param [in] va Pointer to data to invalidate the cache for.
*/
__STATIC_FORCEINLINE void L1C_CleanInvalidateDCacheMVA(void *va)
{
    asm volatile ("DC CIVAC, %0" : : "r" (va) : "memory");
    __DMB();   //ensure the ordering of data cache maintenance operations and their effects
}

/** \brief Calculate log2 rounded up
*  - log(0)  => 0
*  - log(1)  => 0
*  - log(2)  => 1
*  - log(3)  => 2
*  - log(4)  => 2
*  - log(5)  => 3
*        :      :
*  - log(16) => 4
*  - log(32) => 5
*        :      :
* \param [in] n input value parameter
* \return log2(n)
*/
__STATIC_FORCEINLINE uint8_t __log2_up(uint32_t n)
{
    if (n < 2U) {
        return 0U;
    }
    uint8_t log = 0U;
    uint32_t t = n;

    while (t > 1U) {
        log++;
        t >>= 1U;
    }
    if (n & 1U) {
        log++;
    }

    return log;
}

/** \brief  Apply cache maintenance to given cache level.
* \param [in] level cache level to be maintained
* \param [in] maint 0 - invalidate, 1 - clean, otherwise - invalidate and clean
*/
__STATIC_FORCEINLINE void __L1C_MaintainDCacheSetWay(uint32_t level, uint32_t maint)
{
    uint64_t Dummy;
    uint64_t ccsidr;
    uint32_t num_sets;
    uint32_t num_ways;
    uint32_t shift_way;
    uint32_t log2_linesize;
    int32_t log2_num_ways;

    Dummy = level << 1U;
    /* set csselr, select ccsidr register */
    __set_CSSELR(Dummy);
    /* get current ccsidr register */
    ccsidr = __get_CCSIDR();
    num_sets = ((ccsidr & 0x0FFFE000U) >> 13U) + 1U;
    num_ways = ((ccsidr & 0x00001FF8U) >> 3U) + 1U;
    log2_linesize = (ccsidr & 0x00000007U) + 2U + 2U;
    log2_num_ways = __log2_up(num_ways);
    if ((log2_num_ways < 0) || (log2_num_ways > 32)) {
        return; // FATAL ERROR
    }
    shift_way = 32U - (uint32_t)log2_num_ways;
    for (int32_t way = num_ways - 1; way >= 0; way--) {
        for (int32_t set = num_sets - 1; set >= 0; set--) {
            Dummy = (level << 1U) | (((uint32_t)set) << log2_linesize) | (((uint32_t)way) << shift_way);
            switch (maint) {
            case 0U: asm volatile ("DC ISW, %0" : : "r" (Dummy) : "memory");  break;
            case 1U: asm volatile ("DC CSW, %0" : : "r" (Dummy) : "memory");  break;
            default: asm volatile ("DC CISW, %0" : : "r" (Dummy) : "memory"); break;
            }
        }
    }
    __DMB();
}

/** \brief  Clean and Invalidate the entire data or unified cache
* Generic mechanism for cleaning/invalidating the entire data or unified cache to the point of coherency
* \param [in] op 0 - invalidate, 1 - clean, otherwise - invalidate and clean
*/
__STATIC_FORCEINLINE void L1C_CleanInvalidateCache(uint32_t op)
{
    uint32_t clidr;
    uint32_t cache_type;

    clidr = __get_CLIDR();
    for (uint32_t i = 0U; i < 7U; i++) {
        cache_type = (clidr >> i * 3U) & 0x7UL;
        if ((cache_type >= 2U) && (cache_type <= 4U)) {
            __L1C_MaintainDCacheSetWay(i, op);
        }
    }
}

/** \brief  Clean and Invalidate the entire data or unified cache
* Generic mechanism for cleaning/invalidating the entire data or unified cache to the point of coherency
* \param [in] op 0 - invalidate, 1 - clean, otherwise - invalidate and clean
* \deprecated Use generic L1C_CleanInvalidateCache instead.
*/
CMSIS_DEPRECATED
__STATIC_FORCEINLINE void __L1C_CleanInvalidateCache(uint32_t op)
{
    L1C_CleanInvalidateCache(op);
}

/** \brief  Invalidate the whole data cache.
*/
__STATIC_FORCEINLINE void L1C_InvalidateDCacheAll(void)
{
    L1C_CleanInvalidateCache(0);
}

/** \brief  Clean the whole data cache.
 */
__STATIC_FORCEINLINE void L1C_CleanDCacheAll(void)
{
    L1C_CleanInvalidateCache(1);
}

/** \brief  Clean and invalidate the whole data cache.
 */
__STATIC_FORCEINLINE void L1C_CleanInvalidateDCacheAll(void)
{
    L1C_CleanInvalidateCache(2);
}

#ifdef __cplusplus
}
#endif

#endif
