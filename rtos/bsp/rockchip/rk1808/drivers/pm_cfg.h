/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-01-31     tony.xie     the first version
 */

#ifndef __PM_CFG_H__
#define __PM_CFG_H__

/* All modes used for rt_pm_request() and rt_pm_release() */
enum
{
    /* run modes */
    PM_RUN_MODE_HIGH = 0,
    PM_RUN_MODE_NORMAL,
    PM_RUN_MODE_LOW,

    /* sleep modes */
    PM_SLEEP_MODE_SLEEP,
    PM_SLEEP_MODE_TIMER,
    PM_SLEEP_MODE_SHUTDOWN,
};

/* The name of all modes used in the msh command "pm_dump" */
#define PM_MODE_NAMES           \
{                               \
    "Running High Mode",        \
    "Running Normal Mode",      \
    "Running Low Mode",         \
                                \
    "Sleep Mode",               \
    "Timer Mode",               \
    "Shutdown Mode",            \
}

/* run mode count : 3 */
#define PM_RUN_MODE_COUNT       3
/* sleep mode count : 3 */
#define PM_SLEEP_MODE_COUNT     3

#define PM_RUN_MODE_DEFAULT     PM_RUN_MODE_NORMAL

#define PM_SLEEP_MODE_DEFAULT   PM_SLEEP_MODE_SLEEP
#define PM_MODE_DEFAULT PM_SLEEP_MODE_SLEEP

/* support redefining the minimum tick into sleep mode */
#ifndef PM_MIN_ENTER_SLEEP_TICK
#define PM_MIN_ENTER_SLEEP_TICK (1)
#endif

#endif /* __PM_H__ */
