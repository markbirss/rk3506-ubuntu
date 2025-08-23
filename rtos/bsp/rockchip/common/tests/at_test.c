/*
 * Copyright (c) 2020 Fuzhou Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-02-25     Steven Liu   the first version
 */

#include <rtthread.h>
#include "at_test.h"

#ifdef RT_USING_AUTO_TEST
#ifdef RT_USING_FINSH
#include <finsh.h>

#define AT_TEST_PRIORITY   25
#define AT_TEST_TICK       5
#define AT_TEST_STACK_SIZE 0x800

static rt_thread_t at_test_thread;
static rt_uint8_t at_test_prefix[64];
static const char *at_test_current;

#if (RTTHREAD_VERSION >= RT_VERSION_CHECK(4, 1, 0))
#define NEXT_INDEX(index) (index = (void *)index + sizeof(struct at_case))
static void at_test_entry(void *parameter)
{
    struct at_test_info *test_info = parameter;
    struct at_case *index;
    void *table_begin = test_info->begin;
    void *table_end = test_info->begin + test_info->size;
#else
#define NEXT_INDEX FINSH_NEXT_SYSCALL
static void at_test_entry(void *parameter)
{
    struct finsh_syscall *index;
    void *table_begin = _syscall_table_begin;
    void *table_end = _syscall_table_end;

#endif
    rt_uint8_t pass_count = 0;
    rt_uint8_t fail_count = 0;
    long test_return;

    rt_kprintf("AutoTest Start!\n");
    for (index = table_begin; (void *)index < table_end; NEXT_INDEX(index))
    {
#if (RTTHREAD_VERSION < RT_VERSION_CHECK(4, 1, 0))
        if (rt_strstr(index->name, (const char *)at_test_prefix) == index->name)
        {
            at_test_current = index->name + 4;
#else
        at_test_current = index->name;
#endif
            rt_kprintf("START - %s>\n", at_test_current);
            test_return = index->func();
            if (test_return == RT_EOK)
            {
                rt_kprintf(" PASS - %s\n", at_test_current);
                pass_count++;
            }
            else
            {
                rt_kprintf(" FAIL - %s\n", at_test_current);
                fail_count++;
            }
#if (RTTHREAD_VERSION < RT_VERSION_CHECK(4, 1, 0))
        }
#endif
    }
    rt_kprintf("AutoTest completed %d pass %d fail.\n", pass_count, fail_count);
}

#if (RTTHREAD_VERSION >= RT_VERSION_CHECK(4, 1, 0))
static int f_test_case(void)
{
    rt_kprintf("This f_test_case sample\n");
    return 0;
}

RT_WEAK const struct at_case test_case[] =
{
    {"test_case", f_test_case},
};

RT_WEAK DEFINE_AT_TEST_INFO(test_case)

static int at_test_list(void)
{
    struct at_case *index;

    for (index = (struct at_case *)at_test_info.begin;
            (void *)index < (at_test_info.begin + at_test_info.size) ;
            NEXT_INDEX(index))
        rt_kprintf("%s\n", index->name);

    return RT_EOK;
}
INIT_APP_EXPORT(at_test_list);
#else
static int at_test_list(void)
{
    struct finsh_syscall *index;

    for (index = _syscall_table_begin; index < _syscall_table_end; FINSH_NEXT_SYSCALL(index))
    {
        if (rt_strstr(index->name, "_at_") == index->name)
        {
#ifdef FINSH_USING_DESCRIPTION
            rt_kprintf("%-16s -- %s\n", index->name + 4, index->desc);
#else
            rt_kprintf("%s\n", index->name + 4);
#endif
        }
    }

    return RT_EOK;
}
INIT_APP_EXPORT(at_test_list);
#endif

static int at_test_start(void)
{
    rt_err_t ret = RT_EOK;
    const char *at_case = "all";
    void *parameter = RT_NULL;

#if (RTTHREAD_VERSION >= RT_VERSION_CHECK(4, 1, 0))
    parameter = &at_test_info;
#endif

    rt_memset(at_test_prefix, 0, sizeof(at_test_prefix));
    if (rt_strncmp(at_case, "all", sizeof(at_case)) == 0)
        rt_snprintf((char *)at_test_prefix, sizeof(at_test_prefix), "_at_");
    else
        rt_snprintf((char *)at_test_prefix, sizeof(at_test_prefix), "_at_%s", at_case);

    at_test_thread = rt_thread_create("at_test", at_test_entry, parameter, AT_TEST_STACK_SIZE,
                                      AT_TEST_PRIORITY, AT_TEST_TICK);
    if (at_test_thread != RT_NULL)
        rt_thread_startup(at_test_thread);
    else
        ret = RT_ERROR;

    return ret;
}
INIT_APP_EXPORT(at_test_start);

#endif /* RT_USING_FINSH */
#endif /* RT_USING_AUTO_TEST */
