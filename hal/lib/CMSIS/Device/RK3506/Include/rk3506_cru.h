/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#ifndef __RK3506_CRU_H
#define __RK3506_CRU_H

// ========================= CRU module definition bank=0 =========================
// CRU_SOFTRST_CON00(Offset:0xA00)
#define SRST_NCOREPORESET0_AC                    0x00000000
#define SRST_NCOREPORESET1_AC                    0x00000001
#define SRST_NCOREPORESET2_AC                    0x00000002
#define SRST_NCORESET0_AC                        0x00000004
#define SRST_NCORESET1_AC                        0x00000005
#define SRST_NCORESET2_AC                        0x00000006
#define SRST_NL2RESET_AC                         0x00000008
#define SRST_ARESETN_CORE_BIU_AC                 0x00000009
#define SRST_HRESETN_M0_AC                       0x0000000A

// CRU_SOFTRST_CON02(Offset:0xA08)
#define SRST_NDBGRESET                           0x0000002A
#define SRST_PRESETN_CORE_BIU                    0x0000002E
#define SRST_RESETN_PMU                          0x0000002F

// CRU_SOFTRST_CON03(Offset:0xA0C)
#define SRST_PRESETN_DBG                         0x00000031
#define SRST_POTRESETN_DBG                       0x00000032
#define SRST_PRESETN_CORE_GRF                    0x00000034
#define SRST_RESETN_CORE_EMA_DETECT              0x00000036
#define SRST_RESETN_REF_PVTPLL_CORE              0x00000037
#define SRST_PRESETN_GPIO1                       0x00000038
#define SRST_DBRESETN_GPIO1                      0x00000039

// CRU_SOFTRST_CON04(Offset:0xA10)
#define SRST_ARESETN_CORE_PERI_BIU               0x00000043
#define SRST_ARESETN_DSMC                        0x00000045
#define SRST_PRESETN_DSMC                        0x00000046
#define SRST_RESETN_FLEXBUS                      0x00000047
#define SRST_ARESETN_FLEXBUS                     0x00000049
#define SRST_HRESETN_FLEXBUS                     0x0000004A
#define SRST_ARESETN_DSMC_SLV                    0x0000004B
#define SRST_HRESETN_DSMC_SLV                    0x0000004C
#define SRST_RESETN_DSMC_SLV                     0x0000004D

// CRU_SOFTRST_CON05(Offset:0xA14)
#define SRST_ARESETN_BUS_BIU                     0x00000053
#define SRST_HRESETN_BUS_BIU                     0x00000054
#define SRST_PRESETN_BUS_BIU                     0x00000055
#define SRST_ARESETN_SYSRAM                      0x00000056
#define SRST_HRESETN_SYSRAM                      0x00000057
#define SRST_ARESETN_DMAC0                       0x00000058
#define SRST_ARESETN_DMAC1                       0x00000059
#define SRST_HRESETN_M0                          0x0000005A
#define SRST_RESETN_M0_JTAG                      0x0000005B
#define SRST_HRESETN_CRYPTO                      0x0000005F

// CRU_SOFTRST_CON06(Offset:0xA18)
#define SRST_HRESETN_RNG                         0x00000060
#define SRST_PRESETN_BUS_GRF                     0x00000061
#define SRST_PRESETN_TIMER0                      0x00000062
#define SRST_RESETN_TIMER0_CH0                   0x00000063
#define SRST_RESETN_TIMER0_CH1                   0x00000064
#define SRST_RESETN_TIMER0_CH2                   0x00000065
#define SRST_RESETN_TIMER0_CH3                   0x00000066
#define SRST_RESETN_TIMER0_CH4                   0x00000067
#define SRST_RESETN_TIMER0_CH5                   0x00000068
#define SRST_PRESETN_WDT0                        0x00000069
#define SRST_TRESETN_WDT0                        0x0000006A
#define SRST_PRESETN_WDT1                        0x0000006B
#define SRST_TRESETN_WDT1                        0x0000006C
#define SRST_PRESETN_MAILBOX                     0x0000006D
#define SRST_PRESETN_INTMUX                      0x0000006E
#define SRST_PRESETN_SPINLOCK                    0x0000006F

// CRU_SOFTRST_CON07(Offset:0xA1C)
#define SRST_PRESETN_DDRC                        0x00000070
#define SRST_HRESETN_DDRPHY                      0x00000071
#define SRST_PRESETN_DDRMON                      0x00000072
#define SRST_RESETN_DDRMON_OSC                   0x00000073
#define SRST_PRESETN_DDR_LPC                     0x00000074
#define SRST_HRESETN_USBOTG0                     0x00000075
#define SRST_RESETN_USBOTG0_ADP                  0x00000077
#define SRST_HRESETN_USBOTG1                     0x00000078
#define SRST_RESETN_USBOTG1_ADP                  0x0000007A
#define SRST_PRESETN_USBPHY                      0x0000007B
#define SRST_RESETN_USBPHY_POR                   0x0000007C
#define SRST_RESETN_USBPHY_OTG0                  0x0000007D
#define SRST_RESETN_USBPHY_OTG1                  0x0000007E

// CRU_SOFTRST_CON08(Offset:0xA20)
#define SRST_ARESETN_DMA2DDR                     0x00000080
#define SRST_PRESETN_DMA2DDR                     0x00000081

// CRU_SOFTRST_CON09(Offset:0xA24)
#define SRST_RESETN_USBOTG0_UTMI                 0x00000090
#define SRST_RESETN_USBOTG1_UTMI                 0x00000091

// CRU_SOFTRST_CON10(Offset:0xA28)
#define SRST_ARESETN_DDRC_0                      0x000000A0
#define SRST_ARESETN_DDRC_1                      0x000000A1
#define SRST_ARESETN_DDR_BIU                     0x000000A2
#define SRST_RESETN_DDRC                         0x000000A3
#define SRST_RESETN_DDRMON                       0x000000A4

// CRU_SOFTRST_CON11(Offset:0xA2C)
#define SRST_HRESETN_LSPERI_BIU                  0x000000B2
#define SRST_PRESETN_UART0                       0x000000B4
#define SRST_PRESETN_UART1                       0x000000B5
#define SRST_PRESETN_UART2                       0x000000B6
#define SRST_PRESETN_UART3                       0x000000B7
#define SRST_PRESETN_UART4                       0x000000B8
#define SRST_RESETN_UART0                        0x000000B9
#define SRST_RESETN_UART1                        0x000000BA
#define SRST_RESETN_UART2                        0x000000BB
#define SRST_RESETN_UART3                        0x000000BC
#define SRST_RESETN_UART4                        0x000000BD
#define SRST_PRESETN_I2C0                        0x000000BE
#define SRST_RESETN_I2C0                         0x000000BF

// CRU_SOFTRST_CON12(Offset:0xA30)
#define SRST_PRESETN_I2C1                        0x000000C0
#define SRST_RESETN_I2C1                         0x000000C1
#define SRST_PRESETN_I2C2                        0x000000C2
#define SRST_RESETN_I2C2                         0x000000C3
#define SRST_PRESETN_PWM1                        0x000000C4
#define SRST_RESETN_PWM1                         0x000000C5
#define SRST_PRESETN_SPI0                        0x000000CA
#define SRST_RESETN_SPI0                         0x000000CB
#define SRST_PRESETN_SPI1                        0x000000CC
#define SRST_RESETN_SPI1                         0x000000CD
#define SRST_PRESETN_GPIO2                       0x000000CE
#define SRST_DBRESETN_GPIO2                      0x000000CF

// CRU_SOFTRST_CON13(Offset:0xA34)
#define SRST_PRESETN_GPIO3                       0x000000D0
#define SRST_DBRESETN_GPIO3                      0x000000D1
#define SRST_PRESETN_GPIO4                       0x000000D2
#define SRST_DBRESETN_GPIO4                      0x000000D3
#define SRST_HRESETN_CAN0                        0x000000D4
#define SRST_RESETN_CAN0                         0x000000D5
#define SRST_HRESETN_CAN1                        0x000000D6
#define SRST_RESETN_CAN1                         0x000000D7
#define SRST_HRESETN_PDM                         0x000000D8
#define SRST_MRESETN_PDM                         0x000000D9
#define SRST_RESETN_PDM                          0x000000DA
#define SRST_RESETN_SPDIFTX                      0x000000DB
#define SRST_HRESETN_SPDIFTX                     0x000000DC
#define SRST_HRESETN_SPDIFRX                     0x000000DD
#define SRST_RESETN_SPDIFRX                      0x000000DE
#define SRST_MRESETN_SAI0                        0x000000DF

// CRU_SOFTRST_CON14(Offset:0xA38)
#define SRST_HRESETN_SAI0                        0x000000E0
#define SRST_MRESETN_SAI1                        0x000000E2
#define SRST_HRESETN_SAI1                        0x000000E3
#define SRST_HRESETN_ASRC0                       0x000000E5
#define SRST_RESETN_ASRC0                        0x000000E6
#define SRST_HRESETN_ASRC1                       0x000000E7
#define SRST_RESETN_ASRC1                        0x000000E8

// CRU_SOFTRST_CON17(Offset:0xA44)
#define SRST_HRESETN_HSPERI_BIU                  0x00000114
#define SRST_HRESETN_SDMMC                       0x00000117
#define SRST_HRESETN_FSPI                        0x00000118
#define SRST_SRESETN_FSPI                        0x00000119
#define SRST_PRESETN_SPI2                        0x0000011A
#define SRST_ARESETN_MAC0                        0x0000011B
#define SRST_ARESETN_MAC1                        0x0000011C

// CRU_SOFTRST_CON18(Offset:0xA48)
#define SRST_MRESETN_SAI2                        0x00000122
#define SRST_HRESETN_SAI2                        0x00000123
#define SRST_HRESETN_SAI3                        0x00000126
#define SRST_MRESETN_SAI3                        0x00000127
#define SRST_HRESETN_SAI4                        0x0000012A
#define SRST_MRESETN_SAI4                        0x0000012B
#define SRST_HRESETN_DSM                         0x0000012C
#define SRST_MRESETN_DSM                         0x0000012D
#define SRST_PRESETN_AUDIO_ADC                   0x0000012E
#define SRST_MRESETN_AUDIO_ADC                   0x0000012F

// CRU_SOFTRST_CON19(Offset:0xA4C)
#define SRST_PRESETN_SARADC                      0x00000130
#define SRST_RESETN_SARADC                       0x00000131
#define SRST_RESETN_SARADC_PHY                   0x00000132
#define SRST_PRESETN_OTPC_NS                     0x00000133
#define SRST_RESETN_SBPI_OTPC_NS                 0x00000134
#define SRST_RESETN_USER_OTPC_NS                 0x00000135
#define SRST_PRESETN_UART5                       0x00000136
#define SRST_RESETN_UART5                        0x00000137
#define SRST_PRESETN_GPIO234_IOC                 0x00000138

// CRU_SOFTRST_CON21(Offset:0xA54)
#define SRST_ARESETN_VIO_BIU                     0x00000153
#define SRST_HRESETN_VIO_BIU                     0x00000154
#define SRST_HRESETN_RGA                         0x00000156
#define SRST_ARESETN_RGA                         0x00000157
#define SRST_RESETN_CORE_RGA                     0x00000158
#define SRST_ARESETN_VOP                         0x00000159
#define SRST_HRESETN_VOP                         0x0000015A
#define SRST_RESETN_VOP                          0x0000015B
#define SRST_PRESETN_DPHY                        0x0000015C
#define SRST_PRESETN_DSI_HOST                    0x0000015D
#define SRST_PRESETN_TSADC                       0x0000015E
#define SRST_RESETN_TSADC                        0x0000015F

// CRU_SOFTRST_CON22(Offset:0xA58)
#define SRST_PRESETN_GPIO1_IOC                   0x00000161

// CRU_GATE_CON00(Offset:0x800)
#define XIN_OSC0_FUNC_GATE                       0x00000001
#define CLK_GPLL_MUX_GATE                        0x00000002
#define CLK_V0PLL_MUX_GATE                       0x00000003
#define CLK_V1PLL_MUX_GATE                       0x00000004
#define CLK_GPLL_DIV_GATE                        0x00000005
#define CLK_GPLL_DIV_100M_GATE                   0x00000006
#define CLK_V0PLL_DIV_GATE                       0x00000007
#define CLK_V1PLL_DIV_GATE                       0x00000008
#define CLK_INT_VOICE_MATRIX0_GATE               0x00000009
#define CLK_INT_VOICE_MATRIX1_GATE               0x0000000A
#define CLK_INT_VOICE_MATRIX2_GATE               0x0000000B
#define CLK_TESTOUT_GATE                         0x0000000C
#define CLK_FRAC_UART_MATRIX0_GATE               0x0000000D
#define CLK_FRAC_UART_MATRIX1_GATE               0x0000000E
#define CLK_FRAC_VOICE_MATRIX0_GATE              0x0000000F

