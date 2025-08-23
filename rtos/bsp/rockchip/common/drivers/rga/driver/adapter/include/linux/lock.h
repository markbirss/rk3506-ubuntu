/**
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: Cerf Yu <cerf.yu@rock-chips.com>
 */

#ifndef __RTT_ADAPTER_LINUX_LOCK_H__
#define __RTT_ADAPTER_LINUX_LOCK_H__

#include <rtdevice.h>
#include <assert.h>
#include <errno.h>

/* mutex */
#define mutex rt_mutex

#define mutex_lock(m) rt_mutex_take(m, RT_WAITING_FOREVER)
#define mutex_unlock rt_mutex_release
#define mutex_trylock rt_mutex_trytake

static inline rt_err_t mutex_is_locked(struct rt_mutex *m)
{
    rt_err_t rc = mutex_trylock(m);
    if (rc == RT_EOK)
    {
        // Locked, so was not locked; unlock and return not locked
        mutex_unlock(m);
        return 0;
    }
    else if (rc == -RT_ETIMEOUT)
    {
        // Already locked
        return 1;
    }
    else
    {
        // Error occurred, so we do not really know, but be conversative
        return 1;
    }
}

#define mutex_init(m) ({ \
    int __rc = rt_mutex_init(m, "rga_mutex", RT_IPC_FLAG_FIFO); \
    assert(__rc == RT_EOK); \
    __rc; \
})

#define mutex_destroy(m) ({ \
    int __rc = rt_mutex_detach(m); \
    assert(__rc == RT_EOK); \
    __rc; \
})

/* spinlock */
#ifdef RT_USING_SMP
typedef struct rt_spinlock spinlock_t;

#define spin_lock_irqsave(lock, flag) ({ \
    flag = rt_spin_lock_irqsave(lock); \
})
#define spin_unlock_irqrestore rt_spin_unlock_irqrestore
#define spin_lock_init rt_spin_lock_init
#else
typedef struct rt_mutex spinlock_t;

#define spin_lock_irqsave(lock, flag) ({ \
    (void)(flag); \
    rt_mutex_take(lock, RT_WAITING_FOREVER); \
})
#define spin_unlock_irqrestore(lock, flag) ({ \
    (void)(flag); \
    rt_mutex_release(lock); \
})
#define spin_lock_init(m) ({ \
    int __rc = rt_mutex_init(m, "rga_mutex", RT_IPC_FLAG_FIFO); \
    assert(__rc == RT_EOK); \
    __rc; \
})
#endif

#endif /* #ifndef __RTT_ADAPTER_LINUX_LOCK_H__ */
