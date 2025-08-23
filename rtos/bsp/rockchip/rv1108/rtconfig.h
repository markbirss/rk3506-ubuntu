#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Configuration */

#define SOC_RV1108

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
#define IDLE_THREAD_STACK_SIZE 256
/* RT_USING_TIMER_SOFT is not set */
#define RT_DEBUG
#define RT_DEBUG_COLOR
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
/* RT_USING_DEVICE_OPS is not set */
/* RT_USING_INTERRUPT_INFO is not set */
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 128
#define RT_CONSOLE_DEVICE_NAME "uart2"
#define RT_VER_NUM 0x30103
#define ARCH_ARM
#define ARCH_ARM_CORTEX_A
#define ARCH_ARM_CORTEX_A7
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
/* FINSH_USING_MSH_ONLY is not set */
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
/* RT_USING_USB_DEVICE is not set */

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

/* RT_USING_CMBACKTRACE is not set */
/* RT_USING_LOGTRACE is not set */
/* RT_USING_RYM is not set */
/* RT_USING_RZM is not set */
/* RT_USING_ULOG is not set */
/* RT_USING_UTEST is not set */

/* RT-Thread Benchmarks */

/* RT_USING_BENCHMARK is not set */

/* Bluetooth */

/* BT_ENABLED is not set */

/* examples bluetooth */

/* Bluetooth examlpes */

/* Example 'BT API TEST' Config */

/* BT_API_TEST is not set */

/* Example 'BT DISCOVERY' Config */

/* BT_DISCOVERY_EXAMPLES is not set */

/* Example 'A2DP SINK' Config */

/* A2DP_SINK_EXAMPLE is not set */

/* Example 'HFP CLIENT' Config */

/* HFP_CLIENT_EXAMPLE is not set */
/* RT_USING_PLAYER is not set */

/* RT-Thread application */

/* RT_USING_OLPC_DEMO is not set */

/* RT-Thread online packages */

/* IoT - internet of things */

/* PKG_USING_PAHOMQTT is not set */
/* PKG_USING_WEBCLIENT is not set */
/* PKG_USING_WEBNET is not set */
/* PKG_USING_MONGOOSE is not set */
/* PKG_USING_WEBTERMINAL is not set */
/* PKG_USING_CJSON is not set */
/* PKG_USING_JSMN is not set */
/* PKG_USING_LJSON is not set */
/* PKG_USING_EZXML is not set */
/* PKG_USING_NANOPB is not set */

/* Wi-Fi */

/* Marvell WiFi */

/* PKG_USING_WLANMARVELL is not set */

/* Wiced WiFi */

/* PKG_USING_WLAN_WICED is not set */
/* PKG_USING_RW007 is not set */
/* PKG_USING_COAP is not set */
/* PKG_USING_NOPOLL is not set */
/* PKG_USING_NETUTILS is not set */
/* PKG_USING_AT_DEVICE is not set */
/* PKG_USING_WIZNET is not set */

/* IoT Cloud */

/* PKG_USING_ONENET is not set */
/* PKG_USING_GAGENT_CLOUD is not set */
/* PKG_USING_ALI_IOTKIT is not set */
/* PKG_USING_AZURE is not set */
/* PKG_USING_TENCENT_IOTKIT is not set */
/* PKG_USING_NIMBLE is not set */

/* security packages */

/* PKG_USING_MBEDTLS is not set */
/* PKG_USING_libsodium is not set */
/* PKG_USING_TINYCRYPT is not set */

/* language packages */

/* PKG_USING_LUA is not set */
/* PKG_USING_JERRYSCRIPT is not set */
/* PKG_USING_MICROPYTHON is not set */

/* multimedia packages */

/* PKG_USING_OPENMV is not set */
/* PKG_USING_MUPDF is not set */

/* tools packages */

/* PKG_USING_CMBACKTRACE is not set */
/* PKG_USING_EASYFLASH is not set */
/* PKG_USING_EASYLOGGER is not set */
/* PKG_USING_SYSTEMVIEW is not set */
/* PKG_USING_RDB is not set */
/* PKG_USING_QRCODE is not set */
/* PKG_USING_ULOG_EASYFLASH is not set */
/* PKG_USING_ADBD is not set */

/* system packages */