// CRU_GATE_CON01(Offset:0x804)
#define CLK_FRAC_VOICE_MATRIX1_GATE              0x00000010
#define CLK_FRAC_COMMON_MATRIX0_GATE             0x00000011
#define CLK_FRAC_COMMON_MATRIX1_GATE             0x00000012
#define CLK_FRAC_COMMON_MATRIX2_GATE             0x00000013
#define CLK_REF_SRC_USBPHY_GATE                  0x00000014
#define CLK_REF_SRC_DPHY_GATE                    0x00000015

// CRU_GATE_CON02(Offset:0x808)
#define CLK_CORE_DIV_GATE                        0x00000020
#define CLK_CORE_GATE                            0x00000021
#define ACLK_CORE_ROOT_GATE                      0x0000002B
#define PCLK_CORE_ROOT_GATE                      0x0000002C
#define ACLK_CORE_BIU_GATE                       0x0000002D
#define PCLK_CORE_BIU_GATE                       0x0000002E

// CRU_GATE_CON03(Offset:0x80C)
#define PCLK_DBG_GATE                            0x00000031
#define SWCLKTCK_GATE                            0x00000032
#define PCLK_CORE_GRF_GATE                       0x00000034
#define CLK_CORE_EMA_DETECT_GATE                 0x00000036
#define CLK_REF_PVTPLL_CORE_GATE                 0x00000037
#define PCLK_GPIO1_GATE                          0x00000038
#define DBCLK_GPIO1_GATE                         0x00000039

// CRU_GATE_CON04(Offset:0x810)
#define ACLK_CORE_PERI_ROOT_GATE                 0x00000040
#define HCLK_CORE_PERI_ROOT_GATE                 0x00000041
#define PCLK_CORE_PERI_ROOT_GATE                 0x00000042
#define ACLK_CORE_PERI_BIU_GATE                  0x00000043
#define CLK_DSMC_GATE                            0x00000044
#define ACLK_DSMC_GATE                           0x00000045
#define PCLK_DSMC_GATE                           0x00000046
#define CLK_FLEXBUS_TX_GATE                      0x00000047
#define CLK_FLEXBUS_RX_GATE                      0x00000048
#define ACLK_FLEXBUS_GATE                        0x00000049
#define HCLK_FLEXBUS_GATE                        0x0000004A
#define ACLK_DSMC_SLV_GATE                       0x0000004B
#define HCLK_DSMC_SLV_GATE                       0x0000004C

// CRU_GATE_CON05(Offset:0x814)
#define ACLK_BUS_ROOT_GATE                       0x00000050
#define HCLK_BUS_ROOT_GATE                       0x00000051
#define PCLK_BUS_ROOT_GATE                       0x00000052
#define ACLK_BUS_BIU_GATE                        0x00000053
#define HCLK_BUS_BIU_GATE                        0x00000054
#define PCLK_BUS_BIU_GATE                        0x00000055
#define ACLK_SYSRAM_GATE                         0x00000056
#define HCLK_SYSRAM_GATE                         0x00000057
#define ACLK_DMAC0_GATE                          0x00000058
#define ACLK_DMAC1_GATE                          0x00000059
#define HCLK_M0_GATE                             0x0000005A
#define SWCLKTCK_M0_GATE                         0x0000005B
#define CLK_CORE_CRYPTO_NS_GATE                  0x0000005C
#define CLK_PKA_CRYPTO_NS_GATE                   0x0000005D
#define ACLK_CRYPTO_NS_GATE                      0x0000005E
#define HCLK_CRYPTO_GATE                         0x0000005F

// CRU_GATE_CON06(Offset:0x818)
#define HCLK_RNG_GATE                            0x00000060
#define PCLK_BUS_GRF_GATE                        0x00000061
#define PCLK_TIMER0_GATE                         0x00000062
#define CLK_TIMER0_CH0_GATE                      0x00000063
#define CLK_TIMER0_CH1_GATE                      0x00000064
#define CLK_TIMER0_CH2_GATE                      0x00000065
#define CLK_TIMER0_CH3_GATE                      0x00000066
#define CLK_TIMER0_CH4_GATE                      0x00000067
#define CLK_TIMER0_CH5_GATE                      0x00000068
#define PCLK_WDT0_GATE                           0x00000069
#define TCLK_WDT0_GATE                           0x0000006A
#define PCLK_WDT1_GATE                           0x0000006B
#define TCLK_WDT1_GATE                           0x0000006C
#define PCLK_MAILBOX_GATE                        0x0000006D
#define PCLK_INTMUX_GATE                         0x0000006E
#define PCLK_SPINLOCK_GATE                       0x0000006F

// CRU_GATE_CON07(Offset:0x81C)
#define PCLK_DDRC_GATE                           0x00000070
#define HCLK_DDRPHY_GATE                         0x00000071
#define PCLK_DDRMON_GATE                         0x00000072
#define CLK_DDRMON_OSC_GATE                      0x00000073
#define PCLK_DDR_LPC_GATE                        0x00000074
#define HCLK_USBOTG0_GATE                        0x00000075
#define HCLK_USBOTG0_PMU_GATE                    0x00000076
#define CLK_USBOTG0_ADP_GATE                     0x00000077
#define HCLK_USBOTG1_GATE                        0x00000078
#define HCLK_USBOTG1_PMU_GATE                    0x00000079
#define CLK_USBOTG1_ADP_GATE                     0x0000007A
#define PCLK_USBPHY_GATE                         0x0000007B

// CRU_GATE_CON08(Offset:0x820)
#define ACLK_DMA2DDR_GATE                        0x00000080
#define PCLK_DMA2DDR_GATE                        0x00000081
#define STCLK_M0_GATE                            0x00000082

// CRU_GATE_CON10(Offset:0x828)
#define ACLK_DDRC_0_GATE                         0x000000A0
#define ACLK_DDRC_1_GATE                         0x000000A1
#define ACLK_DDR_BIU_GATE                        0x000000A2
#define CLK_DDRC_GATE                            0x000000A3
#define CLK_DDRMON_GATE                          0x000000A4

// CRU_GATE_CON11(Offset:0x82C)
#define HCLK_LSPERI_ROOT_GATE                    0x000000B0
#define PCLK_LSPERI_ROOT_GATE                    0x000000B1
#define HCLK_LSPERI_BIU_GATE                     0x000000B2
#define PCLK_UART0_GATE                          0x000000B4
#define PCLK_UART1_GATE                          0x000000B5
#define PCLK_UART2_GATE                          0x000000B6
#define PCLK_UART3_GATE                          0x000000B7
#define PCLK_UART4_GATE                          0x000000B8
#define SCLK_UART0_SRC_GATE                      0x000000B9
#define SCLK_UART1_SRC_GATE                      0x000000BA
#define SCLK_UART2_SRC_GATE                      0x000000BB
#define SCLK_UART3_SRC_GATE                      0x000000BC
#define SCLK_UART4_SRC_GATE                      0x000000BD
#define PCLK_I2C0_GATE                           0x000000BE
#define CLK_I2C0_GATE                            0x000000BF

// CRU_GATE_CON12(Offset:0x830)
#define PCLK_I2C1_GATE                           0x000000C0
#define CLK_I2C1_GATE                            0x000000C1
#define PCLK_I2C2_GATE                           0x000000C2
#define CLK_I2C2_GATE                            0x000000C3
#define PCLK_PWM1_GATE                           0x000000C4
#define CLK_PWM1_GATE                            0x000000C5
#define CLK_OSC_PWM1_GATE                        0x000000C6
#define CLK_RC_PWM1_GATE                         0x000000C7
#define CLK_FREQ_PWM1_GATE                       0x000000C8
#define CLK_COUNTER_PWM1_GATE                    0x000000C9
#define PCLK_SPI0_GATE                           0x000000CA
#define CLK_SPI0_GATE                            0x000000CB
#define PCLK_SPI1_GATE                           0x000000CC
#define CLK_SPI1_GATE                            0x000000CD
#define PCLK_GPIO2_GATE                          0x000000CE
#define DBCLK_GPIO2_GATE                         0x000000CF

// CRU_GATE_CON13(Offset:0x834)
#define PCLK_GPIO3_GATE                          0x000000D0
#define DBCLK_GPIO3_GATE                         0x000000D1
#define PCLK_GPIO4_GATE                          0x000000D2
#define DBCLK_GPIO4_GATE                         0x000000D3
#define PCLK_CAN0_GATE                           0x000000D4
#define CLK_CAN0_GATE                            0x000000D5
#define PCLK_CAN1_GATE                           0x000000D6
#define CLK_CAN1_GATE                            0x000000D7
#define HCLK_PDM_GATE                            0x000000D8
#define MCLK_PDM_GATE                            0x000000D9
#define CLKOUT_PDM_GATE                          0x000000DA
#define MCLK_SPDIFTX_GATE                        0x000000DB
#define HCLK_SPDIFTX_GATE                        0x000000DC
#define HCLK_SPDIFRX_GATE                        0x000000DD
#define MCLK_SPDIFRX_GATE                        0x000000DE
#define MCLK_SAI0_GATE                           0x000000DF

// CRU_GATE_CON14(Offset:0x838)
#define HCLK_SAI0_GATE                           0x000000E0
#define MCLK_OUT_SAI0_GATE                       0x000000E1
#define MCLK_SAI1_GATE                           0x000000E2
#define HCLK_SAI1_GATE                           0x000000E3
#define MCLK_OUT_SAI1_GATE                       0x000000E4
#define HCLK_ASRC0_GATE                          0x000000E5
#define CLK_ASRC0_GATE                           0x000000E6
#define HCLK_ASRC1_GATE                          0x000000E7
#define CLK_ASRC1_GATE                           0x000000E8
#define PCLK_CRU_GATE                            0x000000E9
#define PCLK_PMU_ROOT_GATE                       0x000000EA

// CRU_GATE_CON16(Offset:0x840)
#define MCLK_ASRC0_SRC_GATE                      0x00000100
#define MCLK_ASRC1_SRC_GATE                      0x00000101
#define MCLK_ASRC2_SRC_GATE                      0x00000102
#define MCLK_ASRC3_SRC_GATE                      0x00000103
#define LRCK_ASRC0_SRC_GATE                      0x00000104
#define LRCK_ASRC0_DST_GATE                      0x00000105
#define LRCK_ASRC1_SRC_GATE                      0x00000106
#define LRCK_ASRC1_DST_GATE                      0x00000107

// CRU_GATE_CON17(Offset:0x844)
#define ACLK_HSPERIROOT_GATE                     0x00000110
#define HCLK_HSPERI_ROOT_GATE                    0x00000111
#define PCLK_HSPERI_ROOT_GATE                    0x00000112
#define HCLK_HSPERI_BIU_GATE                     0x00000114
#define CCLK_SRC_SDMMC_GATE                      0x00000116
#define HCLK_SDMMC_GATE                          0x00000117
#define HCLK_FSPI_GATE                           0x00000118
#define SCLK_FSPI_GATE                           0x00000119
#define PCLK_SPI2_GATE                           0x0000011A
#define ACLK_MAC0_GATE                           0x0000011B
#define ACLK_MAC1_GATE                           0x0000011C
#define PCLK_MAC0_GATE                           0x0000011D
#define PCLK_MAC1_GATE                           0x0000011E
#define CLK_MAC_ROOT_GATE                        0x0000011F

// CRU_GATE_CON18(Offset:0x848)
#define CLK_MAC0_GATE                            0x00000120
#define CLK_MAC1_GATE                            0x00000121
#define MCLK_SAI2_GATE                           0x00000122
#define HCLK_SAI2_GATE                           0x00000123
#define MCLK_OUT_SAI2_GATE                       0x00000124
#define MCLK_SAI3_SRC_GATE                       0x00000125
#define HCLK_SAI3_GATE                           0x00000126
#define MCLK_SAI3_GATE                           0x00000127
#define MCLK_OUT_SAI3_GATE                       0x00000128
#define MCLK_SAI4_SRC_GATE                       0x00000129
#define HCLK_SAI4_GATE                           0x0000012A
#define MCLK_SAI4_GATE                           0x0000012B
#define HCLK_DSM_GATE                            0x0000012C
#define MCLK_DSM_GATE                            0x0000012D
#define PCLK_AUDIO_ADC_GATE                      0x0000012E
#define MCLK_AUDIO_ADC_GATE                      0x0000012F

