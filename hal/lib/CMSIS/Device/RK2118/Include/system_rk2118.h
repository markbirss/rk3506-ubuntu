/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2020-2023 Rockchip Electronics Co., Ltd.
 */

#ifndef SYSTEM_RK2118_H
#define SYSTEM_RK2118_H

#ifdef __cplusplus
extern "C" {
#endif

/**
  \brief Exception / Interrupt Handler Function Prototype
*/
typedef void(*VECTOR_TABLE_Type)(void);

/**
  \brief System Clock Frequency (Core Clock)
*/
extern uint32_t SystemCoreClock;

/**
  \brief Enable I&Dcache
 */
extern void CacheInit(void);

/**
  \brief Setup the microcontroller system.

   Initialize the System and update the SystemCoreClock variable.
 */
extern void SystemInit (void);


/**
  \brief  Update SystemCoreClock variable.

   Updates the SystemCoreClock with current core Clock retrieved from cpu registers.
 */
extern void SystemCoreClockUpdate (void);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_RK2118_H */
