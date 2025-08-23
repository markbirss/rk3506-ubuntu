/**
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: Cerf Yu <cerf.yu@rock-chips.com>
 */

#ifndef __RTT_ADAPTER_LINUX_TYPES_H__
#define __RTT_ADAPTER_LINUX_TYPES_H__

#include <stddef.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>

typedef int8_t s8;
typedef uint8_t u8;
typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;
typedef int64_t s64;
typedef uint64_t u64;

typedef int8_t __s8;
typedef uint8_t __u8;
typedef int16_t __s16;
typedef uint16_t __u16;
typedef int32_t __s32;
typedef uint32_t __u32;
typedef int64_t __s64;
typedef uint64_t __u64;

typedef u16 __le16;
typedef u16 __be16;
typedef u32 __le32;
typedef u32 __be32;
typedef u64 __le64;
typedef u64 __be64;

#define CONFIG_PHYS_ADDR_T_64BIT 64

#define U64_C(x) x ## ULL

/* typedef _Bool bool; */

#define DECLARE_BITMAP(name,bits) \
    unsigned long name[BITS_TO_LONGS(bits)]

#ifndef pgoff_t
#define pgoff_t unsigned long
#endif

// #if __OFF_BITS__ == 32
//  typedef _Uint32t uoff_t;
// #elif __OFF_BITS__ == 64
//  typedef _Uint64t uoff_t;
// #else
//  #error Do not know what size to make uoff_t
// #endif

#ifndef NULL
#define NULL                            (void *) 0
#endif

#define cpu_to_le32(x) (( __le32)(__u32)(x))
#define le32_to_cpu(x) ((__u32)(__le32)(x))
#define cpu_to_le16(x) (( __le16)(__u16)(x))
#define le16_to_cpu(x) ((__u16)(__le16)(x))

#define __kernel
#define __user
#define __iomem
#define __percpu
#define __safe
#define __force
#define __nocast
#define __acquires(x)
#define __releases(x)
#define __cond_lock(x,c)   (c)
#define __chk_user_ptr(x)  (void)0
#define __chk_io_ptr(x)    (void)0
#define __bitwise__
#define __bitwise
#define thread_local __thread
#define __devinit

#define BUG_ON(x)                                               \
    if (x) {                                                \
        do {                                                \
            printk(KERN_ERR "[%s, %d] Runtime check failed for " #x "\n", __func__, __LINE__);  \
            assert(0);                                                  \
        } while (0);                            \
    };

#define BUG()                                           \
    do {                                            \
        printk(KERN_ERR "[%s, %d] Bug in the code detected\n", __func__, __LINE__); \
        assert(0);                                                      \
    } while (0);

#undef offsetof
#ifdef __compiler_offsetof
#define offsetof(TYPE, MEMBER)  __compiler_offsetof(TYPE, MEMBER)
#else
#define offsetof(TYPE, MEMBER)  ((size_t)&((TYPE *)0)->MEMBER)
#endif

#define container_of(ptr, type, member) ({          \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type,member) );})

//typedef uid_t kuid_t;

/* Linux macros */
#define __GFP_DMA       0x01u
#define __GFP_HIGHMEM       0x02u
#define __GFP_DMA32     0x04u
#define __GFP_MOVABLE       0x08u
#define __GFP_WAIT      0x10u
#define __GFP_HIGH      0x20u
#define __GFP_IO        0x40u
#define __GFP_FS        0x80u
#define __GFP_COLD      0x100u
#define __GFP_NOWARN        0x200u
#define __GFP_REPEAT        0x400u
#define __GFP_NOFAIL        0x800u
#define __GFP_NORETRY       0x1000u
#define __GFP_COMP      0x4000u
#define __GFP_ZERO      0x8000u
#define __GFP_NOMEMALLOC    0x10000u
#define __GFP_HARDWALL      0x20000u
#define __GFP_THISNODE      0x40000u
#define __GFP_RECLAIMABLE   0x80000u
#define __GFP_NOTRACK       0
#define __GFP_NO_KSWAPD 0x400000u
#define __GFP_OTHER_NODE    0x800000u
#define __GFP_WRITE     0x1000000u

#define GFP_ATOMIC  (__GFP_HIGH)
#define GFP_NOIO    (__GFP_WAIT)
#define GFP_NOFS    (__GFP_WAIT | __GFP_IO)
#define GFP_KERNEL  (__GFP_WAIT | __GFP_IO | __GFP_FS)
#define GFP_DMA32       0x04u
#define GFP_TEMPORARY   (__GFP_WAIT | __GFP_IO | __GFP_FS | \
             __GFP_RECLAIMABLE)
#define GFP_USER    (__GFP_WAIT | __GFP_IO | __GFP_FS | __GFP_HARDWALL)
#define GFP_HIGHUSER    (__GFP_WAIT | __GFP_IO | __GFP_FS | __GFP_HARDWALL | \
             __GFP_HIGHMEM)
#define GFP_HIGHUSER_MOVABLE    (__GFP_WAIT | __GFP_IO | __GFP_FS | \
                 __GFP_HARDWALL | __GFP_HIGHMEM | \
                 __GFP_MOVABLE)
#define GFP_IOFS    (__GFP_IO | __GFP_FS)
#define GFP_TRANSHUGE   (GFP_HIGHUSER_MOVABLE | __GFP_COMP )

#define GFP_NOWAIT  (GFP_ATOMIC & ~__GFP_HIGH)


typedef unsigned long gfp_t;
typedef unsigned __bitwise__ fmode_t;
typedef unsigned __bitwise__ oom_flags_t;

typedef struct
{
    volatile unsigned counter;
} atomic_t;

typedef u64 dma_addr_t;

#ifdef CONFIG_PHYS_ADDR_T_64BIT
typedef u64 phys_addr_t;
#else
typedef u32 phys_addr_t;
#endif

typedef phys_addr_t resource_size_t;

#endif /* #ifndef __RTT_ADAPTER_LINUX_TYPES_H__ */