// CRU_GATE_CON19(Offset:0x84C)
#define PCLK_SARADC_GATE                         0x00000130
#define CLK_SARADC_GATE                          0x00000131
#define PCLK_OTPC_NS_GATE                        0x00000133
#define CLK_SBPI_OTPC_NS_GATE                    0x00000134
#define PCLK_UART5_GATE                          0x00000136
#define SCLK_UART5_GATE                          0x00000137
#define PCLK_GPIO234_IOC_GATE                    0x00000138

// CRU_GATE_CON20(Offset:0x850)
#define CLK_SPI2_GATE                            0x00000140

// CRU_GATE_CON21(Offset:0x854)
#define ACLK_VIO_ROOT_GATE                       0x00000150
#define HCLK_VIO_ROOT_GATE                       0x00000151
#define PCLK_VIO_ROOT_GATE                       0x00000152
#define ACLK_VIO_BIU_GATE                        0x00000153
#define HCLK_VIO_BIU_GATE                        0x00000154
#define HCLK_RGA_GATE                            0x00000156
#define ACLK_RGA_GATE                            0x00000157
#define CLK_CORE_RGA_GATE                        0x00000158
#define ACLK_VOP_GATE                            0x00000159
#define HCLK_VOP_GATE                            0x0000015A
#define DCLK_VOP_GATE                            0x0000015B
#define PCLK_DPHY_GATE                           0x0000015C
#define PCLK_DSI_HOST_GATE                       0x0000015D
#define PCLK_TSADC_GATE                          0x0000015E
#define CLK_TSADC_GATE                           0x0000015F

// CRU_GATE_CON22(Offset:0x858)
#define CLK_TSADC_TSEN_GATE                      0x00000160
#define PCLK_GPIO1_IOC_GATE                      0x00000161

// CRU_CLKSEL_CON00(Offset:0x300)
#define CLK_GPLL_DIV_DIV                         0x04060000
#define CLK_GPLL_DIV_100M_DIV                    0x040A0000

// CRU_CLKSEL_CON01(Offset:0x304)
#define CLK_V0PLL_DIV_DIV                        0x04000001
#define CLK_V1PLL_DIV_DIV                        0x04040001
#define CLK_INT_VOICE_MATRIX0_DIV                0x05080001

// CRU_CLKSEL_CON02(Offset:0x308)
#define CLK_INT_VOICE_MATRIX1_DIV                0x05000002
#define CLK_INT_VOICE_MATRIX2_DIV                0x05050002

// CRU_CLKSEL_CON03(Offset:0x30C)
#define CLK_TESTOUT_DIV                          0x05000003
#define CLK_TESTOUT_SEL                          0x04050003
#define CLK_TESTOUT_SEL_XIN_OSC0_FUNC_GATE       0U
#define CLK_TESTOUT_SEL_CLK_GPLL                 1U
#define CLK_TESTOUT_SEL_CLK_V0PLL                2U
#define CLK_TESTOUT_SEL_CLK_V1PLL                3U
#define CLK_TESTOUT_SEL_CLK_DEEPSLOW             4U
#define CLK_TESTOUT_SEL_CLK_RC                   5U
#define CLK_TESTOUT_SEL_CLK_CORE_PVTPLL_LOGIC    6U
#define CLK_TESTOUT_SEL_CLK_FRAC_UART_MATRIX0    7U
#define CLK_TESTOUT_SEL_CLK_FRAC_VOICE_MATRIX0   8U
#define CLK_TESTOUT_SEL_CLK_CORE_SRC_PRE         9U
#define CLK_TESTOUT_SEL_CLK_USBOTG0_UTMI_PHY     10U
#define CLK_TESTOUT_SEL_CLK_USBOTG1_UTMI_PHY     11U
#define CLK_TESTOUT_SEL_DSI_LANEBYTE_CLK_PHY     12U
#define CLK_TESTOUT_SEL_DSI_RXCLK_ESC_PHY        13U
#define CLK_TESTOUT_SEL_CLK_SPDIFRX_TO_ASRC      14U
#define CLK_TESTOUT_SEL_RESERVED                 15U
#define CLK_FRAC_UART_MATRIX0_MUX_SEL            0x02090003
#define CLK_FRAC_UART_MATRIX0_MUX_SEL_XIN_OSC0_FUNC 0U
#define CLK_FRAC_UART_MATRIX0_MUX_SEL_CLK_GPLL_MUX 1U
#define CLK_FRAC_UART_MATRIX0_MUX_SEL_CLK_V0PLL_MUX_GATE 2U
#define CLK_FRAC_UART_MATRIX0_MUX_SEL_CLK_V1PLL_MUX_GATE 3U
#define CLK_FRAC_UART_MATRIX1_MUX_SEL            0x020B0003
#define CLK_FRAC_UART_MATRIX1_MUX_SEL_XIN_OSC0_FUNC 0U
#define CLK_FRAC_UART_MATRIX1_MUX_SEL_CLK_GPLL_MUX_GATE 1U
#define CLK_FRAC_UART_MATRIX1_MUX_SEL_CLK_V0PLL_MUX_GATE 2U
#define CLK_FRAC_UART_MATRIX1_MUX_SEL_CLK_V1PLL_MUX_GATE 3U
#define CLK_FRAC_VOICE_MATRIX0_MUX_SEL           0x020D0003
#define CLK_FRAC_VOICE_MATRIX0_MUX_SEL_XIN_OSC0_FUNC_GATE 0U
#define CLK_FRAC_VOICE_MATRIX0_MUX_SEL_CLK_GPLL_MUX_GATE 1U
#define CLK_FRAC_VOICE_MATRIX0_MUX_SEL_CLK_V0PLL_MUX_GATE 2U
#define CLK_FRAC_VOICE_MATRIX0_MUX_SEL_CLK_V1PLL_MUX_GATE 3U

// CRU_CLKSEL_CON04(Offset:0x310)
#define CLK_FRAC_VOICE_MATRIX1_MUX_SEL           0x02000004
#define CLK_FRAC_VOICE_MATRIX1_MUX_SEL_XIN_OSC0_FUNC_GATE 0U
#define CLK_FRAC_VOICE_MATRIX1_MUX_SEL_CLK_GPLL_MUX_GATE 1U
#define CLK_FRAC_VOICE_MATRIX1_MUX_SEL_CLK_V0PLL_MUX_GATE 2U
#define CLK_FRAC_VOICE_MATRIX1_MUX_SEL_CLK_V1PLL_MUX_GATE 3U
#define CLK_FRAC_COMMON_MATRIX0_MUX_SEL          0x02020004
#define CLK_FRAC_COMMON_MATRIX0_MUX_SEL_XIN_OSC0_FUNC_GATE 0U
#define CLK_FRAC_COMMON_MATRIX0_MUX_SEL_CLK_GPLL_MUX_GATE 1U
#define CLK_FRAC_COMMON_MATRIX0_MUX_SEL_CLK_V0PLL_MUX_GATE 2U
#define CLK_FRAC_COMMON_MATRIX0_MUX_SEL_CLK_V1PLL_MUX_GATE 3U
#define CLK_FRAC_COMMON_MATRIX1_MUX_SEL          0x02040004
#define CLK_FRAC_COMMON_MATRIX1_MUX_SEL_XIN_OSC0_FUNC 0U
#define CLK_FRAC_COMMON_MATRIX1_MUX_SEL_CLK_GPLL_MUX_GATE 1U
#define CLK_FRAC_COMMON_MATRIX1_MUX_SEL_CLK_V0PLL_MUX_GATE 2U
#define CLK_FRAC_COMMON_MATRIX1_MUX_SEL_CLK_V1PLL_MUX_GATE 3U
#define CLK_FRAC_COMMON_MATRIX2_MUX_SEL          0x02060004
#define CLK_FRAC_COMMON_MATRIX2_MUX_SEL_XIN_OSC0_FUNC_GATE 0U
#define CLK_FRAC_COMMON_MATRIX2_MUX_SEL_CLK_GPLL_MUX_GATE 1U
#define CLK_FRAC_COMMON_MATRIX2_MUX_SEL_CLK_V0PLL_MUX_GATE 2U
#define CLK_FRAC_COMMON_MATRIX2_MUX_SEL_CLK_V1PLL_MUX_GATE 3U

// CRU_CLKSEL_CON05(Offset:0x314)
#define CLK_FRAC_UART_MATRIX0_DIV                0x20000005

// CRU_CLKSEL_CON06(Offset:0x318)
#define CLK_FRAC_UART_MATRIX1_DIV                0x20000006

// CRU_CLKSEL_CON07(Offset:0x31C)
#define CLK_FRAC_VOICE_MATRIX0_DIV               0x20000007

// CRU_CLKSEL_CON08(Offset:0x320)

// CRU_CLKSEL_CON09(Offset:0x324)
#define CLK_FRAC_VOICE_MATRIX1_DIV               0x20000009

// CRU_CLKSEL_CON10(Offset:0x328)

// CRU_CLKSEL_CON11(Offset:0x32C)
#define CLK_FRAC_COMMON_MATRIX0_DIV              0x2000000B

// CRU_CLKSEL_CON12(Offset:0x330)
#define CLK_FRAC_COMMON_MATRIX1_DIV              0x2000000C

// CRU_CLKSEL_CON13(Offset:0x334)
#define CLK_FRAC_COMMON_MATRIX2_DIV              0x2000000D

// CRU_CLKSEL_CON15(Offset:0x33C)
#define CLK_CORE_SRC_DIV_DIV                     0x0500000F
#define ACLK_CORE_ROOT_DIV                       0x0409000F
#define CLK_CORE_SRC_DIV_SEL                     0x0205000F
#define CLK_CORE_SRC_DIV_SEL_CLK_GPLL_MUX_GATE   0U
#define CLK_CORE_SRC_DIV_SEL_CLK_V0PLL_MUX_GATE  1U
#define CLK_CORE_SRC_DIV_SEL_CLK_V1PLL_MUX_GATE  2U
#define CLK_CORE_PVTPLL_SRC_SEL                  0x0107000F
#define CLK_CORE_PVTPLL_SRC_SEL_CLK_DEEPSLOW     0U
#define CLK_CORE_PVTPLL_SRC_SEL_CLK_CORE_PVTPLL  1U
#define CLK_CORE_SRC_PVTMUX_SEL                  0x0108000F
#define CLK_CORE_SRC_PVTMUX_SEL_CLK_CORE_SRC_PRE 0U
#define CLK_CORE_SRC_PVTMUX_SEL_CLK_CORE_PVTPLL_SRC 1U

// CRU_CLKSEL_CON16(Offset:0x340)
#define PCLK_CORE_ROOT_DIV                       0x04000010

// CRU_CLKSEL_CON18(Offset:0x348)
#define ACLK_CORE_PERI_ROOT_DIV                  0x05000012
#define CLK_DSMC_DIV                             0x05070012
#define ACLK_CORE_PERI_ROOT_SEL                  0x02050012
#define ACLK_CORE_PERI_ROOT_SEL_CLK_GPLL_MUX_GATE 0U
#define ACLK_CORE_PERI_ROOT_SEL_CLK_V0PLL_MUX_GATE 1U
#define ACLK_CORE_PERI_ROOT_SEL_CLK_V1PLL_MUX_GATE 2U
#define CLK_DSMC_SEL                             0x020C0012
#define CLK_DSMC_SEL_XIN_OSC0_FUNC_GATE          0U
#define CLK_DSMC_SEL_CLK_GPLL_MUX_GATE           1U
#define CLK_DSMC_SEL_CLK_V0PLL_MUX_GATE          2U
#define CLK_DSMC_SEL_CLK_V1PLL_MUX_GATE          3U

