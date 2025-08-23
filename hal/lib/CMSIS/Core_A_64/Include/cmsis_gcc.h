/**************************************************************************//**
 * @file     cmsis_gcc.h
 * @brief    CMSIS compiler GCC header file
 * @version  V1.0.0
 * @date     20 Dec 2023
 ******************************************************************************/
/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2023 Rockchip Electronics Co., Ltd.
 */

#ifndef __CMSIS_GCC_H
#define __CMSIS_GCC_H

/* ignore some GCC warnings */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* Fallback for __has_builtin */
#ifndef __has_builtin
  #define __has_builtin(x) (0)
#endif

/* CMSIS compiler specific defines */
#ifndef   __ASM
  #define __ASM __asm
#endif
#ifndef   __INLINE
  #define __INLINE inline
#endif
#ifndef   __STATIC_INLINE
  #define __STATIC_INLINE static inline
#endif
#ifndef   __STATIC_FORCEINLINE
  #define __STATIC_FORCEINLINE __attribute__((always_inline)) static inline
#endif
#ifndef   __NO_RETURN
  #define __NO_RETURN __attribute__((__noreturn__))
#endif
#ifndef   CMSIS_DEPRECATED
 #define  CMSIS_DEPRECATED                       __attribute__((deprecated))
#endif
#ifndef   __USED
  #define __USED __attribute__((used))
#endif
#ifndef   __WEAK
  #define __WEAK __attribute__((weak))
#endif
#ifndef   __PACKED
  #define __PACKED __attribute__((packed, aligned(1)))
#endif
#ifndef   __PACKED_STRUCT
  #define __PACKED_STRUCT struct __attribute__((packed, aligned(1)))
#endif
#ifndef   __PACKED_UNION
  #define __PACKED_UNION union __attribute__((packed, aligned(1)))
#endif
#ifndef   __UNALIGNED_UINT32        /* deprecated */
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wpacked"
  #pragma GCC diagnostic ignored "-Wattributes"
struct __attribute__((packed)) T_UINT32 { uint32_t v; };
  #pragma GCC diagnostic pop
  #define __UNALIGNED_UINT32(x) (((struct T_UINT32 *)(x))->v)
#endif
#ifndef   __UNALIGNED_UINT16_WRITE
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wpacked"
  #pragma GCC diagnostic ignored "-Wattributes"
__PACKED_STRUCT T_UINT16_WRITE { uint16_t v; };
  #pragma GCC diagnostic pop
  #define __UNALIGNED_UINT16_WRITE(addr, val) (void)((((struct T_UINT16_WRITE *)(void *)(addr))->v) = (val))
#endif
#ifndef   __UNALIGNED_UINT16_READ
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wpacked"
  #pragma GCC diagnostic ignored "-Wattributes"
__PACKED_STRUCT T_UINT16_READ { uint16_t v; };
  #pragma GCC diagnostic pop
  #define __UNALIGNED_UINT16_READ(addr) (((const struct T_UINT16_READ *)(const void *)(addr))->v)
#endif
#ifndef   __UNALIGNED_UINT32_WRITE
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wpacked"
  #pragma GCC diagnostic ignored "-Wattributes"
__PACKED_STRUCT T_UINT32_WRITE { uint32_t v; };
  #pragma GCC diagnostic pop
  #define __UNALIGNED_UINT32_WRITE(addr, val) (void)((((struct T_UINT32_WRITE *)(void *)(addr))->v) = (val))
#endif
#ifndef   __UNALIGNED_UINT32_READ
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wpacked"
  #pragma GCC diagnostic ignored "-Wattributes"
__PACKED_STRUCT T_UINT32_READ { uint32_t v; };
  #pragma GCC diagnostic pop
  #define __UNALIGNED_UINT32_READ(addr) (((const struct T_UINT32_READ *)(const void *)(addr))->v)
#endif
#ifndef   __ALIGNED
  #define __ALIGNED(x) __attribute__((aligned(x)))
#endif
#ifndef   __RESTRICT
  #define __RESTRICT __restrict
#endif
#ifndef   __COMPILER_BARRIER
  #define __COMPILER_BARRIER() __ASM volatile("":::"memory")
