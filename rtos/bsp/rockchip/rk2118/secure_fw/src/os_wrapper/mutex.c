/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-11-28     Cliff      first implementation
 *
 */
#include "os_wrapper/mutex.h"
#include <rtthread.h>

/**
 * \brief Creates a mutex for mutual exclusion of resources
 *
 * \return The handle of the created mutex on success or NULL on error
 */
void *os_wrapper_mutex_create(void)
{
    return rt_mutex_create("trust", RT_IPC_FLAG_FIFO);
}

/**
 * \brief Acquires a mutex that is created by \ref os_wrapper_mutex_create()
 *
 * \param[in] handle   The handle of the mutex to acquire. Should be one of the
 *                     handles returned by \ref os_wrapper_mutex_create()
 * \param[in] timeout  The maximum amount of time(in tick periods) for the
 *                     thread to wait for the mutex to be available.
 *                     If timeout is zero, the function will return immediately.
 *                     Setting timeout to \ref OS_WRAPPER_WAIT_FOREVER will
 *                     cause the thread to wait indefinitely
 *
 * \return \ref OS_WRAPPER_SUCCESS on success or \ref OS_WRAPPER_ERROR on error
 *              or timeout
 */
uint32_t os_wrapper_mutex_acquire(void *handle, uint32_t timeout)
{
    rt_mutex_t mutex = (rt_mutex_t)handle;

    return rt_mutex_take(mutex, timeout);
}

/**
 * \brief Releases the mutex acquired previously
 *

 * \param[in] handle The handle of the mutex that has been acquired
 *
 * \return \ref OS_WRAPPER_SUCCESS on success or \ref OS_WRAPPER_ERROR on error
 */
uint32_t os_wrapper_mutex_release(void *handle)
{
    rt_mutex_t mutex = (rt_mutex_t)handle;

    return rt_mutex_release(mutex);
}

/**
 * \brief Deletes a mutex that is created by \ref os_wrapper_mutex_create()
 *
 * \param[in] handle The handle of the mutex to be deleted
 *
 * \return \ref OS_WRAPPER_SUCCESS on success or \ref OS_WRAPPER_ERROR on error
 */
uint32_t os_wrapper_mutex_delete(void *handle)
{
    rt_mutex_t mutex = (rt_mutex_t)handle;

    return rt_mutex_delete(mutex);
}