/* PKG_USING_GUIENGINE is not set */
/* PKG_USING_PERSIMMON is not set */
/* PKG_USING_CAIRO is not set */
/* PKG_USING_PIXMAN is not set */
/* PKG_USING_LWEXT4 is not set */
/* PKG_USING_PARTITION is not set */
/* PKG_USING_FAL is not set */
/* PKG_USING_SQLITE is not set */
/* PKG_USING_RTI is not set */
/* PKG_USING_LITTLEVGL2RTT is not set */
/* PKG_USING_CMSIS is not set */
/* PKG_USING_DFS_YAFFS is not set */
/* PKG_USING_LITTLEFS is not set */

/* peripheral libraries and drivers */

/* sensors drivers */

/* PKG_USING_LSM6DSL is not set */
/* PKG_USING_LPS22HB is not set */
/* PKG_USING_HTS221 is not set */
/* PKG_USING_LSM303AGR is not set */
/* PKG_USING_BME280 is not set */
/* PKG_USING_BMA400 is not set */
/* PKG_USING_REALTEK_AMEBA is not set */
/* PKG_USING_SHT2X is not set */
/* PKG_USING_AHT10 is not set */
/* PKG_USING_AP3216C is not set */
/* PKG_USING_STM32_SDIO is not set */
/* PKG_USING_ICM20608 is not set */
/* PKG_USING_U8G2 is not set */
/* PKG_USING_BUTTON is not set */
/* PKG_USING_MPU6XXX is not set */
/* PKG_USING_PCF8574 is not set */
/* PKG_USING_KENDRYTE_SDK is not set */

/* miscellaneous packages */

/* PKG_USING_LIBCSV is not set */
/* PKG_USING_OPTPARSE is not set */
/* PKG_USING_FASTLZ is not set */
/* PKG_USING_MINILZO is not set */
/* PKG_USING_QUICKLZ is not set */
/* PKG_USING_MULTIBUTTON is not set */
/* PKG_USING_CANFESTIVAL is not set */
/* PKG_USING_ZLIB is not set */
/* PKG_USING_DSTR is not set */
/* PKG_USING_TINYFRAME is not set */
/* PKG_USING_KENDRYTE_DEMO is not set */

/* samples: kernel and components samples */

/* PKG_USING_KERNEL_SAMPLES is not set */
/* PKG_USING_FILESYSTEM_SAMPLES is not set */
/* PKG_USING_NETWORK_SAMPLES is not set */
/* PKG_USING_PERIPHERAL_SAMPLES is not set */
/* PKG_USING_HELLO is not set */
/* PKG_USING_VI is not set */

/* RT-Thread board config */

#define RT_USING_BOARD_RV1108_EVB

/* RT-Thread rockchip common drivers */

/* RT_USING_SND_GLB_RST is not set */
/* RT_USING_RESET is not set */
/* RT_USING_CRU_DUMP is not set */
/* RT_USING_REGULATOR_DUMP is not set */
/* RT_USING_PMIC is not set */
/* RT_USING_UNCACHE_HEAP is not set */
/* RT_USING_LARGE_HEAP is not set */
#define RT_USING_SNOR
/* RT_USING_SNOR_FSPI_HOST is not set */
#define RT_USING_SNOR_SFC_HOST
/* RT_USING_SNOR_SPI_HOST is not set */

/* RT-Thread rockchip pm drivers */

/* RT_USING_PM_DVFS is not set */
/* RT_USING_PM_REQ_PWR is not set */
/* RT_USING_PM_REQ_CLK is not set */
/* RT_USING_DVFS_DBG is not set */

/* RT-Thread rockchip rv1108 drivers */

#define RT_USING_CRU
/* RT_USING_DMA is not set */
/* RT_USING_VOP is not set */
/* RT_USING_DSI is not set */
/* RT_USING_DSP is not set */

/* Enable UART */

#define RT_USING_UART
/* RT_USING_UART0 is not set */
/* RT_USING_UART1 is not set */
#define RT_USING_UART2

/* RT-Thread bsp test case */

/* RT_USING_TC is not set */
/* RT_USING_FILE_TEST is not set */
/* RT_USING_PRIVATE_TEST is not set */

/* RT-Thread Common Test case */

/* RT_USING_COMMON_TEST is not set */

#endif
