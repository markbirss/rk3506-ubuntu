#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Configuration */

#define RKMCU_RK2206
#define PM_HAS_CUSTOM_CONFIG

/* RT-Thread Kernel */

#define RT_USING_CORE_RTTHREAD
/* #define RT_USING_CORE_FREERTOS */
#define RT_NAME_MAX 8
#define RT_ALIGN_SIZE 4
/* RT_THREAD_PRIORITY_8 is not set */
#define RT_THREAD_PRIORITY_32
/* RT_THREAD_PRIORITY_256 is not set */
#define RT_THREAD_PRIORITY_MAX 32
#define RT_TICK_PER_SECOND 1000
#define RT_USING_OVERFLOW_CHECK
#define RT_USING_HOOK
#define RT_USING_IDLE_HOOK
#define RT_IDEL_HOOK_LIST_SIZE 4
#define IDLE_THREAD_STACK_SIZE 512
/* RT_USING_TIMER_SOFT is not set */
#define RT_DEBUG
/* RT_DEBUG_COLOR is not set */
/* RT_DEBUG_INIT_CONFIG is not set */
#define RT_DEBUG_USING_IO
/* RT_DEBUG_THREAD_CONFIG is not set */
/* RT_DEBUG_SCHEDULER_CONFIG is not set */
/* RT_DEBUG_IPC_CONFIG is not set */
/* RT_DEBUG_TIMER_CONFIG is not set */
/* RT_DEBUG_IRQ_CONFIG is not set */
/* RT_DEBUG_MEM_CONFIG is not set */
/* RT_DEBUG_SLAB_CONFIG is not set */
/* RT_DEBUG_MEMHEAP_CONFIG is not set */
/* RT_DEBUG_MODULE_CONFIG is not set */

/* Inter-Thread communication */

#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX
#define RT_USING_EVENT
#define RT_USING_MAILBOX
#define RT_USING_MESSAGEQUEUE
/* RT_USING_SIGNALS is not set */

/* Memory Management */

#define RT_USING_MEMPOOL
#define RT_USING_MEMHEAP
/* RT_USING_NOHEAP is not set */
#define RT_USING_SMALL_MEM
/* RT_USING_SLAB is not set */
/* RT_USING_MEMHEAP_AS_HEAP is not set */
/* RT_USING_MEMTRACE is not set */
#define RT_USING_HEAP

/* Kernel Device Object */

#define RT_USING_DEVICE
#define RT_USING_DEVICE_OPS
/* RT_USING_INTERRUPT_INFO is not set */
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 128
#define RT_CONSOLE_DEVICE_NAME "uart0"
#define RT_VER_NUM 0x30103
#define ARCH_ARM
#define ARCH_ARM_CORTEX_M
#define ARCH_ARM_CORTEX_M4
/* ARCH_CPU_STACK_GROWS_UPWARD is not set */

/* RT-Thread Components */

#define RT_USING_COMPONENTS_INIT
/* RT_USING_USER_MAIN is not set */

/* C++ features */

/* RT_USING_CPLUSPLUS is not set */

/* Command shell */

#define RT_USING_FINSH
#define FINSH_THREAD_NAME "tshell"
#define FINSH_USING_HISTORY
#define FINSH_HISTORY_LINES 5
#define FINSH_USING_SYMTAB
#define FINSH_USING_DESCRIPTION
/* FINSH_ECHO_DISABLE_DEFAULT is not set */
#define FINSH_THREAD_PRIORITY 20
#define FINSH_THREAD_STACK_SIZE 4096
#define FINSH_CMD_SIZE 80
/* FINSH_USING_AUTH is not set */
#define FINSH_USING_MSH
#define FINSH_USING_MSH_DEFAULT
#define FINSH_USING_MSH_ONLY
#define FINSH_ARG_MAX 10

/* Device virtual file system */

