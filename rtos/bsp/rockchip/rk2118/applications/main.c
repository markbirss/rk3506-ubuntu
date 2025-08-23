/**
  * Copyright (c) 2022 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    main.c
  * @version V0.1
  * @brief   application entry
  *
  * Change Logs:
  * Date           Author          Notes
  * 2022-08-04     Cliff.Chen      first implementation
  *
  ******************************************************************************
  */
#include <rtthread.h>
#include "hal_base.h"
#include "tfm_ns_interface.h"

extern void rk_ota_set_boot_success(void);
extern uint32_t psa_framework_version(void);
extern uint32_t firmware_version;

void test_reg_read(void)
{
    uint32_t value1, value2;
    uint32_t start, time1, time2;

    for (int i = 0; i < 100; i++)
    {
        start = SysTick->VAL;
        value1 = SAI0->VERSION;
        time1 = start - SysTick->VAL;

        start = SysTick->VAL;
        value2 = TIMER0->CURRENT_VALUE[0];
        time2 = start - SysTick->VAL;

        rt_kprintf("read register latency: %d(value=0x%x), %d(value=0x%x)\n",
                   time1, value1, time2, value2);
    }
}

int main(void)
{
    //test_reg_read();

    //tfm_ns_interface_init();
    //psa_framework_version();
#ifdef RK2118_CPU_CORE0
    rt_kprintf("this is cpu0\n");

#ifdef RT_USING_FWANALYSIS
    rt_kprintf("FIRMWARE_VER: %d.%d.%d\n", (uint8_t)(firmware_version >> 24),
               (uint8_t)(firmware_version >> 16), (uint16_t)(firmware_version));
#endif
#ifdef RT_BUILD_RECOVERY_FW
    // set secure for USB2.0_OTG master port
    // SGRF_SOC_SECURE0.bit8 = 0
    *((rt_uint32_t *)0x40150140) = 0x01000000;

    rt_kprintf("in recovery mode\n");
#else
#ifdef RT_USING_NEW_OTA
    rk_ota_set_boot_success();
#endif
#endif

#else
    rt_kprintf("this is cpu1\n");
#endif

    return 0;
}

extern void coremark_main(void);
extern void linpack_main(void);
void stress_entry(void *param)
{
    uint32_t loop;
    uint32_t i = 0;

    loop = *((uint32_t *)param);
    rt_kprintf("loop=%d\n", loop);
    while (i++ < loop)
    {
#ifdef RT_USING_COREMARK
        coremark_main();
#endif
#ifdef RT_USING_LINPACK
        linpack_main();
#endif
    }
}

