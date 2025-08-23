/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef __MATH64_H__
#define __MATH64_H__

#include <stdint.h>

static inline uint64_t div64_u64_closest(uint64_t dividend, uint64_t divisor)
{
	uint64_t tmp, remainder;

	tmp = dividend / divisor;
	remainder = dividend % divisor;

	if (remainder * 2U >= divisor)
		return tmp + 1U;
	else
		return tmp;
}

static inline uint64_t div64_u64_round_up(uint64_t dividend, uint64_t divisor)
{
	uint64_t tmp, remainder;

	tmp = dividend / divisor;
	remainder = dividend % divisor;

	return remainder ? (tmp + 1U) : tmp;
}

static inline long long div64_round_up(long long dividend, long long divisor)
{
	long long tmp, remainder;

	tmp = dividend / divisor;
	remainder = dividend % divisor;

	return remainder ? (tmp + 1) : tmp;
}

#define div64_ul(x, y)   div64_u64_closest((x), (y))
#define div64_ul_up(x, y)   div64_u64_round_up((x), (y))
#define div64_up(x, y)	div64_round_up((x), (y))

#endif