#define RT_USING_DFS
#define DFS_USING_WORKDIR
#define DFS_FILESYSTEMS_MAX 2
#define DFS_FILESYSTEM_TYPES_MAX 2
#define DFS_FD_MAX 16
#define RT_USING_DFS_MNTTABLE
#define RT_USING_DFS_ELMFAT

/* elm-chan's FatFs, Generic FAT Filesystem Module */

#define RT_DFS_ELM_CODE_PAGE 437
#define RT_DFS_ELM_WORD_ACCESS
/* RT_DFS_ELM_USE_LFN_0 is not set */
/* RT_DFS_ELM_USE_LFN_1 is not set */
/* RT_DFS_ELM_USE_LFN_2 is not set */
#define RT_DFS_ELM_USE_LFN_3
#define RT_DFS_ELM_USE_LFN 3
#define RT_DFS_ELM_MAX_LFN 255
#define RT_DFS_ELM_DRIVES 2
#define RT_DFS_ELM_MAX_SECTOR_SIZE 4096
/* RT_DFS_ELM_USE_ERASE is not set */
#define RT_DFS_ELM_REENTRANT
#define RT_USING_DFS_DEVFS
/* RT_USING_DFS_ROMFS is not set */
/* RT_USING_DFS_RAMFS is not set */
/* RT_USING_DFS_UFFS is not set */
/* RT_USING_DFS_JFFS2 is not set */

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_PIPE_BUFSZ 512
#define RT_USING_SERIAL
#define RT_SERIAL_USING_DMA
#define RT_SERIAL_RB_BUFSZ 64
/* RT_USING_CAN is not set */
/* RT_USING_HWTIMER is not set */
/* RT_USING_CPUTIME is not set */
/* RT_USING_I2C is not set */
#define RT_USING_PIN
/* RT_USING_ADC is not set */
/* RT_USING_PWM is not set */
#define RT_USING_MTD_NOR
/* RT_USING_MTD_NAND is not set */
/* RT_USING_MTD is not set */
/* RT_USING_PM is not set */
/* RT_USING_RTC is not set */
/* RT_USING_SDIO is not set */
/* RT_USING_SPI is not set */
/* RT_USING_WDT is not set */
/* RT_USING_AUDIO is not set */
/* RT_USING_SENSOR is not set */

/* Using WiFi */

/* RT_USING_WIFI is not set */

/* Using USB */

/* RT_USING_USB_HOST is not set */
#define RT_USING_USB_DEVICE
#define RT_USBD_THREAD_STACK_SZ 4096
#define USB_VENDOR_ID 0x2207
#define USB_PRODUCT_ID 0x0007
/* RT_USB_DEVICE_COMPOSITE is not set */
/* _RT_USB_DEVICE_NONE is not set */
/* _RT_USB_DEVICE_CDC is not set */
/* _RT_USB_DEVICE_MSTORAGE is not set */
#define _RT_USB_DEVICE_HID
/* _RT_USB_DEVICE_WINUSB is not set */
#define RT_USB_DEVICE_HID
#define RT_USB_DEVICE_HID_KEYBOARD
#define RT_USB_DEVICE_HID_KEYBOARD_NUMBER 1
/* RT_USB_DEVICE_HID_MOUSE is not set */
/* RT_USB_DEVICE_HID_GENERAL is not set */
/* RT_USB_DEVICE_HID_MEDIA is not set */

/* POSIX layer and C standard library */

#define RT_USING_LIBC
/* RT_USING_PTHREADS is not set */
#define RT_USING_POSIX
/* RT_USING_POSIX_MMAP is not set */
/* RT_USING_POSIX_TERMIOS is not set */
/* RT_USING_POSIX_AIO is not set */
/* RT_USING_MODULE is not set */

/* Network */

/* Socket abstraction layer */

/* RT_USING_SAL is not set */

/* light weight TCP/IP stack */

/* RT_USING_LWIP is not set */

/* Modbus master and slave stack */