void cpu_stress(int argc, char **argv)
{
    rt_thread_t thread;
    static uint32_t loop = 1;

    if (argc == 2)
        loop = atoi(argv[1]);

    thread = rt_thread_create("cpu",
                              stress_entry, &loop,
                              2048,
                              21, 20);
    rt_thread_startup(thread);
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(cpu_stress, cpu stress test);
#endif

void mem_bw_read(void *src, uint32_t size, uint32_t loop)
{
    uint32_t i = 0;
    uint64_t time1, time2;
    float time_us, bw;

    time1 = HAL_GetSysTimerCount();
    while (i++ < loop)
    {
        asm volatile(
            "   stmdb   sp!, {r2, r3, r4, r5, r6, r7, r8} \n"
            "   mov  r7, %[size]            \n"
            "   mov  r2, %[addr]            \n"
            "0:                             \n"
            "   ldmia r2!, {r3-r6}          \n"
            "   ldmia r2!, {r3-r6}          \n"
            "   ldmia r2!, {r3-r6}          \n"
            "   ldmia r2!, {r3-r6}          \n"
            "   ldmia r2!, {r3-r6}          \n"
            "   ldmia r2!, {r3-r6}          \n"
            "   ldmia r2!, {r3-r6}          \n"
            "   ldmia r2!, {r3-r6}          \n"
            "   add r8, r8, #128            \n"
            "   cmp r8, r7                  \n"
            "   blo 0b                      \n"
            "   ldmia   sp!, {r2, r3, r4, r5, r6, r7, r8} \n"
            :
            : [addr] "r"(src), [size] "r"(size)
            : "cc", "memory"
        );
    }
    time2 = HAL_GetSysTimerCount();
    if (time2 > time1)
        time_us = ((time2 - time1) * 1000000.0) / PLL_INPUT_OSC_RATE;
    else
        time_us = ((time1 - time2) * 1000000.0) / PLL_INPUT_OSC_RATE;
    bw = (size * loop) / time_us;   /* MB/s */
    rt_kprintf("mem_bw_read: %d MB/s\n", (uint32_t)bw);
}

void mem_bw_write(void *dst, uint32_t size, uint32_t loop)
{
    uint32_t i = 0;
    uint64_t time1, time2;
    float time_us, bw;

    time1 = HAL_GetSysTimerCount();
    while (i++ < loop)
    {
        asm volatile(
            "   stmdb   sp!, {r2, r7, r8}   \n"
            "   mov  r7, %[size]            \n"
            "   mov  r2, %[addr]            \n"
            "   mov  r8, #0                 \n"
            "0:                             \n"
            "   stmia r2!, {r3-r6}          \n"
            "   stmia r2!, {r3-r6}          \n"
            "   stmia r2!, {r3-r6}          \n"
            "   stmia r2!, {r3-r6}          \n"
            "   stmia r2!, {r3-r6}          \n"
            "   stmia r2!, {r3-r6}          \n"
            "   stmia r2!, {r3-r6}          \n"
            "   stmia r2!, {r3-r6}          \n"
            "   add r8, r8, #128            \n"
            "   cmp r8, r7                  \n"
            "   blo 0b                      \n"
            "   ldmia sp!, {r2, r7, r8}     \n"
            :
            : [addr] "r"(dst), [size] "r"(size)
            : "cc", "memory"
        );
    }
    time2 = HAL_GetSysTimerCount();
    if (time2 > time1)
        time_us = ((time2 - time1) * 1000000.0) / PLL_INPUT_OSC_RATE;
    else
        time_us = ((time1 - time2) * 1000000.0) / PLL_INPUT_OSC_RATE;
    bw = (size * loop) / time_us;   /* MB/s */
    rt_kprintf("mem_bw_write: %d MB/s\n", (uint32_t)bw);
}

void mem_bw_copy(void *src, void *dst, uint32_t size, uint32_t loop)
{
    uint32_t i = 0;
    uint64_t time1, time2;
    float time_us, bw;

    time1 = HAL_GetSysTimerCount();
    while (i++ < loop)
    {
        asm volatile(
            "   stmdb   sp!, {r0-r8}        \n"
            "   mov  r4, %[src]             \n"
            "   mov  r5, %[dst]             \n"
            "   mov  r6, %[size]            \n"
            "   mov  r8, #0                 \n"
            "0:                             \n"
            "   ldmia r4!, {r0-r3}          \n"
            "   stmia r5!, {r0-r3}          \n"
            "   ldmia r4!, {r0-r3}          \n"
            "   stmia r5!, {r0-r3}          \n"
            "   ldmia r4!, {r0-r3}          \n"
            "   stmia r5!, {r0-r3}          \n"
            "   ldmia r4!, {r0-r3}          \n"
            "   stmia r5!, {r0-r3}          \n"
            "   add r8, r8, #128            \n"
            "   cmp r8, r6                  \n"
            "   blo 0b                      \n"
            "   ldmia sp!, {r0-r8}          \n"
            : [dst] "+r"(dst)
            : [src] "r"(src), [size] "r"(size)
            : "cc", "memory"
        );
    }
    time2 = HAL_GetSysTimerCount();
    time_us = ((time2 - time1) * 1000000.0) / PLL_INPUT_OSC_RATE;
    bw = (size * loop) / time_us;   /* MB/s */
    rt_kprintf("mem_bw_copy: %d MB/s\n", (uint32_t)bw);
}

void mem_bw_test(int argc, char **argv)
{
    void *src, *dst;
    uint32_t size, loop;
    char op;

    if (argc != 6)
    {
        rt_kprintf("Usage: mem_bw_test op src dst size loop, op=r,w,c\n");
        return;
    }
    op = argv[1][0];
    src = (void *)strtoul(argv[2], NULL, 16);
    dst = (void *)strtoul(argv[3], NULL, 16);
    size = (uint32_t)atoi(argv[4]);
    loop = (uint32_t)atoi(argv[5]);

    rt_kprintf("mem_bw_test: op=%c, src=0x%x, dst=0x%x, size=%d, loop=%d\n",
               op, (uint32_t)src, (uint32_t)dst, size, loop);

    if (size % 128 != 0)
    {
        rt_kprintf("size must be 128 bytes aligned\n");
        return;
    }

    switch (op)
    {
    case 'r':
        mem_bw_read(src, size, loop);
        break;
    case 'w':
        mem_bw_write(dst, size, loop);
        break;
    case 'c':
        mem_bw_copy(src, dst, size, loop);
        break;
    }
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(mem_bw_test, mem bandwidth test);
#endif
