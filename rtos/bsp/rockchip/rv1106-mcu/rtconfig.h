#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Configuration */

#define SOC_RV1106
#define BOARD_RV1106_RISCV_EVB

/* RT-Thread Kernel */

#define RT_USING_CORE_RTTHREAD
/* RT_USING_CORE_FREERTOS is not set */
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
#define IDLE_THREAD_STACK_SIZE 256
/* RT_USING_TIMER_SOFT is not set */
#define RT_DEBUG
#define RT_DEBUG_COLOR
/* RT_DEBUG_INIT_CONFIG is not set */
#define RT_DEBUG_USING_IO
/* RT_DEBUG_USING_GPIO is not set */
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
/* RT_USING_MEMHEAP is not set */
/* RT_USING_NOHEAP is not set */
#define RT_USING_SMALL_MEM
/* RT_USING_SLAB is not set */
/* RT_USING_MEMTRACE is not set */
#define RT_USING_HEAP

/* Kernel Device Object */

#define RT_USING_DEVICE
/* RT_USING_DEVICE_OPS is not set */
/* RT_USING_INTERRUPT_INFO is not set */
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 128
#define RT_CONSOLE_DEVICE_NAME "uart2"
#define RT_VER_NUM 0x30103
#define ARCH_RISCV
/* ARCH_CPU_STACK_GROWS_UPWARD is not set */

/* RT-Thread Components */

#define RT_USING_COMPONENTS_INIT
/* RT_USING_USER_MAIN is not set */

/* C++ features */

/* RT_USING_CPLUSPLUS is not set */

/* Command shell */

/* RT_USING_FINSH is not set */

/* Device virtual file system */

/* RT_USING_DFS is not set */

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_PIPE_BUFSZ 512
#define RT_USING_SERIAL
#define RT_SERIAL_USING_DMA
#define RT_SERIAL_RB_BUFSZ 64
/* RT_USING_CAN is not set */
/* RT_USING_HWTIMER is not set */
/* RT_USING_CPUTIME is not set */
#define RT_USING_I2C
/* RT_USING_I2C_BITOPS is not set */
#define RT_USING_PIN
#define RT_USING_ADC
#define RT_USING_PWM
/* RT_USING_MTD_NOR is not set */
/* RT_USING_MTD_NAND is not set */
/* RT_USING_MTD is not set */
/* RT_USING_PM is not set */
/* RT_USING_RTC is not set */
/* RT_USING_SDIO is not set */
/* RT_USING_SPI is not set */
/* RT_USING_WDT is not set */
/* RT_USING_AUDIO is not set */
/* RT_USING_SENSOR is not set */
/* RT_USING_TOUCH is not set */

/* Using WiFi */

/* RT_USING_WIFI is not set */

/* Using USB */

/* RT_USING_USB_HOST is not set */
/* RT_USING_USB_DEVICE is not set */

/* POSIX layer and C standard library */

#define RT_USING_LIBC
/* RT_USING_PTHREADS is not set */
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

/* RT_USING_CMBACKTRACE is not set */
/* RT_USING_LOGTRACE is not set */
/* RT_USING_RYM is not set */
/* RT_USING_ULOG is not set */
/* RT_USING_UTEST is not set */

/* RT-Thread Benchmarks */

/* RT_USING_BENCHMARK is not set */
/* RT_USING_NET_HTTP is not set */
/* RT_USING_OTA is not set */
/* RT_USING_AUPIPE is not set */

/* RT-Thread board config */

#define RT_BOARD_NAME "rv1106_evb"

/* RT-Thread rv1106_evb board config */

#define RT_USING_SPL_RTT_SHARE_DDR_ADRESS 0x1e00000

/* RT-Thread rockchip common drivers */

