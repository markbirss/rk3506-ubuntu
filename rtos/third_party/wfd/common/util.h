/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef __UTIL_H__
#define __UTIL_H__

#include "queue.h"

#if defined(__RT_THREAD__)
#define strdup rt_strdup
#endif

#define BIT(x)	(1UL << (x))
#define ARRAY_SIZE(x)	(sizeof((x)) / sizeof((x)[0]))

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

#define UL			(unsigned long)
#define ULL			(unsigned long long)
#define IS_ALIGNED(x, a)	(((x) & ((typeof(x))(a) - 1)) == 0)

#define BITS_PER_LONG_LONG	64
#define BITS_PER_BYTE		8
#if defined(ARCH_CPU_64BIT)
#define BITS_PER_LONG		64
#else
#define BITS_PER_LONG		32
#endif
#define BIT_MASK(nr)		(UL(1) << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr)		((nr) / BITS_PER_LONG)

#define __GENMASK_ULL(h, l) \
		(((~ULL(0)) - (ULL(1) << (l)) + 1) & \
		 (~ULL(0) >> (BITS_PER_LONG_LONG - 1 - (h))))

#define GENMASK_ULL(h, l) \
	(__GENMASK_ULL(h, l))

#define GENMASK(h, l) \
	(((~0UL) << (l)) & (~0UL >> (BITS_PER_LONG - 1 - (h))))

#define BITS_PER_TYPE(type)	(sizeof(type) * BITS_PER_BYTE)
#define BITS_TO_LONGS(nr)	DIV_ROUND_UP(nr, BITS_PER_TYPE(long))
#define __ALIGN_KERNEL_MASK(x, mask)	(((x) + (mask)) & ~(mask))
#define __ALIGN_MASK(x, mask)	__ALIGN_KERNEL_MASK((x), (mask))
#define BITMAP_FIRST_WORD_MASK(start) (~0UL << ((start) & (BITS_PER_LONG - 1)))
#define BITMAP_LAST_WORD_MASK(nbits) (~0UL >> (-(nbits) & (BITS_PER_LONG - 1)))
#define __round_mask(x, y) ((__typeof__(x))((y)-1))
#define round_down(x, y) ((x) & ~__round_mask(x, y))
#define __ALIGN_KERNEL(x, a)	__ALIGN_KERNEL_MASK(x, (typeof(x))(a) - 1)

#define find_first_bit(addr, size) find_next_bit((addr), (size), 0)

inline uint64_t timespec2nsec(const struct timespec *ts)
{
	uint64_t nsec;

	if (ts == NULL) {
		return -1LL;
	}

	nsec = (uint64_t)ts->tv_sec * 1000000000LL + ts->tv_nsec;

	return nsec;
}

inline void nsec2timespec(struct timespec *ts, uint64_t nsec)
{
	if (ts == NULL)
		return;

	ts->tv_sec = nsec / 1000000000LL;
	ts->tv_nsec = nsec % 1000000000LL;

	if (ts->tv_nsec < 0) {
		ts->tv_sec--;
		ts->tv_nsec += 1000000000LL;
	}
}

inline int ClockTime_r(clockid_t id, const uint64_t *new, uint64_t *old)
{
	struct timespec new_ts, old_ts;

	if (new != NULL) {
		nsec2timespec(&new_ts, *new);
		if (clock_settime(id, &new_ts) != 0)
			return -1;
	}

	if (clock_gettime(id, &old_ts) != 0)
		return -1;

	if (old != NULL) {
		*old = timespec2nsec(&old_ts);
		if (*old < 0)
			return -1;
	}

	return 0;
}

#endif