// CRU_CLKSEL_CON19(Offset:0x34C)
#define CLK_FLEXBUS_TX_DIV                       0x05000013
#define CLK_FLEXBUS_RX_DIV                       0x05070013
#define CLK_FLEXBUS_TX_SEL                       0x02050013
#define CLK_FLEXBUS_TX_SEL_XIN_OSC0_FUNC_GATE    0U
#define CLK_FLEXBUS_TX_SEL_CLK_GPLL_MUX_GATE     1U
#define CLK_FLEXBUS_TX_SEL_CLK_V0PLL_MUX_GATE    2U
#define CLK_FLEXBUS_TX_SEL_CLK_V1PLL_MUX_GATE    3U
#define CLK_FLEXBUS_RX_SEL                       0x020C0013
#define CLK_FLEXBUS_RX_SEL_XIN_OSC0_FUNC_GATE    0U
#define CLK_FLEXBUS_RX_SEL_CLK_GPLL_MUX_GATE     1U
#define CLK_FLEXBUS_RX_SEL_CLK_V0PLL_MUX_GATE    2U
#define CLK_FLEXBUS_RX_SEL_CLK_V1PLL_MUX_GATE    3U

// CRU_CLKSEL_CON21(Offset:0x354)
#define ACLK_BUS_ROOT_DIV                        0x05000015
#define HCLK_BUS_ROOT_DIV                        0x05070015
#define ACLK_BUS_ROOT_SEL                        0x02050015
#define ACLK_BUS_ROOT_SEL_CLK_GPLL_DIV           0U
#define ACLK_BUS_ROOT_SEL_CLK_V0PLL_DIV          1U
#define ACLK_BUS_ROOT_SEL_CLK_V1PLL_DIV          2U
#define HCLK_BUS_ROOT_SEL                        0x020C0015
#define HCLK_BUS_ROOT_SEL_CLK_GPLL_DIV           0U
#define HCLK_BUS_ROOT_SEL_CLK_V0PLL_DIV          1U
#define HCLK_BUS_ROOT_SEL_CLK_V1PLL_DIV          2U

// CRU_CLKSEL_CON22(Offset:0x358)
#define PCLK_BUS_ROOT_DIV                        0x05000016
#define PCLK_BUS_ROOT_SEL                        0x02050016
#define PCLK_BUS_ROOT_SEL_CLK_GPLL_DIV           0U
#define PCLK_BUS_ROOT_SEL_CLK_V0PLL_DIV          1U
#define PCLK_BUS_ROOT_SEL_CLK_V1PLL_DIV          2U
#define CLK_TIMER0_CH0_SEL                       0x03070016
#define CLK_TIMER0_CH0_SEL_XIN_OSC0_FUNC         0U
#define CLK_TIMER0_CH0_SEL_CLK_GPLL_DIV_100M     1U
#define CLK_TIMER0_CH0_SEL_CLK_DEEPSLOW          2U
#define CLK_TIMER0_CH0_SEL_CLK_CORE_PVTPLL_LOGIC 3U
#define CLK_TIMER0_CH0_SEL_SAI0_MCLK_IO_IN       4U
#define CLK_TIMER0_CH0_SEL_SAI0_SCLK_IO_IN       5U
#define CLK_TIMER0_CH1_SEL                       0x030A0016
#define CLK_TIMER0_CH1_SEL_XIN_OSC0_FUNC         0U
#define CLK_TIMER0_CH1_SEL_CLK_GPLL_DIV_100M     1U
#define CLK_TIMER0_CH1_SEL_CLK_DEEPSLOW          2U
#define CLK_TIMER0_CH1_SEL_CLK_CORE_PVTPLL_LOGIC 3U
#define CLK_TIMER0_CH1_SEL_SAI1_MCLK_IO_IN       4U
#define CLK_TIMER0_CH1_SEL_SAI1_SCLK_IO_IN       5U
#define CLK_TIMER0_CH2_SEL                       0x030D0016
#define CLK_TIMER0_CH2_SEL_XIN_OSC0_FUNC         0U
#define CLK_TIMER0_CH2_SEL_CLK_GPLL_DIV_100M     1U
#define CLK_TIMER0_CH2_SEL_CLK_DEEPSLOW          2U
#define CLK_TIMER0_CH2_SEL_CLK_CORE_PVTPLL_LOGIC 3U
#define CLK_TIMER0_CH2_SEL_SAI2_MCLK_IO_IN       4U
#define CLK_TIMER0_CH2_SEL_SAI2_SCLK_IO_IN       5U

// CRU_CLKSEL_CON23(Offset:0x35C)
#define STCLK_M0_DIV                             0x06090017
#define CLK_TIMER0_CH3_SEL                       0x03000017
#define CLK_TIMER0_CH3_SEL_XIN_OSC0_FUNC         0U
#define CLK_TIMER0_CH3_SEL_CLK_GPLL_DIV_100M     1U
#define CLK_TIMER0_CH3_SEL_CLK_DEEPSLOW          2U
#define CLK_TIMER0_CH3_SEL_CLK_CORE_PVTPLL_LOGIC 3U
#define CLK_TIMER0_CH3_SEL_SAI3_MCLK_IO_IN       4U
#define CLK_TIMER0_CH3_SEL_SAI3_SCLK_IO_IN       5U
#define CLK_TIMER0_CH4_SEL                       0x03030017
#define CLK_TIMER0_CH4_SEL_XIN_OSC0_FUNC         0U
#define CLK_TIMER0_CH4_SEL_CLK_GPLL_DIV_100M     1U
#define CLK_TIMER0_CH4_SEL_CLK_DEEPSLOW          2U
#define CLK_TIMER0_CH4_SEL_CLK_CORE_PVTPLL_LOGIC 3U
#define CLK_TIMER0_CH4_SEL_MCLK_ASRC0            4U
#define CLK_TIMER0_CH5_SEL                       0x03060017
#define CLK_TIMER0_CH5_SEL_XIN_OSC0_FUNC         0U
#define CLK_TIMER0_CH5_SEL_CLK_GPLL_DIV_100M     1U
#define CLK_TIMER0_CH5_SEL_CLK_DEEPSLOW          2U
#define CLK_TIMER0_CH5_SEL_CLK_CORE_PVTPLL_LOGIC 3U
#define CLK_TIMER0_CH5_SEL_MCLK_ASRC1            4U

// CRU_CLKSEL_CON29(Offset:0x374)
#define HCLK_LSPERI_ROOT_DIV                     0x0500001D
#define SCLK_UART0_DIV                           0x0507001D
#define HCLK_LSPERI_ROOT_SEL                     0x0205001D
#define HCLK_LSPERI_ROOT_SEL_CLK_GPLL_DIV        0U
#define HCLK_LSPERI_ROOT_SEL_CLK_V0PLL_DIV       1U
#define HCLK_LSPERI_ROOT_SEL_CLK_V1PLL_DIV       2U
#define SCLK_UART0_SEL                           0x030C001D
#define SCLK_UART0_SEL_XIN_OSC0_FUNC             0U
#define SCLK_UART0_SEL_CLK_GPLL_MUX_GATE         1U
#define SCLK_UART0_SEL_CLK_V0PLL_MUX_GATE        2U
#define SCLK_UART0_SEL_CLK_FRAC_UART_MATRIX0     3U
#define SCLK_UART0_SEL_CLK_FRAC_UART_MATRIX1     4U
#define SCLK_UART0_SEL_CLK_FRAC_COMMON_MATRIX0   5U
#define SCLK_UART0_SEL_CLK_FRAC_COMMON_MATRIX1   6U
#define SCLK_UART0_SEL_CLK_FRAC_COMMON_MATRIX2   7U

// CRU_CLKSEL_CON30(Offset:0x378)
#define SCLK_UART1_DIV                           0x0500001E
#define SCLK_UART2_DIV                           0x0508001E
#define SCLK_UART1_SEL                           0x0305001E
#define SCLK_UART1_SEL_XIN_OSC0_FUNC             0U
#define SCLK_UART1_SEL_CLK_GPLL_MUX_GATE         1U
#define SCLK_UART1_SEL_CLK_V0PLL_MUX_GATE        2U
#define SCLK_UART1_SEL_CLK_FRAC_UART_MATRIX0     3U
#define SCLK_UART1_SEL_CLK_FRAC_UART_MATRIX1     4U
#define SCLK_UART1_SEL_CLK_FRAC_COMMON_MATRIX0   5U
#define SCLK_UART1_SEL_CLK_FRAC_COMMON_MATRIX1   6U
#define SCLK_UART1_SEL_CLK_FRAC_COMMON_MATRIX2   7U
#define SCLK_UART2_SEL                           0x030D001E
#define SCLK_UART2_SEL_XIN_OSC0_FUNC             0U
#define SCLK_UART2_SEL_CLK_GPLL_MUX_GATE         1U
#define SCLK_UART2_SEL_CLK_V0PLL_MUX_GATE        2U
#define SCLK_UART2_SEL_CLK_FRAC_UART_MATRIX0     3U
#define SCLK_UART2_SEL_CLK_FRAC_UART_MATRIX1     4U
#define SCLK_UART2_SEL_CLK_FRAC_COMMON_MATRIX0   5U
#define SCLK_UART2_SEL_CLK_FRAC_COMMON_MATRIX1   6U
#define SCLK_UART2_SEL_CLK_FRAC_COMMON_MATRIX2   7U

// CRU_CLKSEL_CON31(Offset:0x37C)
#define SCLK_UART3_DIV                           0x0500001F
#define SCLK_UART4_DIV                           0x0508001F
#define SCLK_UART3_SEL                           0x0305001F
#define SCLK_UART3_SEL_XIN_OSC0_FUNC             0U
#define SCLK_UART3_SEL_CLK_GPLL_MUX_GATE         1U
#define SCLK_UART3_SEL_CLK_V0PLL_MUX_GATE        2U
#define SCLK_UART3_SEL_CLK_FRAC_UART_MATRIX0     3U
#define SCLK_UART3_SEL_CLK_FRAC_UART_MATRIX1     4U
#define SCLK_UART3_SEL_CLK_FRAC_COMMON_MATRIX0   5U
#define SCLK_UART3_SEL_CLK_FRAC_COMMON_MATRIX1   6U
#define SCLK_UART3_SEL_CLK_FRAC_COMMON_MATRIX2   7U
#define SCLK_UART4_SEL                           0x030D001F
#define SCLK_UART4_SEL_XIN_OSC0_FUNC             0U
#define SCLK_UART4_SEL_CLK_GPLL_MUX_GATE         1U
#define SCLK_UART4_SEL_CLK_V0PLL_MUX_GATE        2U
#define SCLK_UART4_SEL_CLK_FRAC_UART_MATRIX0     3U
#define SCLK_UART4_SEL_CLK_FRAC_UART_MATRIX1     4U
#define SCLK_UART4_SEL_CLK_FRAC_COMMON_MATRIX0   5U
#define SCLK_UART4_SEL_CLK_FRAC_COMMON_MATRIX1   6U
#define SCLK_UART4_SEL_CLK_FRAC_COMMON_MATRIX2   7U

// CRU_CLKSEL_CON32(Offset:0x380)
#define CLK_I2C0_DIV                             0x04000020
#define CLK_I2C1_DIV                             0x04060020
#define CLK_I2C0_SEL                             0x02040020
#define CLK_I2C0_SEL_XIN_OSC0_FUNC_GATE          0U
#define CLK_I2C0_SEL_CLK_GPLL_DIV                1U
#define CLK_I2C0_SEL_CLK_V0PLL_DIV               2U
#define CLK_I2C0_SEL_CLK_V1PLL_DIV               3U
#define CLK_I2C1_SEL                             0x020A0020
#define CLK_I2C1_SEL_XIN_OSC0_FUNC_GATE          0U
#define CLK_I2C1_SEL_CLK_GPLL_DIV                1U
#define CLK_I2C1_SEL_CLK_V0PLL_DIV               2U
#define CLK_I2C1_SEL_CLK_V1PLL_DIV               3U

// CRU_CLKSEL_CON33(Offset:0x384)
#define CLK_I2C2_DIV                             0x04000021
#define CLK_PWM1_DIV                             0x04060021
#define CLK_I2C2_SEL                             0x02040021
#define CLK_I2C2_SEL_XIN_OSC0_FUNC_GATE          0U
#define CLK_I2C2_SEL_CLK_GPLL_DIV                1U
#define CLK_I2C2_SEL_CLK_V0PLL_DIV               2U
#define CLK_I2C2_SEL_CLK_V1PLL_DIV               3U
#define CLK_PWM1_SEL                             0x020A0021
#define CLK_PWM1_SEL_CLK_GPLL_DIV                0U
#define CLK_PWM1_SEL_CLK_V0PLL_DIV               1U
#define CLK_PWM1_SEL_CLK_V1PLL_DIV               2U
#define CLK_FREQ_PWM1_SEL                        0x040C0021
#define CLK_FREQ_PWM1_SEL_CLK_RC                 0U
#define CLK_FREQ_PWM1_SEL_SAI0_MCLK_IO_IN        1U
#define CLK_FREQ_PWM1_SEL_SAI1_MCLK_IO_IN        2U
#define CLK_FREQ_PWM1_SEL_SAI2_MCLK_IO_IN        3U
#define CLK_FREQ_PWM1_SEL_SAI3_MCLK_IO_IN        4U
#define CLK_FREQ_PWM1_SEL_SAI0_SCLK_IO_IN        5U
#define CLK_FREQ_PWM1_SEL_SAI1_SCLK_IO_IN        6U
#define CLK_FREQ_PWM1_SEL_SAI2_SCLK_IO_IN        7U
#define CLK_FREQ_PWM1_SEL_SAI3_SCLK_IO_IN        8U
#define CLK_FREQ_PWM1_SEL_MCLK_ASRC0             9U
#define CLK_FREQ_PWM1_SEL_MCLK_ASRC1             10U