#endif
#ifndef __NO_INIT
  #define __NO_INIT __attribute__ ((section (".bss.noinit")))
#endif
#ifndef __ALIAS
  #define __ALIAS(x) __attribute__ ((alias(x)))
#endif

/* IO definitions (access restrictions to peripheral registers) */
/**
    \defgroup CMSIS_glob_defs CMSIS Global Defines

    <strong>IO Type Qualifiers</strong> are used
    \li to specify the access to peripheral variables.
    \li for automatic generation of peripheral register debug information.
*/
#ifdef __cplusplus
  #define   __I volatile                 /*!< Defines 'read only' permissions */
#else
  #define   __I volatile const           /*!< Defines 'read only' permissions */
#endif
#define     __O  volatile                /*!< Defines 'write only' permissions */
#define     __IO volatile                /*!< Defines 'read / write' permissions */

/* following defines should be used for structure members */
#define     __IM  volatile const         /*! Defines 'read only' structure member permissions */
#define     __OM  volatile               /*! Defines 'write only' structure member permissions */
#define     __IOM volatile               /*! Defines 'read / write' structure member permissions */

/* ##########################  Core Instruction Access  ######################### */
/** \defgroup CMSIS_Core_InstructionInterface CMSIS Core Instruction Interface
  Access to dedicated instructions
  @{
*/

/**
  \brief   No Operation
  \details No Operation does nothing. This instruction can be used for code alignment purposes.
 */
#define __NOP() __ASM volatile ("nop")

/**
  \brief   Wait For Interrupt
 */
#define __WFI()                             __ASM volatile ("wfi":::"memory")

/**
  \brief   Wait For Event
 */
#define __WFE()                             __ASM volatile ("wfe":::"memory")

/**
  \brief   Send Event
 */
#define __SEV()                             __ASM volatile ("sev")

/**
  \brief   Instruction Synchronization Barrier
  \details Instruction Synchronization Barrier flushes the pipeline in the processor,
           so that all instructions following the ISB are fetched from cache or memory,
           after the instruction has been completed.
 */
__STATIC_FORCEINLINE  void __ISB(void)
{
  __ASM volatile ("isb 0xF":::"memory");
}


/**
  \brief   Data Synchronization Barrier
  \details Acts as a special kind of Data Memory Barrier.
           It completes when all explicit memory accesses before this instruction complete.
 */
__STATIC_FORCEINLINE  void __DSB(void)
{
  __ASM volatile ("dsb 0xF":::"memory");
}

/**
  \brief   Data Memory Barrier
  \details Ensures the apparent order of the explicit memory operations before
           and after the instruction, without ensuring their completion.
 */
__STATIC_FORCEINLINE  void __DMB(void)
{
  __ASM volatile ("dmb 0xF":::"memory");
}

/**
  \brief   Count leading zeros
  \details Counts the number of leading zeros of a data value.
  \param [in]  value  Value to count the leading zeros
  \return             number of leading zeros in value
 */
__STATIC_FORCEINLINE uint8_t __CLZ(uint32_t value)
{
    if (value == 0U) {
        return 32U;
    }

    return __builtin_clz(value);
}

/**
  \brief   Enable IRQ Interrupts
  \details Enables IRQ interrupts by clearing special-purpose register PRIMASK.
           Can only be executed in Privileged modes.
 */
__STATIC_FORCEINLINE void __enable_irq(void)
{
  __ASM volatile ("msr daifclr, #1<<1\n"
                  "isb");
}

/**
  \brief   Disable IRQ Interrupts
  \details Disables IRQ interrupts by setting special-purpose register PRIMASK.
           Can only be executed in Privileged modes.
 */
__STATIC_FORCEINLINE void __disable_irq(void)
{
  __ASM volatile ("msr daifset, #1<<1\n"
                  "isb");
}

#include "cmsis_cp15.h"

__STATIC_INLINE void __FPU_Enable(void)
{
  __ASM volatile ("mov     x1, #0x00300000\n"
                  "msr     cpacr_el1, x1\n"
                  "isb\n"
                  :
                  :
                  : "x1");
}

#pragma GCC diagnostic pop

#endif /* __CMSIS_GCC_H */
