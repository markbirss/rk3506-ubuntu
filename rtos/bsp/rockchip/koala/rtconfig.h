#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Configuration */

#define RKMCU_KOALA

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
#define RT_CONSOLE_DEVICE_NAME "uart0"
#define RT_VER_NUM 0x30103
#define ARCH_ARM
#define ARCH_ARM_CORTEX_M
#define ARCH_ARM_CORTEX_M3
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

/* RT_USING_DFS is not set */

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_PIPE_BUFSZ 64
#define RT_USING_SERIAL
#define RT_SERIAL_USING_DMA
#define RT_SERIAL_RB_BUFSZ 64
/* RT_USING_CAN is not set */
/* RT_USING_HWTIMER is not set */
/* RT_USING_CPUTIME is not set */
#define RT_USING_I2C
#define RT_USING_I2C_BITOPS
#define RT_USING_PIN
/* RT_USING_ADC is not set */
/* RT_USING_PWM is not set */
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

#define RT_USING_CMBACKTRACE
/* PKG_CMBACKTRACE_PLATFORM_M0_M0PLUS is not set */
#define PKG_CMBACKTRACE_PLATFORM_M3
/* PKG_CMBACKTRACE_PLATFORM_M4 is not set */
/* PKG_CMBACKTRACE_PLATFORM_M7 is not set */
/* PKG_CMBACKTRACE_PLATFORM_NOT_SELECTED is not set */
#define PKG_CMBACKTRACE_DUMP_STACK
#define PKG_CMBACKTRACE_PRINT_ENGLISH
/* PKG_CMBACKTRACE_PRINT_CHINESE is not set */
/* PKG_CMBACKTRACE_FAULT_DUMP_TO_FLASH is not set */
/* RT_USING_LOGTRACE is not set */
/* RT_USING_RYM is not set */
/* RT_USING_ULOG is not set */
/* RT_USING_UTEST is not set */

/* ARM CMSIS */

/* RT_USING_CMSIS_OS is not set */
/* RT_USING_RTT_CMSIS is not set */

/* RT-Thread Benchmarks */

/* RT_USING_BENCHMARK is not set */
/* RT_USING_NET_HTTP is not set */
/* RT_USING_OTA is not set */
/* RT_USING_AUPIPE is not set */

/* RT-Thread application */

/* RT_USING_LVGL_APP_DEMO is not set */
/* RT_USING_ROCKCHIP_DEMO is not set */
/* RT_USING_RK_IOT_APP is not set */
/* ENABLE_DUAL_TRACKING is not set */

/* RT-Thread online packages */

/* IoT - internet of things */

/* PKG_USING_LORAWAN_DRIVER is not set */
/* PKG_USING_PAHOMQTT is not set */
/* PKG_USING_UMQTT is not set */
/* PKG_USING_WEBCLIENT is not set */
/* PKG_USING_WEBNET is not set */
/* PKG_USING_MONGOOSE is not set */
/* PKG_USING_MYMQTT is not set */
/* PKG_USING_KAWAII_MQTT is not set */
/* PKG_USING_BC28_MQTT is not set */
/* PKG_USING_WEBTERMINAL is not set */
/* PKG_USING_CJSON is not set */
/* PKG_USING_JSMN is not set */
/* PKG_USING_LIBMODBUS is not set */
/* PKG_USING_FREEMODBUS is not set */
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
/* PKG_USING_CMUX is not set */
/* PKG_USING_PPP_DEVICE is not set */
/* PKG_USING_AT_DEVICE is not set */
/* PKG_USING_ATSRV_SOCKET is not set */
/* PKG_USING_WIZNET is not set */

/* IoT Cloud */