// CRU_CLKSEL_CON34(Offset:0x388)
#define CLK_SPI0_DIV                             0x04040022
#define CLK_SPI1_DIV                             0x040A0022
#define CLK_COUNTER_PWM1_SEL                     0x04000022
#define CLK_COUNTER_PWM1_SEL_CLK_RC              0U
#define CLK_COUNTER_PWM1_SEL_SAI0_MCLK_IO_IN     1U
#define CLK_COUNTER_PWM1_SEL_SAI1_MCLK_IO_IN     2U
#define CLK_COUNTER_PWM1_SEL_SAI2_MCLK_IO_IN     3U
#define CLK_COUNTER_PWM1_SEL_SAI3_MCLK_IO_IN     4U
#define CLK_COUNTER_PWM1_SEL_SAI0_SCLK_IO_IN     5U
#define CLK_COUNTER_PWM1_SEL_SAI1_SCLK_IO_IN     6U
#define CLK_COUNTER_PWM1_SEL_SAI2_SCLK_IO_IN     7U
#define CLK_COUNTER_PWM1_SEL_SAI3_SCLK_IO_IN     8U
#define CLK_COUNTER_PWM1_SEL_MCLK_ASRC0          9U
#define CLK_COUNTER_PWM1_SEL_MCLK_ASRC1          10U
#define CLK_SPI0_SEL                             0x02080022
#define CLK_SPI0_SEL_XIN_OSC0_FUNC_GATE          0U
#define CLK_SPI0_SEL_CLK_GPLL_DIV                1U
#define CLK_SPI0_SEL_CLK_V0PLL_DIV               2U
#define CLK_SPI0_SEL_CLK_V1PLL_DIV               3U
#define CLK_SPI1_SEL                             0x020E0022
#define CLK_SPI1_SEL_XIN_OSC0_FUNC_GATE          0U
#define CLK_SPI1_SEL_CLK_GPLL_DIV                1U
#define CLK_SPI1_SEL_CLK_V0PLL_DIV               2U
#define CLK_SPI1_SEL_CLK_V1PLL_DIV               3U

// CRU_CLKSEL_CON35(Offset:0x38C)
#define CLK_CAN0_DIV                             0x05060023
#define DBCLK_GPIO2_SEL                          0x02000023
#define DBCLK_GPIO2_SEL_XIN_OSC0_FUNC            0U
#define DBCLK_GPIO2_SEL_CLK_RC                   1U
#define DBCLK_GPIO2_SEL_CLK_DEEPSLOW             2U
#define DBCLK_GPIO3_SEL                          0x02020023
#define DBCLK_GPIO3_SEL_XIN_OSC0_FUNC            0U
#define DBCLK_GPIO3_SEL_CLK_RC                   1U
#define DBCLK_GPIO3_SEL_CLK_DEEPSLOW             2U
#define DBCLK_GPIO4_SEL                          0x02040023
#define DBCLK_GPIO4_SEL_XIN_OSC0_FUNC            0U
#define DBCLK_GPIO4_SEL_CLK_RC                   1U
#define DBCLK_GPIO4_SEL_CLK_DEEPSLOW             2U
#define CLK_CAN0_SEL                             0x030B0023
#define CLK_CAN0_SEL_XIN_OSC0_FUNC               0U
#define CLK_CAN0_SEL_CLK_GPLL_MUX                1U
#define CLK_CAN0_SEL_CLK_V0PLL_MUX_GATE          2U
#define CLK_CAN0_SEL_CLK_V1PLL_MUX_GATE          3U
#define CLK_CAN0_SEL_CLK_FRAC_VOICE_MATRIX1      4U
#define CLK_CAN0_SEL_CLK_FRAC_COMMON_MATRIX0     5U
#define CLK_CAN0_SEL_CLK_FRAC_COMMON_MATRIX1     6U
#define CLK_CAN0_SEL_CLK_FRAC_COMMON_MATRIX2     7U

// CRU_CLKSEL_CON36(Offset:0x390)
#define CLK_CAN1_DIV                             0x05000024
#define CLK_CAN1_SEL                             0x03050024
#define CLK_CAN1_SEL_XIN_OSC0_FUNC               0U
#define CLK_CAN1_SEL_CLK_GPLL_MUX                1U
#define CLK_CAN1_SEL_CLK_V0PLL_MUX_GATE          2U
#define CLK_CAN1_SEL_CLK_V1PLL_MUX_GATE          3U
#define CLK_CAN1_SEL_CLK_FRAC_VOICE_MATRIX1      4U
#define CLK_CAN1_SEL_CLK_FRAC_COMMON_MATRIX0     5U
#define CLK_CAN1_SEL_CLK_FRAC_COMMON_MATRIX1     6U
#define CLK_CAN1_SEL_CLK_FRAC_COMMON_MATRIX2     7U

// CRU_CLKSEL_CON37(Offset:0x394)
#define MCLK_PDM_DIV                             0x05000025
#define MCLK_PDM_SEL                             0x04050025
#define MCLK_PDM_SEL_XIN_OSC0_FUNC_GATE          0U
#define MCLK_PDM_SEL_CLK_INT_VOICE_MATRIX0       1U
#define MCLK_PDM_SEL_CLK_INT_VOICE_MATRIX1       2U
#define MCLK_PDM_SEL_CLK_INT_VOICE_MATRIX2       3U
#define MCLK_PDM_SEL_CLK_FRAC_VOICE_MATRIX0      4U
#define MCLK_PDM_SEL_CLK_FRAC_VOICE_MATRIX1      5U
#define MCLK_PDM_SEL_CLK_FRAC_COMMON_MATRIX0     6U
#define MCLK_PDM_SEL_CLK_FRAC_COMMON_MATRIX1     7U
#define MCLK_PDM_SEL_CLK_FRAC_COMMON_MATRIX2     8U
#define MCLK_PDM_SEL_SAI0_MCLK_IO_IN             9U
#define MCLK_PDM_SEL_SAI1_MCLK_IO_IN             10U
#define MCLK_PDM_SEL_SAI2_MCLK_IO_IN             11U
#define MCLK_PDM_SEL_SAI3_MCLK_IO_IN             12U
#define MCLK_PDM_SEL_CLK_GPLL_DIV                13U

// CRU_CLKSEL_CON38(Offset:0x398)
#define CLKOUT_PDM_DIV                           0x0A000026
#define CLKOUT_PDM_SEL                           0x040A0026
#define CLKOUT_PDM_SEL_XIN_OSC0_FUNC_GATE        0U
#define CLKOUT_PDM_SEL_CLK_INT_VOICE_MATRIX0     1U
#define CLKOUT_PDM_SEL_CLK_INT_VOICE_MATRIX1     2U
#define CLKOUT_PDM_SEL_CLK_INT_VOICE_MATRIX2     3U
#define CLKOUT_PDM_SEL_CLK_FRAC_VOICE_MATRIX0    4U
#define CLKOUT_PDM_SEL_CLK_FRAC_VOICE_MATRIX1    5U
#define CLKOUT_PDM_SEL_CLK_FRAC_COMMON_MATRIX0   6U
#define CLKOUT_PDM_SEL_CLK_FRAC_COMMON_MATRIX1   7U
#define CLKOUT_PDM_SEL_CLK_FRAC_COMMON_MATRIX2   8U
#define CLKOUT_PDM_SEL_SAI0_MCLK_IO_IN           9U
#define CLKOUT_PDM_SEL_SAI1_MCLK_IO_IN           10U
#define CLKOUT_PDM_SEL_SAI2_MCLK_IO_IN           11U
#define CLKOUT_PDM_SEL_SAI3_MCLK_IO_IN           12U
#define CLKOUT_PDM_SEL_CLK_GPLL_DIV              13U

// CRU_CLKSEL_CON39(Offset:0x39C)
#define MCLK_SPDIFTX_DIV                         0x05000027
#define MCLK_SPDIFRX_DIV                         0x05090027
#define MCLK_SPDIFTX_SEL                         0x04050027
#define MCLK_SPDIFTX_SEL_XIN_OSC0_FUNC_GATE      0U
#define MCLK_SPDIFTX_SEL_CLK_INT_VOICE_MATRIX0   1U
#define MCLK_SPDIFTX_SEL_CLK_INT_VOICE_MATRIX1   2U
#define MCLK_SPDIFTX_SEL_CLK_INT_VOICE_MATRIX2   3U
#define MCLK_SPDIFTX_SEL_CLK_FRAC_VOICE_MATRIX0  4U
#define MCLK_SPDIFTX_SEL_CLK_FRAC_VOICE_MATRIX1  5U
#define MCLK_SPDIFTX_SEL_CLK_FRAC_COMMON_MATRIX0 6U
#define MCLK_SPDIFTX_SEL_CLK_FRAC_COMMON_MATRIX1 7U
#define MCLK_SPDIFTX_SEL_CLK_FRAC_COMMON_MATRIX2 8U
#define MCLK_SPDIFTX_SEL_SAI0_MCLK_IO_IN         9U
#define MCLK_SPDIFTX_SEL_SAI1_MCLK_IO_IN         10U
#define MCLK_SPDIFTX_SEL_SAI2_MCLK_IO_IN         11U
#define MCLK_SPDIFTX_SEL_SAI3_MCLK_IO_IN         12U
#define MCLK_SPDIFRX_SEL                         0x020E0027
#define MCLK_SPDIFRX_SEL_CLK_GPLL_MUX_GATE       0U
#define MCLK_SPDIFRX_SEL_CLK_V0PLL_MUX_GATE      1U
#define MCLK_SPDIFRX_SEL_CLK_V1PLL_MUX_GATE      2U

// CRU_CLKSEL_CON40(Offset:0x3A0)
#define MCLK_SAI0_DIV                            0x08000028
#define MCLK_SAI0_SEL                            0x04080028
#define MCLK_SAI0_SEL_XIN_OSC0_FUNC_GATE         0U
#define MCLK_SAI0_SEL_CLK_INT_VOICE_MATRIX0      1U
#define MCLK_SAI0_SEL_CLK_INT_VOICE_MATRIX1      2U
#define MCLK_SAI0_SEL_CLK_INT_VOICE_MATRIX2      3U
#define MCLK_SAI0_SEL_CLK_FRAC_VOICE_MATRIX0     4U
#define MCLK_SAI0_SEL_CLK_FRAC_VOICE_MATRIX1     5U
#define MCLK_SAI0_SEL_CLK_FRAC_COMMON_MATRIX0    6U
#define MCLK_SAI0_SEL_CLK_FRAC_COMMON_MATRIX1    7U
#define MCLK_SAI0_SEL_CLK_FRAC_COMMON_MATRIX2    8U
#define MCLK_SAI0_SEL_SAI0_MCLK_IO_IN            9U
#define MCLK_SAI0_SEL_SAI1_MCLK_IO_IN            10U
#define MCLK_SAI0_SEL_SAI2_MCLK_IO_IN            11U
#define MCLK_SAI0_SEL_SAI3_MCLK_IO_IN            12U

