#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Configuration */

#define ARCH_ARM_CORTEX_M33
#define RKMCU_RK2118
#define RK2118_CPU_CORE0

/* RT-Thread Kernel */

#define RT_USING_CORE_RTTHREAD
#define RT_NAME_MAX 8
#define RT_ALIGN_SIZE 4
#define RT_THREAD_PRIORITY_32
#define RT_THREAD_PRIORITY_MAX 32
#define RT_TICK_PER_SECOND 1000
#define RT_USING_OVERFLOW_CHECK
#define RT_USING_HOOK
#define RT_HOOK_USING_FUNC_PTR
#define RT_USING_IDLE_HOOK
#define RT_IDLE_HOOK_LIST_SIZE 4
#define IDLE_THREAD_STACK_SIZE 512

/* kservice optimization */

#define RT_DEBUG
#define RT_DEBUG_USING_IO

/* Inter-Thread communication */

#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX
#define RT_USING_EVENT
#define RT_USING_MAILBOX
#define RT_USING_MESSAGEQUEUE

/* Memory Management */

#define RT_USING_MEMPOOL
#define RT_USING_SMALL_MEM
#define RT_USING_MEMHEAP
#define RT_MEMHEAP_FAST_MODE
#define RT_USING_SMALL_MEM_AS_HEAP
#define RT_USING_HEAP

/* Kernel Device Object */

#define RT_USING_DEVICE
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 128
#define RT_CONSOLE_DEVICE_NAME "uart0"
#define RT_VER_NUM 0x40101
#define ARCH_ARM
#define RT_USING_CPU_FFS
#define ARCH_ARM_CORTEX_M

/* RT-Thread Components */

#define RT_USING_COMPONENTS_INIT
#define RT_USING_USER_MAIN
#define RT_MAIN_THREAD_STACK_SIZE 2048
#define RT_MAIN_THREAD_PRIORITY 10
#define RT_USING_MSH
#define RT_USING_FINSH
#define FINSH_USING_MSH
#define FINSH_THREAD_NAME "tshell"
#define FINSH_THREAD_PRIORITY 20
#define FINSH_THREAD_STACK_SIZE 4096
#define FINSH_USING_HISTORY
#define FINSH_HISTORY_LINES 5
#define FINSH_USING_SYMTAB
#define FINSH_CMD_SIZE 80
#define MSH_USING_BUILT_IN_COMMANDS
#define FINSH_USING_DESCRIPTION
#define FINSH_ARG_MAX 10

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_USING_SERIAL
#define RT_USING_SERIAL_V1
#define RT_SERIAL_USING_DMA
#define RT_SERIAL_RB_BUFSZ 64
#define RT_USING_I2C
#define RT_USING_I2C_BITOPS
#define RT_USING_PIN
#define RT_USING_MTD_NOR

/* Using USB */


/* C/C++ and POSIX layer */

#define RT_LIBC_DEFAULT_TIMEZONE 8

/* POSIX (Portable Operating System Interface) layer */


/* Interprocess Communication (IPC) */


/* Socket is in the 'Network' category */


/* Network */


/* Utilities */

#define RT_USING_CMBACKTRACE
#define PKG_CMBACKTRACE_PLATFORM_M33
#define PKG_CMBACKTRACE_DUMP_STACK
#define PKG_CMBACKTRACE_PRINT_ENGLISH

/* RT-Thread Benchmarks */

#define RT_USING_BENCHMARK
#define RT_USING_COREMARK

/* System */


/* RT-Thread Utestcases */


/* RT-Thread rockchip common drivers */

#define RT_USING_CACHE

/* Enable Fault Dump Hook */

#define RT_USING_SNOR
#define RT_SNOR_SPEED 120000000
#define RT_SNOR_XIP_DATA_BEGIN 327680
#define RT_USING_SNOR_FSPI_HOST

/* RT-Thread rockchip jpeg enc driver */


/* RT-Thread rockchip pm drivers */


/* RT-Thread rockchip mipi-dphy driver */


/* RT-Thread rockchip isp driver */


/* RT-Thread rockchip vcm driver */


/* RT-Thread rockchip vicap driver */


/* RT-Thread rockchip camera driver */


/* RT-Thread rockchip vicap_lite driver */


/* RT-Thread rockchip csi2host driver */


/* RT-Thread rockchip buffer_manage driver */


/* RT-Thread rockchip coredump driver */


/* Enable PSTORE */


/* RT-Thread rockchip RPMSG driver */

/* Enable FTL */


/* RT-Thread rockchip RK2118 drivers */

#define RT_USING_CRU

/* Enable I2C */

#define RT_USING_I2C0
#define RT_USING_SYSTICK

/* Enable UART */

#define RT_USING_UART
#define RT_USING_UART0

/* RT-Thread board config */

#define RT_USING_CLK_CONFIG0
#define RT_USING_BOARD_ADSP_DEMO
#define RT_USING_HIFI4

/* RK2118 HIFI4 Project */

#define HIFI4_PROJECT_HELLOWORLD_DEMO
#define HIFI4_PROJECT_MEM_LAYOUT_FILE "components/hifi4/samples/rk2118/helloworld_demo/mem_layout.h"

/* RK2118 rtt app */


/* RT-Thread Common Test case */


#endif