/* RT_USING_MODBUS is not set */

/* AT commands */

/* RT_USING_AT is not set */

/* System */

/* RT_USING_LITTLEVGL2RTT is not set */

/* VBUS(Virtual Software BUS) */

/* RT_USING_VBUS is not set */

/* Utilities */

#define RT_USING_CMBACKTRACE
/* PKG_CMBACKTRACE_PLATFORM_M0_M0PLUS is not set */
/* PKG_CMBACKTRACE_PLATFORM_M3 is not set */
#define PKG_CMBACKTRACE_PLATFORM_M4
/* PKG_CMBACKTRACE_PLATFORM_M7 is not set */
/* PKG_CMBACKTRACE_PLATFORM_NOT_SELECTED is not set */
#define PKG_CMBACKTRACE_DUMP_STACK
#define PKG_CMBACKTRACE_PRINT_ENGLISH
/* PKG_CMBACKTRACE_PRINT_CHINESE is not set */
/* RT_USING_LOGTRACE is not set */
/* RT_USING_RYM is not set */
/* RT_USING_RZM is not set */
/* RT_USING_ULOG is not set */
/* RT_USING_UTEST is not set */

/* ARM CMSIS */

/* RT_USING_CMSIS_OS is not set */
/* RT_USING_RTT_CMSIS is not set */

/* RT-Thread Benchmarks */

/* RT_USING_BENCHMARK is not set */
/* RT_USING_PLAYER is not set */

/* RT-Thread application */

/* RT_USING_RK_IOT_APP is not set */

/* RT-Thread third party package */

/* PKG_CMSIS_RTOS2 is not set */
/* RT_USING_WLAN_WICED is not set */
/* RT_USING_NETUTILS is not set */
/* RT_USING_PKGS is not set */

/* RT-Thread board config */

#define RT_USING_BOARD_RK2206_EVB
#define M4_JTAG_ENABLE
/* DSP_JTAG_ENABLE is not set */

/* RT-Thread rockchip common drivers */

/* HAL_DBG_ON is not set */
/* RT_USING_SND_GLB_RST is not set */
/* RT_USING_RESET is not set */
/* RT_USING_CRU_DUMP is not set */
/* RT_USING_REGULATOR_DUMP is not set */
/* RT_USING_PMIC is not set */
/* RT_USING_UNCACHE_HEAP is not set */
/* RT_USING_LARGE_HEAP is not set */
/* RT_USING_DTCM_HEAP is not set */
#define RT_USING_SNOR
#define RT_SNOR_SPEED 80000000
#define RT_USING_SNOR_FSPI_HOST
/* RT_USING_SNOR_SFC_HOST is not set */
/* RT_USING_SNOR_SPI_HOST is not set */

/* RT-Thread rockchip pm drivers */

/* RT_USING_PM_DVFS is not set */
/* RT_USING_PM_REQ_PWR is not set */
/* RT_USING_PM_REQ_CLK is not set */
/* RT_USING_DVFS_DBG is not set */
/* RT_USING_PM_RUNTIME is not set */

/* RT-Thread rockchip rk2206 drivers */

#define RT_USING_CRU
#define RT_USING_DMA
#define RT_USING_DMA_DW
/* RT_USING_PMU is not set */
/* RT_USING_RGB is not set */
/* RT_USING_VOP is not set */
/* RT_USING_DSI is not set */

/* Enable DSP */

/* RT_USING_DSP is not set */

/* Enable UART */

#define RT_USING_UART
#define RT_USING_UART0
/* RT_USING_UART1 is not set */
/* RT_USING_UART2 is not set */

/* RT-Thread bsp test case */

/* RT_USING_PRIVATE_TEST is not set */

/* RT-Thread Common Test case */

/* RT_USING_TC is not set */
/* RT_USING_FILE_TEST is not set */
/* RT_USING_COMMON_TEST is not set */

#endif
