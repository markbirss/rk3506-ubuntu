/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_bsp.h"
#include "hal_base.h"

/********************* Private MACRO Definition ******************************/
//#define TEST_DEMO
#define TEST_USE_UART5M2

/********************* Private Structure Definition **************************/

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

#ifdef HAL_GIC_MODULE_ENABLED
static struct GIC_AMP_IRQ_INIT_CFG irqsConfig[] = {
/* TODO: Config the irqs here.
 * GIC version: GICv2
 * Default: CPU0
 */
    //GIC_AMP_IRQ_CFG_ROUTE(UART5_IRQn, 0xd0, CPU_GET_AFFINITY(3, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(0, 0, 0),   /* sentinel */
};

static struct GIC_IRQ_AMP_CTRL irqConfig = {
    .cpuAff = CPU_GET_AFFINITY(0, 0),
    .defPrio = 0xd0,
    .defRouteAff = CPU_GET_AFFINITY(0, 0),
    .irqsCfg = &irqsConfig[0],
};
#endif

/********************* Private Variable Definition ***************************/

/* TODO: By default, UART2 is used for master core, and UART5 is used for remote core */
#ifdef TEST_USE_UART5M2
static struct UART_REG *pUart = UART5;
#else
static struct UART_REG *pUart = UART0;
#endif

/********************* Private Function Definition ***************************/

/********************* Public Function Definition ****************************/

#ifdef __GNUC__
int _write(int fd, char *ptr, int len);
#else
int fputc(int ch, FILE *f);
#endif

#ifdef __GNUC__
HAL_VISIBLE int _write(int fd, char *ptr, int len)
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
#else
HAL_VISIBLE int fputc(int ch, FILE *f)
{
    if (ch == '\n') {
        HAL_UART_SerialOutChar(pUart, '\r');
    }

    HAL_UART_SerialOutChar(pUart, (char)ch);

    return 0;
}
#endif

static void test_irq_isr(int vector, void *param)
{
    printf("enter irq, vector = %d\n", vector);
    HAL_GIC_ClearPending(vector);
    printf("leave irq, vector = %d\n", vector);
}

static void test_irq(int vector)
{
    HAL_IRQ_HANDLER_SetIRQHandler(vector, test_irq_isr, NULL);
    HAL_GIC_Enable(vector);
    HAL_GIC_SetPending(vector);
}

void main(void)
{
    uint32_t cpu_id, irq;

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

#ifdef TEST_DEMO
    TEST_DEMO_GIC_Init();
#else
    /* Interrupt Init */
    HAL_GIC_Init(&irqConfig);
#endif

#ifdef TEST_USE_UART5M2
    HAL_IOMUX_Uart5M2Config();
    HAL_UART_Init(&g_uart5Dev, &hal_uart_config);
#else
    HAL_UART_Init(&g_uart0Dev, &hal_uart_config);
#endif

    cpu_id = HAL_CPU_TOPOLOGY_GetCurrentCpuId();

    HAL_DBG(" CPU%d(aarch64) Initial OK!\n", cpu_id);

    printf("\n");
    printf("****************************************\n");
    printf("  Hello RK3576 Bare-metal using RK_HAL! \n");
    printf("       Rockchip Electronics Co.Ltd      \n");
    printf("              CPI_ID(%d)                \n", HAL_CPU_TOPOLOGY_GetCurrentCpuId());
    printf("****************************************\n");
    printf(" CPU(%d) Initial OK!\n", HAL_CPU_TOPOLOGY_GetCurrentCpuId());
    printf("\n");
    test_irq(GPIO0_IRQn);

#ifdef TEST_DEMO
    test_demo();
#endif

    while (1) {
        /* TODO: Message loop */

        /* Enter cpu idle when no message */
        HAL_CPU_EnterIdle();
    }
}

HAL_VISIBLE void _start(void)
{
    main();
}