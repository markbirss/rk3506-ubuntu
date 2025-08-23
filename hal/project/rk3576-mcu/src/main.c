/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2023 Rockchip Electronics Co., Ltd.
 */

#include "hal_bsp.h"
#include "hal_base.h"

/********************* Private MACRO Definition ******************************/
//#define TEST_DEMO
#define TEST_USE_UART5M2

/********************* Private Structure Definition **************************/

/********************* Private Variable Definition ***************************/

/********************* Private Function Definition ***************************/

#ifdef TEST_USE_UART5M2
static void HAL_IOMUX_Uart5M2Config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK2,
                         GPIO_PIN_A4,
                         PIN_CONFIG_MUX_FUNC9);
    HAL_PINCTRL_SetIOMUX(GPIO_BANK2,
                         GPIO_PIN_A5,
                         PIN_CONFIG_MUX_FUNC9);
}
#endif

/* TODO: wait for CRU driver */
static void HAL_CRU_Init_Temp()
{
    CRU->CLKSEL_CON[64] = 0x00ff0000;
}

/********************* Public Function Definition ****************************/
#ifdef TEST_USE_UART5M2
static struct UART_REG *pUart = UART5;
#else
static struct UART_REG *pUart = UART0;
#endif

#ifdef __GNUC__
__USED int _write(int fd, char *ptr, int len)
{
    int i = 0;

    /*
     * write "len" of char from "ptr" to file id "fd"
     * Return number of char written.
     *
    * Only work for STDOUT, STDIN, and STDERR
     */
    if (fd > 2) {
        return -1;
    }

    while (*ptr && (i < len)) {
        if (*ptr == '\n') {
            HAL_UART_SerialOutChar(pUart, '\r');
        }
        HAL_UART_SerialOutChar(pUart, *ptr);

        i++;
        ptr++;
    }

    return i;
}

#ifdef HAL_PMU_MCU_CORE
__USED int _read(void)
{
    return -1;
}

__USED int _sbrk(void)
{
    return -1;
}

__USED int _close(void)
{
    return -1;
}

__USED int _fstat(void)
{
    return -1;
}

__USED int _isatty(void)
{
    return -1;
}

__USED int _lseek(void)
{
    return -1;
}

__USED int _exit(void)
{
    return -1;
}

__USED int _kill(void)
{
    return -1;
}

__USED int _getpid(void)
{
    return -1;
}
#endif

#else
int fputc(int ch, FILE *f)
{
    if (ch == '\n') {
        HAL_UART_SerialOutChar(pUart, '\r');
    }

    HAL_UART_SerialOutChar(pUart, (char)ch);

    return 0;
}
#endif

int main(void)
{
    struct HAL_UART_CONFIG hal_uart_config = {
        .baudRate = UART_BR_1500000,
        .dataBit = UART_DATA_8B,
        .stopBit = UART_ONE_STOPBIT,
        .parity = UART_PARITY_DISABLE,
    };

    /* HAL BASE Init */
    HAL_Init();

    /* BSP Init */
    BSP_Init();

    /* TODO: wait for CRU driver */
    HAL_CRU_Init_Temp();

    /* UART Init */
#ifdef TEST_USE_UART5M2
    HAL_IOMUX_Uart5M2Config();
    HAL_UART_Init(&g_uart5Dev, &hal_uart_config);
#else
    HAL_UART_Init(&g_uart0Dev, &hal_uart_config);
#endif

#ifdef HAL_BUS_MCU_CORE
    printf("Hello RK3576 mcu\n");
#else
    HAL_UART_SerialOutChar(pUart, 'R');
    HAL_UART_SerialOutChar(pUart, 'K');
    HAL_UART_SerialOutChar(pUart, '3');
    HAL_UART_SerialOutChar(pUart, '5');
    HAL_UART_SerialOutChar(pUart, '7');
    HAL_UART_SerialOutChar(pUart, '6');
    HAL_UART_SerialOutChar(pUart, ' ');
    HAL_UART_SerialOutChar(pUart, 'm');
    HAL_UART_SerialOutChar(pUart, 'c');
    HAL_UART_SerialOutChar(pUart, 'u');
#endif

#ifdef TEST_DEMO
    test_demo();
#endif

    while (1) {
        ;
    }
}

int entry(void)
{
    return main();
}
