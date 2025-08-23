/**************************************************************************//**
 * @file     cmsis_compiler.h
 * @brief    CMSIS compiler generic header file
 * @version  V1.0.0
 * @date     20 Dec 2023
 ******************************************************************************/
/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2023 Rockchip Electronics Co., Ltd.
 */

#ifndef __CMSIS_COMPILER_H
#define __CMSIS_COMPILER_H

#include <stdint.h>

/*
 * GNU Compiler
 */
#if defined(__GNUC__)
  #include "cmsis_gcc.h"
#else
  #error Unknown compiler.
#endif

#endif /* __CMSIS_COMPILER_H */