/* PKG_USING_ONENET is not set */
/* PKG_USING_GAGENT_CLOUD is not set */
/* PKG_USING_ALI_IOTKIT is not set */
/* PKG_USING_AZURE is not set */
/* PKG_USING_TENCENT_IOT_EXPLORER is not set */
/* PKG_USING_JIOT-C-SDK is not set */
/* PKG_USING_UCLOUD_IOT_SDK is not set */
/* PKG_USING_JOYLINK is not set */
/* PKG_USING_NIMBLE is not set */
/* PKG_USING_OTA_DOWNLOADER is not set */
/* PKG_USING_IPMSG is not set */
/* PKG_USING_LSSDP is not set */
/* PKG_USING_AIRKISS_OPEN is not set */
/* PKG_USING_LIBRWS is not set */
/* PKG_USING_TCPSERVER is not set */
/* PKG_USING_PROTOBUF_C is not set */
/* PKG_USING_ONNX_PARSER is not set */
/* PKG_USING_ONNX_BACKEND is not set */
/* PKG_USING_DLT645 is not set */
/* PKG_USING_QXWZ is not set */
/* PKG_USING_SMTP_CLIENT is not set */
/* PKG_USING_ABUP_FOTA is not set */
/* PKG_USING_LIBCURL2RTT is not set */
/* PKG_USING_CAPNP is not set */
/* PKG_USING_RT_CJSON_TOOLS is not set */
/* PKG_USING_AGILE_TELNET is not set */
/* PKG_USING_NMEALIB is not set */
/* PKG_USING_AGILE_JSMN is not set */
/* PKG_USING_PDULIB is not set */
/* PKG_USING_BTSTACK is not set */
/* PKG_USING_LORAWAN_ED_STACK is not set */
/* PKG_USING_WAYZ_IOTKIT is not set */

/* security packages */

/* PKG_USING_MBEDTLS is not set */
/* PKG_USING_libsodium is not set */
/* PKG_USING_TINYCRYPT is not set */
/* PKG_USING_TFM is not set */
/* PKG_USING_YD_CRYPTO is not set */

/* language packages */

/* PKG_USING_LUA is not set */
/* PKG_USING_JERRYSCRIPT is not set */
/* PKG_USING_MICROPYTHON is not set */

/* multimedia packages */

/* PKG_USING_OPENMV is not set */
/* PKG_USING_MUPDF is not set */
/* PKG_USING_STEMWIN is not set */
/* PKG_USING_WAVPLAYER is not set */
/* PKG_USING_TJPGD is not set */
/* PKG_USING_HELIX is not set */
/* PKG_USING_AZUREGUIX is not set */
/* PKG_USING_TOUCHGFX2RTT is not set */

/* tools packages */

/* PKG_USING_CMBACKTRACE is not set */
/* PKG_USING_EASYFLASH is not set */
/* PKG_USING_EASYLOGGER is not set */
/* PKG_USING_SYSTEMVIEW is not set */
/* PKG_USING_RDB is not set */
/* PKG_USING_QRCODE is not set */
/* PKG_USING_ULOG_EASYFLASH is not set */
/* PKG_USING_ULOG_FILE is not set */
/* PKG_USING_LOGMGR is not set */
/* PKG_USING_ADBD is not set */
/* PKG_USING_COREMARK is not set */
/* PKG_USING_DHRYSTONE is not set */
/* PKG_USING_MEMORYPERF is not set */
/* PKG_USING_NR_MICRO_SHELL is not set */
/* PKG_USING_CHINESE_FONT_LIBRARY is not set */
/* PKG_USING_LUNAR_CALENDAR is not set */
/* PKG_USING_BS8116A is not set */
/* PKG_USING_GPS_RMC is not set */
/* PKG_USING_URLENCODE is not set */
/* PKG_USING_UMCN is not set */
/* PKG_USING_LWRB2RTT is not set */
/* PKG_USING_CPU_USAGE is not set */
/* PKG_USING_GBK2UTF8 is not set */
/* PKG_USING_VCONSOLE is not set */
/* PKG_USING_KDB is not set */
/* PKG_USING_WAMR is not set */

/* system packages */

/* PKG_USING_GUIENGINE is not set */
/* PKG_USING_CAIRO is not set */
/* PKG_USING_PIXMAN is not set */
/* PKG_USING_LWEXT4 is not set */
/* PKG_USING_PARTITION is not set */
/* PKG_USING_FAL is not set */
/* PKG_USING_FLASHDB is not set */
/* PKG_USING_SQLITE is not set */
/* PKG_USING_RTI is not set */
/* PKG_USING_LITTLEVGL2RTT is not set */
/* PKG_USING_CMSIS is not set */
/* PKG_USING_DFS_YAFFS is not set */
/* PKG_USING_LITTLEFS is not set */
/* PKG_USING_THREAD_POOL is not set */
/* PKG_USING_ROBOTS is not set */
/* PKG_USING_EV is not set */
/* PKG_USING_SYSWATCH is not set */
/* PKG_USING_SYS_LOAD_MONITOR is not set */
/* PKG_USING_PLCCORE is not set */
/* PKG_USING_RAMDISK is not set */
/* PKG_USING_MININI is not set */
/* PKG_USING_QBOOT is not set */