// CRU_CLKSEL_CON41(Offset:0x3A4)
#define MCLK_SAI1_DIV                            0x08000029
#define MCLK_SAI1_SEL                            0x04080029
#define MCLK_SAI1_SEL_XIN_OSC0_FUNC_GATE         0U
#define MCLK_SAI1_SEL_CLK_INT_VOICE_MATRIX0      1U
#define MCLK_SAI1_SEL_CLK_INT_VOICE_MATRIX1      2U
#define MCLK_SAI1_SEL_CLK_INT_VOICE_MATRIX2      3U
#define MCLK_SAI1_SEL_CLK_FRAC_VOICE_MATRIX0     4U
#define MCLK_SAI1_SEL_CLK_FRAC_VOICE_MATRIX1     5U
#define MCLK_SAI1_SEL_CLK_FRAC_COMMON_MATRIX0    6U
#define MCLK_SAI1_SEL_CLK_FRAC_COMMON_MATRIX1    7U
#define MCLK_SAI1_SEL_CLK_FRAC_COMMON_MATRIX2    8U
#define MCLK_SAI1_SEL_SAI0_MCLK_IO_IN            9U
#define MCLK_SAI1_SEL_SAI1_MCLK_IO_IN            10U
#define MCLK_SAI1_SEL_SAI2_MCLK_IO_IN            11U
#define MCLK_SAI1_SEL_SAI3_MCLK_IO_IN            12U

// CRU_CLKSEL_CON42(Offset:0x3A8)
#define CLK_ASRC0_DIV                            0x0500002A
#define CLK_ASRC1_DIV                            0x0507002A
#define CLK_ASRC0_SEL                            0x0205002A
#define CLK_ASRC0_SEL_CLK_GPLL_MUX_GATE          0U
#define CLK_ASRC0_SEL_CLK_V0PLL_MUX_GATE         1U
#define CLK_ASRC0_SEL_CLK_V1PLL_MUX_GATE         2U
#define CLK_ASRC1_SEL                            0x020C002A
#define CLK_ASRC1_SEL_CLK_GPLL_MUX_GATE          0U
#define CLK_ASRC1_SEL_CLK_V0PLL_MUX_GATE         1U
#define CLK_ASRC1_SEL_CLK_V1PLL_MUX_GATE         2U

// CRU_CLKSEL_CON46(Offset:0x3B8)
#define MCLK_ASRC0_SEL                           0x0400002E
#define MCLK_ASRC0_SEL_XIN_OSC0_FUNC_GATE        0U
#define MCLK_ASRC0_SEL_CLK_INT_VOICE_MATRIX0     1U
#define MCLK_ASRC0_SEL_CLK_INT_VOICE_MATRIX1     2U
#define MCLK_ASRC0_SEL_CLK_INT_VOICE_MATRIX2     3U
#define MCLK_ASRC0_SEL_CLK_FRAC_VOICE_MATRIX0    4U
#define MCLK_ASRC0_SEL_CLK_FRAC_VOICE_MATRIX1    5U
#define MCLK_ASRC0_SEL_CLK_FRAC_COMMON_MATRIX0   6U
#define MCLK_ASRC0_SEL_CLK_FRAC_COMMON_MATRIX1   7U
#define MCLK_ASRC0_SEL_CLK_FRAC_COMMON_MATRIX2   8U
#define MCLK_ASRC0_SEL_SAI0_MCLK_IO_IN           9U
#define MCLK_ASRC0_SEL_SAI1_MCLK_IO_IN           10U
#define MCLK_ASRC0_SEL_SAI2_MCLK_IO_IN           11U
#define MCLK_ASRC0_SEL_SAI3_MCLK_IO_IN           12U
#define MCLK_ASRC1_SEL                           0x0404002E
#define MCLK_ASRC1_SEL_XIN_OSC0_FUNC_GATE        0U
#define MCLK_ASRC1_SEL_CLK_INT_VOICE_MATRIX0     1U
#define MCLK_ASRC1_SEL_CLK_INT_VOICE_MATRIX1     2U
#define MCLK_ASRC1_SEL_CLK_INT_VOICE_MATRIX2     3U
#define MCLK_ASRC1_SEL_CLK_FRAC_VOICE_MATRIX0    4U
#define MCLK_ASRC1_SEL_CLK_FRAC_VOICE_MATRIX1    5U
#define MCLK_ASRC1_SEL_CLK_FRAC_COMMON_MATRIX0   6U
#define MCLK_ASRC1_SEL_CLK_FRAC_COMMON_MATRIX1   7U
#define MCLK_ASRC1_SEL_CLK_FRAC_COMMON_MATRIX2   8U
#define MCLK_ASRC1_SEL_SAI0_MCLK_IO_IN           9U
#define MCLK_ASRC1_SEL_SAI1_MCLK_IO_IN           10U
#define MCLK_ASRC1_SEL_SAI2_MCLK_IO_IN           11U
#define MCLK_ASRC1_SEL_SAI3_MCLK_IO_IN           12U
#define MCLK_ASRC2_SEL                           0x0408002E
#define MCLK_ASRC2_SEL_XIN_OSC0_FUNC_GATE        0U
#define MCLK_ASRC2_SEL_CLK_INT_VOICE_MATRIX0     1U
#define MCLK_ASRC2_SEL_CLK_INT_VOICE_MATRIX1     2U
#define MCLK_ASRC2_SEL_CLK_INT_VOICE_MATRIX2     3U
#define MCLK_ASRC2_SEL_CLK_FRAC_VOICE_MATRIX0    4U
#define MCLK_ASRC2_SEL_CLK_FRAC_VOICE_MATRIX1    5U
#define MCLK_ASRC2_SEL_CLK_FRAC_COMMON_MATRIX0   6U
#define MCLK_ASRC2_SEL_CLK_FRAC_COMMON_MATRIX1   7U
#define MCLK_ASRC2_SEL_CLK_FRAC_COMMON_MATRIX2   8U
#define MCLK_ASRC2_SEL_SAI0_MCLK_IO_IN           9U
#define MCLK_ASRC2_SEL_SAI1_MCLK_IO_IN           10U
#define MCLK_ASRC2_SEL_SAI2_MCLK_IO_IN           11U
#define MCLK_ASRC2_SEL_SAI3_MCLK_IO_IN           12U
#define MCLK_ASRC3_SEL                           0x040C002E
#define MCLK_ASRC3_SEL_XIN_OSC0_FUNC_GATE        0U
#define MCLK_ASRC3_SEL_CLK_INT_VOICE_MATRIX0     1U
#define MCLK_ASRC3_SEL_CLK_INT_VOICE_MATRIX1     2U
#define MCLK_ASRC3_SEL_CLK_INT_VOICE_MATRIX2     3U
#define MCLK_ASRC3_SEL_CLK_FRAC_VOICE_MATRIX0    4U
#define MCLK_ASRC3_SEL_CLK_FRAC_VOICE_MATRIX1    5U
#define MCLK_ASRC3_SEL_CLK_FRAC_COMMON_MATRIX0   6U
#define MCLK_ASRC3_SEL_CLK_FRAC_COMMON_MATRIX1   7U
#define MCLK_ASRC3_SEL_CLK_FRAC_COMMON_MATRIX2   8U
#define MCLK_ASRC3_SEL_SAI0_MCLK_IO_IN           9U
#define MCLK_ASRC3_SEL_SAI1_MCLK_IO_IN           10U
#define MCLK_ASRC3_SEL_SAI2_MCLK_IO_IN           11U
#define MCLK_ASRC3_SEL_SAI3_MCLK_IO_IN           12U

// CRU_CLKSEL_CON47(Offset:0x3BC)
#define LRCK_ASRC0_SRC_SEL                       0x0400002F
#define LRCK_ASRC0_SRC_SEL_MCLK_ASRC0            0U
#define LRCK_ASRC0_SRC_SEL_MCLK_ASRC1            1U
#define LRCK_ASRC0_SRC_SEL_MCLK_SPDIFTX          2U
#define LRCK_ASRC0_SRC_SEL_CLK_SPDIFRX_TO_ASRC   3U
#define LRCK_ASRC0_SRC_SEL_CLKOUT_PDM            4U
#define LRCK_ASRC0_SRC_SEL_SAI0_FS               5U
#define LRCK_ASRC0_SRC_SEL_SAI1_FS               6U
#define LRCK_ASRC0_SRC_SEL_SAI2_FS               7U
#define LRCK_ASRC0_SRC_SEL_SAI3_FS               8U
#define LRCK_ASRC0_SRC_SEL_SAI4_FS               9U
#define LRCK_ASRC0_DST_SEL                       0x0404002F
#define LRCK_ASRC0_DST_SEL_MCLK_ASRC0            0U
#define LRCK_ASRC0_DST_SEL_MCLK_ASRC1            1U
#define LRCK_ASRC0_DST_SEL_MCLK_SPDIFTX          2U
#define LRCK_ASRC0_DST_SEL_CLK_SPDIFRX_TO_ASRC   3U
#define LRCK_ASRC0_DST_SEL_CLKOUT_PDM            4U
#define LRCK_ASRC0_DST_SEL_SAI0_FS               5U
#define LRCK_ASRC0_DST_SEL_SAI1_FS               6U
#define LRCK_ASRC0_DST_SEL_SAI2_FS               7U
#define LRCK_ASRC0_DST_SEL_SAI3_FS               8U
#define LRCK_ASRC0_DST_SEL_SAI4_FS               9U
#define LRCK_ASRC1_SRC_SEL                       0x0408002F
#define LRCK_ASRC1_SRC_SEL_MCLK_ASRC0            0U
#define LRCK_ASRC1_SRC_SEL_MCLK_ASRC1            1U
#define LRCK_ASRC1_SRC_SEL_MCLK_SPDIFTX          2U
#define LRCK_ASRC1_SRC_SEL_CLK_SPDIFRX_TO_ASRC   3U
#define LRCK_ASRC1_SRC_SEL_CLKOUT_PDM            4U
#define LRCK_ASRC1_SRC_SEL_SAI0_FS               5U
#define LRCK_ASRC1_SRC_SEL_SAI1_FS               6U
#define LRCK_ASRC1_SRC_SEL_SAI2_FS               7U
#define LRCK_ASRC1_SRC_SEL_SAI3_FS               8U
#define LRCK_ASRC1_SRC_SEL_SAI4_FS               9U
#define LRCK_ASRC1_DST_SEL                       0x040C002F
#define LRCK_ASRC1_DST_SEL_MCLK_ASRC0            0U
#define LRCK_ASRC1_DST_SEL_MCLK_ASRC1            1U
#define LRCK_ASRC1_DST_SEL_MCLK_SPDIFTX          2U
#define LRCK_ASRC1_DST_SEL_CLK_SPDIFRX_TO_ASRC   3U
#define LRCK_ASRC1_DST_SEL_CLKOUT_PDM            4U
#define LRCK_ASRC1_DST_SEL_SAI0_FS               5U
#define LRCK_ASRC1_DST_SEL_SAI1_FS               6U
#define LRCK_ASRC1_DST_SEL_SAI2_FS               7U
#define LRCK_ASRC1_DST_SEL_SAI3_FS               8U
#define LRCK_ASRC1_DST_SEL_SAI4_FS               9U

// CRU_CLKSEL_CON49(Offset:0x3C4)
#define ACLK_HSPERI_ROOT_DIV                     0x05000031
#define CCLK_SRC_SDMMC_DIV                       0x06070031
#define ACLK_HSPERI_ROOT_SEL                     0x02050031
#define ACLK_HSPERI_ROOT_SEL_CLK_GPLL_DIV        0U
#define ACLK_HSPERI_ROOT_SEL_CLK_V0PLL_DIV       1U
#define ACLK_HSPERI_ROOT_SEL_CLK_V1PLL_DIV       2U
#define CCLK_SRC_SDMMC_SEL                       0x020D0031
#define CCLK_SRC_SDMMC_SEL_XIN_OSC0_FUNC_GATE    0U
#define CCLK_SRC_SDMMC_SEL_CLK_GPLL_MUX          1U
#define CCLK_SRC_SDMMC_SEL_CLK_V0PLL_MUX_GATE    2U
#define CCLK_SRC_SDMMC_SEL_CLK_V1PLL_MUX_GATE    3U

// CRU_CLKSEL_CON50(Offset:0x3C8)
#define SCLK_FSPI_DIV                            0x05000032
#define CLK_MAC_ROOT_DIV                         0x05070032
#define SCLK_FSPI_SEL                            0x02050032
#define SCLK_FSPI_SEL_XIN_OSC0_FUNC_GATE         0U
#define SCLK_FSPI_SEL_CLK_GPLL_MUX_GATE          1U
#define SCLK_FSPI_SEL_CLK_V0PLL_MUX_GATE         2U
#define SCLK_FSPI_SEL_CLK_V1PLL_MUX_GATE         3U