/* HAL_DBG_ON is not set */
/* RT_USING_SND_GLB_RST is not set */
/* RT_USING_RESET is not set */
/* RT_USING_CRU_DUMP is not set */
/* RT_USING_REGULATOR_DUMP is not set */
/* RT_USING_PROF_IRQ is not set */
/* RT_USING_PMIC is not set */
/* RT_USING_PWM_REGULATOR is not set */
#define RT_USING_CACHE
/* RT_USING_UNCACHE_HEAP is not set */
/* RT_USING_LARGE_HEAP is not set */
/* RT_USING_DTCM_HEAP is not set */
/* RT_USING_PSRAM_HEAP is not set */

/* Enable Fault Dump Hook */

/* RT_USING_SNOR is not set */

/* RT-Thread rockchip jpeg enc driver */

/* RT_USING_JPEG_ENC is not set */

/* RT-Thread rockchip pm drivers */

/* RT_USING_PM_DVFS is not set */
/* RT_USING_PM_REQ_PWR is not set */
/* RT_USING_PM_REQ_CLK is not set */
/* RT_USING_DVFS_DBG is not set */
/* RT_USING_PM_RUNTIME is not set */
/* RT_USING_PM_TIMER is not set */

/* RT-Thread rockchip mipi-dphy driver */

/* RT_USING_MIPI_DPHY is not set */

/* RT-Thread rockchip isp driver */

/* RT_USING_ISP is not set */
#define RT_USING_ISP3
#define RT_USING_ISP_DDR_ADRESS 0x1e60000
#define RT_USING_ISP_DDR_SIZE 0x600000
#define RT_USING_CHANNEL_MIPI
/* RT_USING_CHANNEL_DVP is not set */
/* RT_USING_CHANNEL_LVDS is not set */

/* RT-Thread rockchip vcm driver */

/* RT_USING_VCM is not set */

/* RT-Thread rockchip vicap driver */

/* RT_USING_VICAP is not set */

/* RT-Thread rockchip camera driver */

#define RT_USING_CAMERA
/* RT_USING_GC2145 is not set */
/* RT_USING_OV8825 is not set */
/* RT_USING_OV5695 is not set */
/* RT_USING_GC5035 is not set */
/* RT_USING_OV02k10 is not set */
/* RT_USING_GC2053 is not set */
/* RT_USING_F37 is not set */
#define RT_USING_SC230AI

/* RT-Thread rockchip vicap_lite driver */

/* RT_USING_VICAP_LITE is not set */

/* RT-Thread rockchip csi2host driver */

/* RT_USING_CSI2HOST is not set */

/* RT-Thread rockchip buffer_manage driver */

/* RT_USING_BUFFER_MANAGE is not set */
/* RT_USING_SENSOR_HUB is not set */
/* RT_USING_QPIPSRAM is not set */

/* RT-Thread rockchip coredump driver */

/* RT_USING_COREDUMP is not set */
/* RT_USING_SPINAND is not set */

/* Enable PSTORE */

/* RT_USING_PSTORE is not set */

/* RT-Thread rockchip rv1106 drivers */

#define RT_USING_CRU

/* Enable I2C */

/* RT_USING_I2C0 is not set */
/* RT_USING_I2C1 is not set */
/* RT_USING_I2C2 is not set */
#define RT_USING_I2C4
/* RT_USING_I2C5 is not set */
#define RT_USING_PWM0
#define RT_USING_PWM1
#define RT_USING_PWM2
#define RT_USING_SARADC

/* Enable UART */

#define RT_USING_UART
/* RT_USING_UART0 is not set */
#define RT_USING_UART2

/* RT-Thread Common Test case */

/* RT_USING_TC is not set */
/* RT_USING_FILE_TEST is not set */
/* RT_USING_COMMON_TEST is not set */

/* RT-Thread application */

/* RT_USING_LVGL_APP_DEMO is not set */
/* RT_USING_ROCKCHIP_DEMO is not set */
#define RT_USING_RK_BATTERY_IPC
/* RT_USING_RK_IOT_APP is not set */
/* ENABLE_DUAL_TRACKING is not set */

#endif
