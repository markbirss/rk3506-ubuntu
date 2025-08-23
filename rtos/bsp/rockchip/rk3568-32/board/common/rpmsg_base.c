/**
  * Copyright (c) 2023 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  */

#include <rtthread.h>
#include <rtdevice.h>

#ifdef RT_USING_RPMSG_LITE
#include "hal_base.h"
#include "rpmsg_lite.h"
#include "rpmsg_queue.h"
#include "rpmsg_base.h"

#ifndef RT_USING_LINUX_RPMSG

static uint32_t remote_table[3] = {REMOTE_ID_2, REMOTE_ID_3, REMOTE_ID_0};

#ifdef PRIMARY_CPU
static struct rpmsg_lite_instance *instance[3] = {0, 0, 0};
static void rpmsg_master_init(void)
{
    uint32_t i;
    uint32_t master_id;
    uint32_t rpmsg_base;

    master_id = HAL_CPU_TOPOLOGY_GetCurrentCpuId();
    RT_ASSERT(master_id == MASTER_ID);

    for (i = 0; i < 3; i++)
    {

        rpmsg_base = RPMSG_MEM_BASE + i * RPMSG_POOL_SIZE;
        RT_ASSERT((rpmsg_base + RPMSG_POOL_SIZE) <= RPMSG_MEM_END);

        instance[i] = rpmsg_lite_master_init((void *)rpmsg_base,
                                             RPMSG_POOL_SIZE,
                                             RL_PLATFORM_SET_LINK_ID(master_id, remote_table[i]),
                                             RL_NO_FLAGS);
        rpmsg_lite_wait_for_link_up(instance[i]);
    }

    rt_kprintf("[cpu:%d]: rpmsg master init ok!\n", master_id);
}

struct rpmsg_lite_instance *rpmsg_master_get_instance(uint32_t master_id, uint32_t remote_id)
{
    uint32_t i, time_out = 20000;
    uint32_t cur_id;

    //master_id reserved for future

    cur_id = HAL_CPU_TOPOLOGY_GetCurrentCpuId();
    RT_ASSERT(cur_id == MASTER_ID);

    for (i = 0; i < 3; i++)
    {
        if (remote_id == remote_table[i])
        {
            break;
        }
    }
    RT_ASSERT(i < 3);

    while (time_out--)
    {

        if (instance[i])
        {
            return instance[i];
        }
        rt_thread_mdelay(1);
    }
    RT_ASSERT(time_out != 0);

    return RT_NULL;
}

INIT_APP_EXPORT(rpmsg_master_init);

#else

static struct rpmsg_lite_instance *instance = NULL;
static void rpmsg_remote_init(void)
{
    uint32_t i;
    uint32_t remote_id;
    uint32_t rpmsg_base;

    remote_id = HAL_CPU_TOPOLOGY_GetCurrentCpuId();
    RT_ASSERT(remote_id != MASTER_ID);

    for (i = 0; i < 3; i++)
    {
        if (remote_id == remote_table[i])
        {
            break;
        }
    }
    RT_ASSERT(i < 3);

    rpmsg_base = RPMSG_MEM_BASE + i * RPMSG_POOL_SIZE;
    RT_ASSERT((rpmsg_base + RPMSG_POOL_SIZE) <= RPMSG_MEM_END);

    instance = rpmsg_lite_remote_init((void *)rpmsg_base,
                                      RL_PLATFORM_SET_LINK_ID(MASTER_ID, remote_id), RL_NO_FLAGS);
    rpmsg_lite_wait_for_link_up(instance);

    rt_kprintf("[cpu:%d]: rpmsg remote init ok!\n", remote_id);
}

struct rpmsg_lite_instance *rpmsg_remote_get_instance(uint32_t master_id, uint32_t remote_id)
{
    uint32_t time_out = 20000;
    uint32_t cur_id;

    //master_id & remote_id reserved for future

    cur_id = HAL_CPU_TOPOLOGY_GetCurrentCpuId();
    RT_ASSERT(cur_id != MASTER_ID);

    while (time_out--)
    {
        if (instance)
        {
            return instance;
        }
        rt_thread_mdelay(1);
    }
    RT_ASSERT(time_out != 0);

    return RT_NULL;
}

INIT_APP_EXPORT(rpmsg_remote_init);

#endif

#endif /* RT_USING_LINUX_RPMSG */

#endif
