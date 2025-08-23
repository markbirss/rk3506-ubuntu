/**
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: Cerf Yu <cerf.yu@rock-chips.com>
 */

#ifndef __RTT_ADAPTER_LINUX_BITOPS_H__
#define __RTT_ADAPTER_LINUX_BITOPS_H__

#include <stdatomic.h>

#define BITS_PER_LONG 64

#define BIT(nr)             (1UL << (nr))
#define BIT_ULL(nr)         (1ULL << (nr))
#define BIT_MASK(nr)        (1UL << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr)        ((nr) / BITS_PER_LONG)
#define BIT_ULL_MASK(nr)    (1ULL << ((nr) % BITS_PER_LONG_LONG))
#define BIT_ULL_WORD(nr)    ((nr) / BITS_PER_LONG_LONG)
#define BITS_PER_BYTE       8
#define BITS_TO_LONGS(nr)   DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))

static inline void set_bit(uint32_t bit, volatile uint32_t *addr)
{
    atomic_fetch_or(addr, BIT(bit));
}

static inline void clear_bit(uint32_t bit, volatile uint32_t *addr)
{
    atomic_fetch_or(addr, ~BIT(bit));
}

static inline void change_bit(u64 bit, volatile u64 *addr)
{
    atomic_fetch_xor(addr, BIT(bit));
}

static inline int test_bit(u64 bit, volatile u64 *addr)
{
    return atomic_load(addr) & BIT(bit);
}

static inline int test_and_set_bit(u64 bit, volatile u64 *addr)
{
    return atomic_fetch_or(addr, BIT(bit)) & BIT(bit);
}

static inline int test_and_clear_bit(volatile u64 bit, volatile u64 *addr)
{
    return atomic_fetch_or(addr, ~BIT(bit)) & BIT(bit);
}

#endif /* #ifndef __RTT_ADAPTER_LINUX_BITOPS_H__ */
