/**************************************************************************//**
 * @file     cmsis_cp15.h
 * @brief    CMSIS compiler specific macros, functions, instructions
 * @version  V1.0.0
 * @date     20 Apr 2024
 ******************************************************************************/
/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#if   defined ( __ICCARM__ )
  #pragma system_include         /* treat file as system include file for MISRA check */
#elif defined (__clang__)
  #pragma clang system_header   /* treat file as system include file */
#endif

#ifndef __CMSIS_CP15_H
#define __CMSIS_CP15_H

/** \brief  Get ACTLR
    \return               Auxiliary Control register value
 */
__STATIC_FORCEINLINE uint64_t __get_ACTLR(void)
{
  uint64_t result;
  __ASM volatile ("mrs %0, actlr_el1" : "=r" (result));
  return(result);
}

/** \brief  Set ACTLR
    \param [in]    actlr  Auxiliary Control value to set
 */
__STATIC_FORCEINLINE void __set_ACTLR(uint64_t actlr)
{
  __ASM volatile ("msr actlr_el1, %0" : : "r" (actlr));
}

/** \brief  Get CPACR
    \return               Coprocessor Access Control register value
 */
__STATIC_FORCEINLINE uint64_t __get_CPACR(void)
{
  uint64_t result;
  __ASM volatile ("mrs %0, cpacr_el1" : "=r" (result));
  return(result);
}

/** \brief  Set CPACR
    \param [in]    cpacr  Coprocessor Access Control value to set
 */
__STATIC_FORCEINLINE void __set_CPACR(uint64_t cpacr)
{
  __ASM volatile ("msr cpacr_el1, %0" : : "r" (cpacr));
}

/** \brief  Get DFSR
    \return               Data Fault Status Register value
 */
__STATIC_FORCEINLINE uint32_t __get_DFSR(void)
{
  uint32_t result;
  __ASM volatile ("mrs %0, esr_el1" : "=r" (result));
  return(result);
}

/** \brief  Set DFSR
    \param [in]    dfsr  Data Fault Status value to set
 */
__STATIC_FORCEINLINE void __set_DFSR(uint32_t dfsr)
{
  __ASM volatile ("msr esr_el1, %0" : : "r" (dfsr));
}

/** \brief  Get IFSR
    \return               Instruction Fault Status Register value
 */
__STATIC_FORCEINLINE uint32_t __get_IFSR(void)
{
  uint32_t result;
  __ASM volatile ("mrs %0, ifsr32_el2" : "=r" (result));
  return(result);
}

/** \brief  Set IFSR
    \param [in]    ifsr  Instruction Fault Status value to set
 */
__STATIC_FORCEINLINE void __set_IFSR(uint32_t ifsr)
{
  __ASM volatile ("msr ifsr32_el2, %0" : : "r" (ifsr));
}

/** \brief  Get ISR
    \return               Interrupt Status Register value
 */
__STATIC_FORCEINLINE uint64_t __get_ISR(void)
{
  uint64_t result;
  __ASM volatile ("mrs %0, isr_el1" : "=r" (result));
  return(result);
}

/** \brief  Get TTBR0

    This function returns the value of the Translation Table Base Register 0.

    \return               Translation Table Base Register 0 value
 */
__STATIC_FORCEINLINE uint64_t __get_TTBR0(void)
{
  uint64_t result;
  __ASM volatile ("mrs %0, ttbr0_el1" : "=r" (result));
  return(result);
}

/** \brief  Set TTBR0

    This function assigns the given value to the Translation Table Base Register 0.

    \param [in]    ttbr0  Translation Table Base Register 0 value to set
 */
__STATIC_FORCEINLINE void __set_TTBR0(uint64_t ttbr0)
{
  __ASM volatile ("msr ttbr0_el1, %0" : : "r" (ttbr0));
}

/** \brief  Get DACR

    This function returns the value of the Domain Access Control Register.

    \return               Domain Access Control Register value
 */
__STATIC_FORCEINLINE uint32_t __get_DACR(void)
{
  uint32_t result;
  __ASM volatile ("mrs %0, dacr32_el2" : "=r" (result));
  return(result);
}

/** \brief  Set DACR

    This function assigns the given value to the Domain Access Control Register.

    \param [in]    dacr   Domain Access Control Register value to set
 */
__STATIC_FORCEINLINE void __set_DACR(uint32_t dacr)
{
  __ASM volatile ("msr dacr32_el2, %0" : : "r" (dacr));
}

/** \brief  Set SCTLR

    This function assigns the given value to the System Control Register.

    \param [in]    sctlr  System Control Register value to set
 */
__STATIC_FORCEINLINE void __set_SCTLR(uint64_t sctlr)
{
  __ASM volatile ("msr sctlr_el1, %0" : : "r" (sctlr));
}

/** \brief  Get SCTLR
    \return               System Control Register value
 */
__STATIC_FORCEINLINE uint64_t __get_SCTLR(void)
{
  uint64_t result;
  __ASM volatile ("mrs %0, sctlr_el1" : "=r" (result));
  return(result);
}

/** \brief  Get MPIDR

    This function returns the value of the Multiprocessor Affinity Register.

    \return               Multiprocessor Affinity Register value
 */
__STATIC_FORCEINLINE uint64_t __get_MPIDR(void)
{
  uint64_t result;
  __ASM volatile ("mrs %0, mpidr_el1" : "=r" (result));
  return result;
}

/** \brief  Get VBAR

    This function returns the value of the Vector Base Address Register.

    \return               Vector Base Address Register
 */