/* Micrium: Micrium software products porting for RT-Thread */

/* PKG_USING_UCOSIII_WRAPPER is not set */
/* PKG_USING_UCOSII_WRAPPER is not set */
/* PKG_USING_UC_CRC is not set */
/* PKG_USING_UC_CLK is not set */
/* PKG_USING_UC_COMMON is not set */
/* PKG_USING_UC_MODBUS is not set */
/* PKG_USING_PPOOL is not set */
/* PKG_USING_OPENAMP is not set */
/* PKG_USING_RT_PRINTF is not set */

/* peripheral libraries and drivers */

/* PKG_USING_SENSORS_DRIVERS is not set */
/* PKG_USING_REALTEK_AMEBA is not set */
/* PKG_USING_SHT2X is not set */
/* PKG_USING_SHT3X is not set */
/* PKG_USING_AS7341 is not set */
/* PKG_USING_STM32_SDIO is not set */
/* PKG_USING_ICM20608 is not set */
/* PKG_USING_U8G2 is not set */
/* PKG_USING_BUTTON is not set */
/* PKG_USING_PCF8574 is not set */
/* PKG_USING_SX12XX is not set */
/* PKG_USING_SIGNAL_LED is not set */
/* PKG_USING_LEDBLINK is not set */
/* PKG_USING_LITTLED is not set */
/* PKG_USING_LKDGUI is not set */
/* PKG_USING_NRF5X_SDK is not set */
/* PKG_USING_NRFX is not set */
/* PKG_USING_WM_LIBRARIES is not set */
/* PKG_USING_KENDRYTE_SDK is not set */
/* PKG_USING_INFRARED is not set */
/* PKG_USING_ROSSERIAL is not set */
/* PKG_USING_AGILE_BUTTON is not set */
/* PKG_USING_AGILE_LED is not set */
/* PKG_USING_AT24CXX is not set */
/* PKG_USING_MOTIONDRIVER2RTT is not set */
/* PKG_USING_AD7746 is not set */
/* PKG_USING_PCA9685 is not set */
/* PKG_USING_I2C_TOOLS is not set */
/* PKG_USING_NRF24L01 is not set */
/* PKG_USING_TOUCH_DRIVERS is not set */
/* PKG_USING_MAX17048 is not set */
/* PKG_USING_RPLIDAR is not set */
/* PKG_USING_AS608 is not set */
/* PKG_USING_RC522 is not set */
/* PKG_USING_WS2812B is not set */
/* PKG_USING_EMBARC_BSP is not set */
/* PKG_USING_EXTERN_RTC_DRIVERS is not set */
/* PKG_USING_MULTI_RTIMER is not set */
/* PKG_USING_MAX7219 is not set */
/* PKG_USING_BEEP is not set */
/* PKG_USING_EASYBLINK is not set */
/* PKG_USING_PMS_SERIES is not set */
/* PKG_USING_CAN_YMODEM is not set */
/* PKG_USING_LORA_RADIO_DRIVER is not set */
/* PKG_USING_QLED is not set */
/* PKG_USING_PAJ7620 is not set */
/* PKG_USING_AGILE_CONSOLE is not set */
/* PKG_USING_LD3320 is not set */
/* PKG_USING_WK2124 is not set */
/* PKG_USING_LY68L6400 is not set */
/* PKG_USING_DM9051 is not set */
/* PKG_USING_SSD1306 is not set */
/* PKG_USING_QKEY is not set */
/* PKG_USING_RS485 is not set */
/* PKG_USING_NES is not set */
/* PKG_USING_VIRTUAL_SENSOR is not set */
/* PKG_USING_VDEVICE is not set */
/* PKG_USING_SGM706 is not set */

