#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <stdlib.h>
#include <time.h>
#include "hal_base.h"

#define THREAD_STACK_SIZE   1024
#define THREAD_PRIORITY      25
#define THREAD_COUNT         4 // 4个线程并发

static rt_sem_t test_sem;
static rt_mutex_t test_mutex;

static void memory_alloc_thread_entry(void *parameter)
{
    void *ptr = RT_NULL;

    while (1)
    {
        rt_size_t alloc_size = rand() % 2048 + 1;
        ptr = rt_malloc(alloc_size);
        if (ptr)
        {
            rt_kprintf("Memory allocated: %p, size: %d\n", ptr, alloc_size);
            rt_thread_mdelay(rand() % 100);
            rt_free(ptr);
            rt_kprintf("Memory freed: %p\n", ptr);
        }
        else
        {
            rt_kprintf("Failed to allocate memory.\n");
        }
        HAL_CPUDelayUs(rand() % 10000);
    }
}

static void semaphore_thread_entry(void *parameter)
{
    while (1)
    {
        if (rt_sem_take(test_sem, RT_WAITING_FOREVER) == RT_EOK)
        {
            rt_kprintf("Semaphore taken by thread: %d.\n", (int)(rt_uint32_t)parameter);
            HAL_CPUDelayUs(rand() % 100000);
            rt_sem_release(test_sem);
            rt_kprintf("Semaphore released by thread: %d.\n", (int)(rt_uint32_t)parameter);
        }
        rt_thread_mdelay(rand() % 100);
    }
}

static void mutex_thread_entry(void *parameter)
{
    while (1)
    {
        if (rt_mutex_take(test_mutex, RT_WAITING_FOREVER) == RT_EOK)
        {
            rt_kprintf("Mutex taken by thread: %d.\n", (int)(rt_uint32_t)parameter);
            HAL_CPUDelayUs(rand() % 100000);
            rt_mutex_release(test_mutex);
            rt_kprintf("Mutex released by thread: %d.\n", (int)(rt_uint32_t)parameter);
        }
        rt_thread_mdelay(rand() % 100);
    }
}

void stability_test(void)
{
    rt_thread_t memory_alloc_threads[THREAD_COUNT];
    rt_thread_t semaphore_threads[THREAD_COUNT];
    rt_thread_t mutex_threads[THREAD_COUNT];

    int i;

    srand((unsigned int)time(RT_NULL));

    /* 创建信号量 */
    test_sem = rt_sem_create("test_sem", 1, RT_IPC_FLAG_FIFO);
    if (test_sem == RT_NULL)
    {
        rt_kprintf("Failed to create semaphore.\n");
        return;
    }

    /* 创建互斥锁 */
    test_mutex = rt_mutex_create("test_mutex", RT_IPC_FLAG_FIFO);
    if (test_mutex == RT_NULL)
    {
        rt_kprintf("Failed to create mutex.\n");
        return;
    }

    /* 创建内存分配线程 */
    for (i = 0; i < THREAD_COUNT; i++)
    {
        char name[RT_NAME_MAX];
        rt_snprintf(name, sizeof(name), "mem_thread%d", i);
        memory_alloc_threads[i] = rt_thread_create(name,
                                                   memory_alloc_thread_entry,
                                                   (void *)(rt_uint32_t)i,
                                                   THREAD_STACK_SIZE,
                                                   THREAD_PRIORITY,
                                                   10);
        if (memory_alloc_threads[i] != RT_NULL)
        {
            rt_thread_startup(memory_alloc_threads[i]);
        }
        else
        {
            rt_kprintf("Failed to create memory_alloc_thread %d.\n", i);
        }
    }

    /* 创建信号量线程 */
    for (i = 0; i < THREAD_COUNT; i++)
    {
        char name[RT_NAME_MAX];
        rt_snprintf(name, sizeof(name), "sem_thread%d", i);
        semaphore_threads[i] = rt_thread_create(name,
                                                semaphore_thread_entry,
                                                (void *)(rt_uint32_t)i,
                                                THREAD_STACK_SIZE,
                                                THREAD_PRIORITY + 1,
                                                10);
        if (semaphore_threads[i] != RT_NULL)
        {
            rt_thread_startup(semaphore_threads[i]);
        }
        else
        {
            rt_kprintf("Failed to create semaphore_thread %d.\n", i);
        }
    }
    /* 创建互斥锁线程 */
    for (i = 0; i < THREAD_COUNT; i++)
    {
        char name[RT_NAME_MAX];
        rt_snprintf(name, sizeof(name), "mutex_thread%d", i);
        mutex_threads[i] = rt_thread_create(name,
                                            mutex_thread_entry,
                                            (void *)(rt_uint32_t)i,
                                            THREAD_STACK_SIZE,
                                            THREAD_PRIORITY + 2,
                                            10);
        if (mutex_threads[i] != RT_NULL)
        {
            /* 将互斥锁线程绑定到特定的CPU */
            rt_thread_control(mutex_threads[i], RT_THREAD_CTRL_BIND_CPU, (void *)(rt_uint32_t)i);
            rt_thread_startup(mutex_threads[i]);
        }
        else
        {
            rt_kprintf("Failed to create mutex_thread %d.\n", i);
        }
    }
}

MSH_CMD_EXPORT(stability_test, RT - Thread stability test with concurrency and randomness);