__STATIC_FORCEINLINE uint64_t __get_VBAR(void)
{
  uint64_t result;
  __ASM volatile ("mrs %0, vbar_el1" : "=r" (result));
  return result;
}

/** \brief  Set VBAR

    This function assigns the given value to the Vector Base Address Register.

    \param [in]    vbar  Vector Base Address Register value to set
 */
__STATIC_FORCEINLINE void __set_VBAR(uint64_t vbar)
{
  __ASM volatile ("msr vbar_el1, %0" : : "r" (vbar));
}

/** \brief  Get MVBAR

    This function returns the value of the Monitor Vector Base Address Register.

    \return               Monitor Vector Base Address Register
 */
__STATIC_FORCEINLINE uint64_t __get_MVBAR(void)
{
  uint64_t result;
  __ASM volatile ("mrs %0, vbar_el3" : "=r" (result));
  return result;
}

/** \brief  Set MVBAR

    This function assigns the given value to the Monitor Vector Base Address Register.

    \param [in]    mvbar  Monitor Vector Base Address Register value to set
 */
__STATIC_FORCEINLINE void __set_MVBAR(uint64_t mvbar)
{
  __ASM volatile ("msr vbar_el3, %0" : : "r" (mvbar));
}

/** \brief  Set CNTFRQ

  This function assigns the given value to PL1 Physical Timer Counter Frequency Register (CNTFRQ).

  \param [in]    value  CNTFRQ Register value to set
*/
__STATIC_FORCEINLINE void __set_CNTFRQ(uint64_t value)
{
  __ASM volatile ("msr cntfrq_el0, %0" : : "r" (value));
}

/** \brief  Get CNTFRQ

    This function returns the value of the PL1 Physical Timer Counter Frequency Register (CNTFRQ).

    \return               CNTFRQ Register value
 */
__STATIC_FORCEINLINE uint64_t __get_CNTFRQ(void)
{
  uint64_t result;
  __ASM volatile ("mrs %0, cntfrq_el0" : "=r" (result));
  return result;
}

/** \brief  Set CNTP_TVAL

  This function assigns the given value to PL1 Physical Timer Value Register (CNTP_TVAL).

  \param [in]    value  CNTP_TVAL Register value to set
*/
__STATIC_FORCEINLINE void __set_CNTP_TVAL(uint64_t value)
{
  __ASM volatile ("msr cntp_tval_el0, %0" : : "r" (value));
}

/** \brief  Get CNTP_TVAL

    This function returns the value of the PL1 Physical Timer Value Register (CNTP_TVAL).

    \return               CNTP_TVAL Register value
 */
__STATIC_FORCEINLINE uint64_t __get_CNTP_TVAL(void)
{
  uint64_t result;
  __ASM volatile ("mrs %0, cntp_tval_el0" : "=r" (result));
  return result;
}

/** \brief  Get CNTPCT

    This function returns the value of the 64 bits PL1 Physical Count Register (CNTPCT).

    \return               CNTPCT Register value
 */
__STATIC_FORCEINLINE uint64_t __get_CNTPCT(void)
{
  uint64_t result;
  __ASM volatile ("mrs %0, cntpct_el0" : "=r" (result));
  return result;
}

/** \brief  Set CNTP_CVAL

  This function assigns the given value to 64bits PL1 Physical Timer CompareValue Register (CNTP_CVAL).

  \param [in]    value  CNTP_CVAL Register value to set
*/
__STATIC_FORCEINLINE void __set_CNTP_CVAL(uint64_t value)
{
  __ASM volatile ("msr cntp_cval_el0, %0" : : "r" (value));
}

/** \brief  Get CNTP_CVAL

    This function returns the value of the 64 bits PL1 Physical Timer CompareValue Register (CNTP_CVAL).

    \return               CNTP_CVAL Register value
 */
__STATIC_FORCEINLINE uint64_t __get_CNTP_CVAL(void)
{
  uint64_t result;
  __ASM volatile ("mrs %0, cntp_cval_el0" : "=r" (result));
  return result;
}

/** \brief  Set CNTP_CTL

  This function assigns the given value to PL1 Physical Timer Control Register (CNTP_CTL).

  \param [in]    value  CNTP_CTL Register value to set
*/
__STATIC_FORCEINLINE void __set_CNTP_CTL(uint64_t value)
{
  __ASM volatile ("msr cntp_ctl_el0, %0" : : "r" (value));
}

/** \brief  Get CNTP_CTL register
    \return               CNTP_CTL Register value
 */
__STATIC_FORCEINLINE uint64_t __get_CNTP_CTL(void)
{
  uint64_t result;
  __ASM volatile ("mrs %0, cntp_ctl_el0" : "=r" (result));
  return result;
}

/** \brief  Set TLBIALL

  TLB Invalidate All
 */
__STATIC_FORCEINLINE void __set_TLBIALL(uint32_t value)
{
  __ASM volatile ("TLBI     VMALLE1       \n"
                  "DSB      SY            \n"
                  "ISB                    \n");
}

/** \brief  Set ICIALLU

  Instruction Cache Invalidate All
 */
__STATIC_FORCEINLINE void __set_ICIALLU(uint32_t value)
{
  __ASM volatile ("IC IALLUIS             \n"
                  "ISB                    \n");
}

/** \brief  Enable Branch Prediction by setting Z bit in SCTLR register.
*/
__STATIC_FORCEINLINE void L1C_EnableBTAC(void) {
  __set_SCTLR( __get_SCTLR() | (1UL << 11));
  __ISB();
}

#endif /* __CMSIS_CP15_H */