/* miscellaneous packages */

/* PKG_USING_LIBCSV is not set */
/* PKG_USING_OPTPARSE is not set */
/* PKG_USING_FASTLZ is not set */
/* PKG_USING_MINILZO is not set */
/* PKG_USING_QUICKLZ is not set */
/* PKG_USING_LZMA is not set */
/* PKG_USING_MULTIBUTTON is not set */
/* PKG_USING_FLEXIBLE_BUTTON is not set */
/* PKG_USING_CANFESTIVAL is not set */
/* PKG_USING_ZLIB is not set */
/* PKG_USING_DSTR is not set */
/* PKG_USING_TINYFRAME is not set */
/* PKG_USING_KENDRYTE_DEMO is not set */
/* PKG_USING_DIGITALCTRL is not set */
/* PKG_USING_UPACKER is not set */
/* PKG_USING_UPARAM is not set */

/* samples: kernel and components samples */

/* PKG_USING_KERNEL_SAMPLES is not set */
/* PKG_USING_FILESYSTEM_SAMPLES is not set */
/* PKG_USING_NETWORK_SAMPLES is not set */
/* PKG_USING_PERIPHERAL_SAMPLES is not set */
/* PKG_USING_HELLO is not set */
/* PKG_USING_VI is not set */
/* PKG_USING_KI is not set */
/* PKG_USING_NNOM is not set */
/* PKG_USING_LIBANN is not set */
/* PKG_USING_ELAPACK is not set */
/* PKG_USING_ARMv7M_DWT is not set */
/* PKG_USING_VT100 is not set */
/* PKG_USING_ULAPACK is not set */
/* PKG_USING_UKAL is not set */
/* PKG_USING_CRCLIB is not set */

/* games: games run on RT-Thread console */

/* PKG_USING_THREES is not set */
/* PKG_USING_2048 is not set */
/* PKG_USING_SNAKE is not set */
/* PKG_USING_TETRIS is not set */
/* PKG_USING_LWGPS is not set */
/* PKG_USING_TENSORFLOWLITEMICRO is not set */
/* PKG_USING_STATE_MACHINE is not set */

/* RT-Thread board config */

#define RT_USING_BOARD_KOALA_EVB

/* RT-Thread rockchip common drivers */

/* HAL_DBG_ON is not set */
/* RT_USING_SND_GLB_RST is not set */
/* RT_USING_RESET is not set */
#define RT_USING_CRU_DUMP
/* RT_USING_REGULATOR_DUMP is not set */
/* RT_USING_PROF_IRQ is not set */
#define RT_USING_PMIC
/* RT_USING_PMIC_RK817 is not set */
/* RT_USING_PMIC_RK816 is not set */
#define RT_USING_CACHE
/* RT_USING_UNCACHE_HEAP is not set */
/* RT_USING_LARGE_HEAP is not set */
/* RT_USING_DTCM_HEAP is not set */
/* RT_USING_PSRAM_HEAP is not set */

/* Enable Fault Dump Hook */

/* RT_USNING_FAULT_DUMP_HOOK is not set */
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
/* RT_USING_ISP3 is not set */

/* RT-Thread rockchip vcm driver */

/* RT_USING_VCM is not set */

/* RT-Thread rockchip vicap driver */

/* RT_USING_VICAP is not set */

/* RT-Thread rockchip camera driver */

/* RT_USING_CAMERA is not set */

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

/* RT-Thread rockchip koala drivers */

#define RT_USING_CRU
#define RT_USING_DMA
#define RT_USING_DMA_PL330
#define RT_USING_DMA0

/* Enable DSP */

/* RT_USING_DSP is not set */

/* Enable UART */

#define RT_USING_UART
#define RT_USING_UART0
/* RT_USING_UART1 is not set */

/* Enable I2C */

/* RT_USING_I2C0 is not set */
/* RT_USING_I2C1 is not set */
#define RT_USING_I2C2

/* RT-Thread bsp test case */

/* RT_USING_TC is not set */
/* RT_USING_PRIVATE_TEST is not set */

/* RT-Thread Common Test case */

/* RT_USING_FILE_TEST is not set */
/* RT_USING_COMMON_TEST is not set */

#endif