// CRU_CLKSEL_CON51(Offset:0x3CC)
#define MCLK_SAI2_DIV                            0x08000033
#define MCLK_SAI2_SEL                            0x04080033
#define MCLK_SAI2_SEL_XIN_OSC0_FUNC_GATE         0U
#define MCLK_SAI2_SEL_CLK_INT_VOICE_MATRIX0      1U
#define MCLK_SAI2_SEL_CLK_INT_VOICE_MATRIX1      2U
#define MCLK_SAI2_SEL_CLK_INT_VOICE_MATRIX2      3U
#define MCLK_SAI2_SEL_CLK_FRAC_VOICE_MATRIX0     4U
#define MCLK_SAI2_SEL_CLK_FRAC_VOICE_MATRIX1     5U
#define MCLK_SAI2_SEL_CLK_FRAC_COMMON_MATRIX0    6U
#define MCLK_SAI2_SEL_CLK_FRAC_COMMON_MATRIX1    7U
#define MCLK_SAI2_SEL_CLK_FRAC_COMMON_MATRIX2    8U
#define MCLK_SAI2_SEL_SAI0_MCLK_IO_IN            9U
#define MCLK_SAI2_SEL_SAI1_MCLK_IO_IN            10U
#define MCLK_SAI2_SEL_SAI2_MCLK_IO_IN            11U
#define MCLK_SAI2_SEL_SAI3_MCLK_IO_IN            12U

// CRU_CLKSEL_CON52(Offset:0x3D0)
#define MCLK_SAI3_SRC_DIV                        0x08000034
#define MCLK_SAI3_SRC_SEL                        0x04080034
#define MCLK_SAI3_SRC_SEL_XIN_OSC0_FUNC_GATE     0U
#define MCLK_SAI3_SRC_SEL_CLK_INT_VOICE_MATRIX0  1U
#define MCLK_SAI3_SRC_SEL_CLK_INT_VOICE_MATRIX1  2U
#define MCLK_SAI3_SRC_SEL_CLK_INT_VOICE_MATRIX2  3U
#define MCLK_SAI3_SRC_SEL_CLK_FRAC_VOICE_MATRIX0 4U
#define MCLK_SAI3_SRC_SEL_CLK_FRAC_VOICE_MATRIX1 5U
#define MCLK_SAI3_SRC_SEL_CLK_FRAC_COMMON_MATRIX0 6U
#define MCLK_SAI3_SRC_SEL_CLK_FRAC_COMMON_MATRIX1 7U
#define MCLK_SAI3_SRC_SEL_CLK_FRAC_COMMON_MATRIX2 8U
#define MCLK_SAI3_SRC_SEL_SAI0_MCLK_IO_IN        9U
#define MCLK_SAI3_SRC_SEL_SAI1_MCLK_IO_IN        10U
#define MCLK_SAI3_SRC_SEL_SAI2_MCLK_IO_IN        11U
#define MCLK_SAI3_SRC_SEL_SAI3_MCLK_IO_IN        12U

// CRU_CLKSEL_CON53(Offset:0x3D4)
#define MCLK_SAI4_SRC_DIV                        0x08000035
#define MCLK_SAI4_SRC_SEL                        0x04080035
#define MCLK_SAI4_SRC_SEL_XIN_OSC0_FUNC_GATE     0U
#define MCLK_SAI4_SRC_SEL_CLK_INT_VOICE_MATRIX0  1U
#define MCLK_SAI4_SRC_SEL_CLK_INT_VOICE_MATRIX1  2U
#define MCLK_SAI4_SRC_SEL_CLK_INT_VOICE_MATRIX2  3U
#define MCLK_SAI4_SRC_SEL_CLK_FRAC_VOICE_MATRIX0 4U
#define MCLK_SAI4_SRC_SEL_CLK_FRAC_VOICE_MATRIX1 5U
#define MCLK_SAI4_SRC_SEL_CLK_FRAC_COMMON_MATRIX0 6U
#define MCLK_SAI4_SRC_SEL_CLK_FRAC_COMMON_MATRIX1 7U
#define MCLK_SAI4_SRC_SEL_CLK_FRAC_COMMON_MATRIX2 8U
#define MCLK_SAI4_SRC_SEL_SAI0_MCLK_IO_IN        9U
#define MCLK_SAI4_SRC_SEL_SAI1_MCLK_IO_IN        10U
#define MCLK_SAI4_SRC_SEL_SAI2_MCLK_IO_IN        11U
#define MCLK_SAI4_SRC_SEL_SAI3_MCLK_IO_IN        12U

// CRU_CLKSEL_CON54(Offset:0x3D8)
#define CLK_SARADC_DIV                           0x04000036
#define SCLK_UART5_DIV                           0x05060036
#define CLK_SARADC_SEL                           0x02040036
#define CLK_SARADC_SEL_XIN_OSC0_FUNC             0U
#define CLK_SARADC_SEL_CLK_RC                    1U
#define CLK_SARADC_SEL_CLK_DEEPSLOW              2U
#define SCLK_UART5_SEL                           0x030B0036
#define SCLK_UART5_SEL_XIN_OSC0_FUNC             0U
#define SCLK_UART5_SEL_CLK_GPLL_MUX_GATE         1U
#define SCLK_UART5_SEL_CLK_V0PLL_MUX_GATE        2U
#define SCLK_UART5_SEL_CLK_FRAC_UART_MATRIX0     3U
#define SCLK_UART5_SEL_CLK_FRAC_UART_MATRIX1     4U
#define SCLK_UART5_SEL_CLK_FRAC_COMMON_MATRIX0   5U
#define SCLK_UART5_SEL_CLK_FRAC_COMMON_MATRIX1   6U
#define SCLK_UART5_SEL_CLK_FRAC_COMMON_MATRIX2   7U

// CRU_CLKSEL_CON55(Offset:0x3DC)
#define CLK_MAC_PTP_ROOT_DIV                     0x05000037
#define CLK_MAC_PTP_ROOT_SEL                     0x02050037
#define CLK_MAC_PTP_ROOT_SEL_CLK_GPLL_MUX        0U
#define CLK_MAC_PTP_ROOT_SEL_CLK_V0PLL_MUX       1U
#define CLK_MAC_PTP_ROOT_SEL_CLK_V1PLL_MUX       2U

// CRU_CLKSEL_CON58(Offset:0x3E8)
#define ACLK_VIO_ROOT_DIV                        0x0500003A
#define HCLK_VIO_ROOT_DIV                        0x0507003A
#define ACLK_VIO_ROOT_SEL                        0x0205003A
#define ACLK_VIO_ROOT_SEL_CLK_GPLL_MUX_GATE      0U
#define ACLK_VIO_ROOT_SEL_CLK_V0PLL_MUX_GATE     1U
#define ACLK_VIO_ROOT_SEL_CLK_V1PLL_MUX_GATE     2U
#define HCLK_VIO_ROOT_SEL                        0x020C003A
#define HCLK_VIO_ROOT_SEL_CLK_GPLL_DIV           0U
#define HCLK_VIO_ROOT_SEL_CLK_V0PLL_DIV          1U
#define HCLK_VIO_ROOT_SEL_CLK_V1PLL_DIV          2U

// CRU_CLKSEL_CON59(Offset:0x3EC)
#define CLK_CORE_RGA_DIV                         0x0500003B
#define CLK_CORE_RGA_SEL                         0x0205003B
#define CLK_CORE_RGA_SEL_CLK_GPLL_MUX_GATE       0U
#define CLK_CORE_RGA_SEL_CLK_V0PLL_MUX_GATE      1U
#define CLK_CORE_RGA_SEL_CLK_V1PLL_MUX_GATE      2U

// CRU_CLKSEL_CON60(Offset:0x3F0)
#define DCLK_VOP_DIV                             0x0800003C
#define DCLK_VOP_SEL                             0x0308003C
#define DCLK_VOP_SEL_XIN_OSC0_FUNC_GATE          0U
#define DCLK_VOP_SEL_CLK_GPLL_MUX_GATE           1U
#define DCLK_VOP_SEL_CLK_V0PLL_MUX_GATE          2U
#define DCLK_VOP_SEL_CLK_V1PLL_MUX_GATE          3U
#define DCLK_VOP_SEL_CLK_FRAC_VOICE_MATRIX1      4U
#define DCLK_VOP_SEL_CLK_FRAC_COMMON_MATRIX0     5U
#define DCLK_VOP_SEL_CLK_FRAC_COMMON_MATRIX1     6U
#define DCLK_VOP_SEL_CLK_FRAC_COMMON_MATRIX2     7U

// CRU_CLKSEL_CON61(Offset:0x3F4)
#define CLK_TSADC_DIV                            0x0800003D
#define CLK_TSADC_TSEN_DIV                       0x0308003D

// ======================== SCRU module definition bank=1 =========================
// SCRU_SOFTRST_CON00(Offset:0x80)
#define SRST_PRESETN_OTPC_S                      0x00001000
#define SRST_RESETN_SBPI_OTPC_S                  0x00001001
#define SRST_RESETN_USER_OTPC_S                  0x00001002
#define SRST_PRESETN_OTP_MASK                    0x00001003

// SCRU_SOFTRST_CON02(Offset:0x88)
#define SRST_HRESETN_BOOTROM                     0x00001020
#define SRST_PRESETN_DDR_SERVICE                 0x00001021
#define SRST_HRESETN_CRYPTO_S                    0x00001022
#define SRST_HRESETN_KEYLAD                      0x00001023
#define SRST_RESETN_CORE_CRYPTO                  0x00001024
#define SRST_RESETN_PKA_CRYPTO                   0x00001025
#define SRST_ARESETN_CRYPTO                      0x00001026
#define SRST_HRESETN_RNG_S                       0x00001027
#define SRST_PRESETN_TIMER1                      0x00001029
#define SRST_RESETN_TIMER1_CH0                   0x0000102A
#define SRST_RESETN_TIMER1_CH1                   0x0000102B
#define SRST_RESETN_TIMER1_CH2                   0x0000102C
#define SRST_RESETN_TIMER1_CH3                   0x0000102D
#define SRST_RESETN_TIMER1_CH4                   0x0000102E
#define SRST_RESETN_TIMER1_CH5                   0x0000102F

// SCRU_GATE_CON00(Offset:0x40)
#define PCLK_OTPC_S_GATE                         0x00001000
#define CLK_SBPI_OTPC_S_GATE                     0x00001001
#define PCLK_OTP_MASK_GATE                       0x00001003
#define PCLK_KEYREADER_GATE                      0x00001005

// SCRU_GATE_CON02(Offset:0x48)
#define HCLK_BOOTROM_GATE                        0x00001020
#define PCLK_DDR_SERVICE_GATE                    0x00001021
#define HCLK_CRYPTO_S_GATE                       0x00001022
#define HCLK_KEYLAD_GATE                         0x00001023
#define CLK_CORE_CRYPTO_S_GATE                   0x00001024
#define CLK_PKA_CRYPTO_S_GATE                    0x00001025
#define ACLK_CRYPTO_S_GATE                       0x00001026
#define HCLK_RNG_S_GATE                          0x00001027
#define PCLK_BUS_SGRF_GATE                       0x00001028
#define PCLK_TIMER1_GATE                         0x00001029
#define CLK_TIMER1_CH0_GATE                      0x0000102A
#define CLK_TIMER1_CH1_GATE                      0x0000102B
#define CLK_TIMER1_CH2_GATE                      0x0000102C
#define CLK_TIMER1_CH3_GATE                      0x0000102D
#define CLK_TIMER1_CH4_GATE                      0x0000102E
#define CLK_TIMER1_CH5_GATE                      0x0000102F

// SCRU_CLKSEL_CON00(Offset:0x10)
#define CLK_CORE_CRYPTO_DIV                      0x05000100
#define CLK_PKA_CRYPTO_DIV                       0x05070100
#define CLK_CORE_CRYPTO_SEL                      0x02050100
#define CLK_CORE_CRYPTO_SEL_CLK_GPLL_DIV         0U
#define CLK_CORE_CRYPTO_SEL_CLK_V0PLL_DIV        1U
#define CLK_CORE_CRYPTO_SEL_CLK_V1PLL_DIV        2U
#define CLK_PKA_CRYPTO_SEL                       0x020C0100
#define CLK_PKA_CRYPTO_SEL_CLK_GPLL_MUX_GATE     0U
#define CLK_PKA_CRYPTO_SEL_CLK_V0PLL_MUX_GATE    1U
#define CLK_PKA_CRYPTO_SEL_CLK_V1PLL_MUX_GATE    2U

// SCRU_CLKSEL_CON01(Offset:0x14)
#define CLK_TIMER1_CH0_SEL                       0x03000101
#define CLK_TIMER1_CH0_SEL_XIN_OSC0_FUNC         0U
#define CLK_TIMER1_CH0_SEL_CLK_GPLL_DIV_100M     1U
#define CLK_TIMER1_CH0_SEL_CLK_DEEPSLOW          2U
#define CLK_TIMER1_CH0_SEL_CLK_CORE_PVTPLL_LOGIC 3U
#define CLK_TIMER1_CH0_SEL_SAI0_MCLK_IO_IN       4U
#define CLK_TIMER1_CH0_SEL_SAI0_SCLK_IO_IN       5U
#define CLK_TIMER1_CH1_SEL                       0x03030101
#define CLK_TIMER1_CH1_SEL_XIN_OSC0_FUNC         0U
#define CLK_TIMER1_CH1_SEL_CLK_GPLL_DIV_100M     1U
#define CLK_TIMER1_CH1_SEL_CLK_DEEPSLOW          2U
#define CLK_TIMER1_CH1_SEL_CLK_CORE_PVTPLL_LOGIC 3U
#define CLK_TIMER1_CH1_SEL_SAI1_MCLK_IO_IN       4U
#define CLK_TIMER1_CH1_SEL_SAI1_SCLK_IO_IN       5U
#define CLK_TIMER1_CH2_SEL                       0x03060101
#define CLK_TIMER1_CH2_SEL_XIN_OSC0_FUNC         0U
#define CLK_TIMER1_CH2_SEL_CLK_GPLL_DIV_100M     1U
#define CLK_TIMER1_CH2_SEL_CLK_DEEPSLOW          2U
#define CLK_TIMER1_CH2_SEL_CLK_CORE_PVTPLL_LOGIC 3U
#define CLK_TIMER1_CH2_SEL_SAI2_MCLK_IO_IN       4U
#define CLK_TIMER1_CH2_SEL_SAI2_SCLK_IO_IN       5U
#define CLK_TIMER1_CH3_SEL                       0x03090101
#define CLK_TIMER1_CH3_SEL_XIN_OSC0_FUNC         0U
#define CLK_TIMER1_CH3_SEL_CLK_GPLL_DIV_100M     1U
#define CLK_TIMER1_CH3_SEL_CLK_DEEPSLOW          2U
#define CLK_TIMER1_CH3_SEL_CLK_CORE_PVTPLL_LOGIC 3U
#define CLK_TIMER1_CH3_SEL_SAI3_MCLK_IO_IN       4U
#define CLK_TIMER1_CH3_SEL_SAI3_SCLK_IO_IN       5U
#define CLK_TIMER1_CH4_SEL                       0x030C0101
#define CLK_TIMER1_CH4_SEL_XIN_OSC0_FUNC         0U
#define CLK_TIMER1_CH4_SEL_CLK_GPLL_DIV_100M     1U
#define CLK_TIMER1_CH4_SEL_CLK_DEEPSLOW          2U
#define CLK_TIMER1_CH4_SEL_CLK_CORE_PVTPLL_LOGIC 3U
#define CLK_TIMER1_CH4_SEL_MCLK_ASRC0            4U

// SCRU_CLKSEL_CON02(Offset:0x18)
#define CLK_TIMER1_CH5_SEL                       0x03000102
#define CLK_TIMER1_CH5_SEL_XIN_OSC0_FUNC         0U
#define CLK_TIMER1_CH5_SEL_CLK_GPLL_DIV_100M     1U
#define CLK_TIMER1_CH5_SEL_CLK_DEEPSLOW          2U
#define CLK_TIMER1_CH5_SEL_CLK_CORE_PVTPLL_LOGIC 3U
#define CLK_TIMER1_CH5_SEL_MCLK_ASRC1            4U

// ======================= CRU_PMU module definition bank=2 =======================
// CRU_PMU_SOFTRST_CON00(Offset:0xA00)
#define SRST_PRESETN_PMU_GRF                     0x00002005
#define SRST_PRESETN_GPIO0_IOC                   0x00002007
#define SRST_PRESETN_GPIO0                       0x00002008
#define SRST_DBRESETN_GPIO0                      0x00002009
#define SRST_PRESETN_GPIO1_SHADOW                0x0000200A
#define SRST_DBRESETN_GPIO1_SHADOW               0x0000200B
#define SRST_PRESETN_PMU_HP_TIMER                0x0000200C
#define SRST_RESETN_PMU_HP_TIMER                 0x0000200D
#define SRST_RESETN_PMU_HP_TIMER_DEEPSLOW        0x0000200E
#define SRST_PRESETN_PWM0                        0x0000200F

// CRU_PMU_SOFTRST_CON01(Offset:0xA04)
#define SRST_RESETN_PWM0                         0x00002010
#define SRST_RESETN_DDRPHY                       0x0000201B
#define SRST_PRESETN_TOUCH_KEY                   0x0000201C
#define SRST_RESETN_TOUCH_KEY                    0x0000201D

// CRU_PMU_GATE_CON00(Offset:0x800)
#define CLK_PMU_GATE                             0x00002001
#define PCLK_PMU_GATE                            0x00002002
#define CLK_PMU_DEEPSLOW_GATE                    0x00002003
#define PCLK_PMU_CRU_GATE                        0x00002004
#define PCLK_PMU_GRF_GATE                        0x00002005
#define PCLK_GPIO0_IOC_GATE                      0x00002007
#define PCLK_GPIO0_GATE                          0x00002008
#define DBCLK_GPIO0_GATE                         0x00002009
#define PCLK_GPIO1_SHADOW_GATE                   0x0000200A
#define DBCLK_GPIO1_SHADOW_GATE                  0x0000200B
#define PCLK_PMU_HP_TIMER_GATE                   0x0000200C
#define CLK_PMU_HP_TIMER_GATE                    0x0000200D
#define CLK_PMU_HP_TIMER_DEEPSLOW_GATE           0x0000200E
#define PCLK_PWM0_GATE                           0x0000200F

// CRU_PMU_GATE_CON01(Offset:0x804)
#define CLK_PWM0_GATE                            0x00002010
#define CLK_OSC_PWM0_GATE                        0x00002011
#define CLK_RC_PWM0_GATE                         0x00002012
#define CLK_MAC_OUT_GATE                         0x00002013
#define CLK_REF_OUT0_GATE                        0x00002014
#define CLK_REF_OUT1_GATE                        0x00002015
#define CLK_DEEPSLOW_FRAC_DIV_GATE               0x00002016
#define CLK_DEEPSLOW_RC_GATE                     0x00002017
#define CLK_DEEPSLOW_GATE                        0x00002018
#define CLK_DEEPSLOW_PMU_GATE                    0x00002019
#define CLK_UART_JTAG_GATE                       0x0000201A
#define CLK_DDRPHY_GATE                          0x0000201B
#define PCLK_TOUCH_KEY_GATE                      0x0000201C
#define CLK_TOUCH_KEY_GATE                       0x0000201D
#define CLK_REF_PHY_PLL_GATE                     0x0000201E

// CRU_PMU_GATE_CON02(Offset:0x808)
#define CLK_WIFI_OUT_GATE                        0x00002020

// CRU_PMU_CLKSEL_CON00(Offset:0x300)
#define CLK_PWM0_DIV                             0x04060200
#define CLK_MAC_OUT_DIV                          0x060A0200
#define DBCLK_GPIO0_SEL                          0x02000200
#define DBCLK_GPIO0_SEL_XIN_OSC0_FUNC            0U
#define DBCLK_GPIO0_SEL_CLK_RC                   1U
#define DBCLK_GPIO0_SEL_CLK_DEEPSLOW_PMU         2U
#define DBCLK_GPIO1_SHADOW_SEL                   0x02020200
#define DBCLK_GPIO1_SHADOW_SEL_XIN_OSC0_FUNC     0U
#define DBCLK_GPIO1_SHADOW_SEL_CLK_RC            1U
#define DBCLK_GPIO1_SHADOW_SEL_CLK_DEEPSLOW_PMU  2U
#define CLK_PMU_HP_TIMER_SEL                     0x02040200
#define CLK_PMU_HP_TIMER_SEL_XIN_OSC0_FUNC       0U
#define CLK_PMU_HP_TIMER_SEL_CLK_GPLL_DIV_100M   1U
#define CLK_PMU_HP_TIMER_SEL_CLK_CORE_PVTPLL_LOGIC 2U

// CRU_PMU_CLKSEL_CON01(Offset:0x304)
#define CLK_REF_OUT0_DIV                         0x06000201
#define CLK_REF_OUT1_DIV                         0x06080201
#define CLK_REF_OUT0_SEL                         0x02060201
#define CLK_REF_OUT0_SEL_XIN_OSC0_FUNC           0U
#define CLK_REF_OUT0_SEL_CLK_GPLL                1U
#define CLK_REF_OUT0_SEL_CLK_V0PLL               2U
#define CLK_REF_OUT0_SEL_CLK_V1PLL               3U
#define CLK_REF_OUT1_SEL                         0x020E0201
#define CLK_REF_OUT1_SEL_XIN_OSC0_FUNC           0U
#define CLK_REF_OUT1_SEL_CLK_GPLL                1U
#define CLK_REF_OUT1_SEL_CLK_V0PLL               2U
#define CLK_REF_OUT1_SEL_CLK_V1PLL               3U

// CRU_PMU_CLKSEL_CON02(Offset:0x308)
#define CLK_DEEPSLOW_FRAC_DIV                    0x20000202

// CRU_PMU_CLKSEL_CON03(Offset:0x30C)
#define CLK_DEEPSLOW_RC_DIV                      0x05020203
#define CLK_DEEPSLOW_FRAC_SEL                    0x02000203
#define CLK_DEEPSLOW_FRAC_SEL_XIN_OSC0_FUNC      0U
#define CLK_DEEPSLOW_FRAC_SEL_CLK_V0PLL          1U
#define CLK_DEEPSLOW_FRAC_SEL_CLK_V1PLL          2U
#define CLK_DEEPSLOW_FRAC_SEL_CLK_RC             3U
#define CLK_DEEPSLOW_SEL                         0x02070203
#define CLK_DEEPSLOW_SEL_CLK_RTC_DEEPSLOW_IO     0U
#define CLK_DEEPSLOW_SEL_CLK_DEEPSLOW_RC         1U
#define CLK_DEEPSLOW_SEL_CLK_DEEPSLOW_FRAC       2U
#define CLK_DEEPSLOW_PMU_SEL                     0x02090203
#define CLK_DEEPSLOW_PMU_SEL_CLK_RTC_DEEPSLOW_IO 0U
#define CLK_DEEPSLOW_PMU_SEL_CLK_DEEPSLOW_RC     1U
#define CLK_DEEPSLOW_PMU_SEL_CLK_DEEPSLOW_FRAC   2U

// CRU_PMU_CLKSEL_CON04(Offset:0x310)
#define CLK_DDRPHY_DIV                           0x04000204
#define CLK_REF_PHY_PLL_DIV                      0x07060204
#define CLK_DDRPHY_SEL                           0x02040204
#define CLK_DDRPHY_SEL_CLK_GPLL                  0U
#define CLK_DDRPHY_SEL_CLK_V0PLL                 1U
#define CLK_DDRPHY_SEL_CLK_V1PLL                 2U
#define CLK_REF_PHY_PLL_SEL                      0x020D0204
#define CLK_REF_PHY_PLL_SEL_CLK_GPLL             0U
#define CLK_REF_PHY_PLL_SEL_CLK_V0PLL            1U
#define CLK_REF_PHY_PLL_SEL_CLK_V1PLL            2U
#define CLK_REF_PHY_PMU_MUX_SEL                  0x010F0204
#define CLK_REF_PHY_PMU_MUX_SEL_XIN_OSC0_FUNC_PHY 0U
#define CLK_REF_PHY_PMU_MUX_SEL_CLK_REF_PHY_PLL  1U

// CRU_PMU_CLKSEL_CON06(Offset:0x318)
#define CLK_V0PLL_REF_SEL                        0x01000206
#define CLK_V0PLL_REF_SEL_XIN_OSC0_FUNC          0U
#define CLK_V0PLL_REF_SEL_CLK_PLL_REF_IO         1U
#define CLK_V1PLL_REF_SEL                        0x01010206
#define CLK_V1PLL_REF_SEL_XIN_OSC0_FUNC          0U
#define CLK_V1PLL_REF_SEL_CLK_PLL_REF_IO         1U

// ====================== SCRU_PMU module definition bank=3 =======================
// SCRU_PMU_GATE_CON00(Offset:0x40)
#define PCLK_PMU_SGRF_GATE                       0x00003000

#endif /* __RK3506_CRU_H */
