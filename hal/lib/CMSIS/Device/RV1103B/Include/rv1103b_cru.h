/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#ifndef __RV1103B_CRU_H
#define __RV1103B_CRU_H

// ======================= PERICRU module definition bank=0 =======================
// PERICRU_SOFTRST_CON00(Offset:0xA00)
#define SRST_ARESETN_PERI_BIU                    0x00000002
#define SRST_HRESETN_HPMCU_BIU                   0x00000003
#define SRST_LSRESETN_PERI_BIU                   0x00000004
#define SRST_PRESETN_PERI_BIU                    0x00000005
#define SRST_PRESETN_RTC_BIU                     0x00000006
#define SRST_HRESETN_BOOTROM                     0x00000007

// PERICRU_SOFTRST_CON01(Offset:0xA04)
#define SRST_PRESETN_TIMER                       0x00000010
#define SRST_RESETN_TIMER0                       0x00000011
#define SRST_RESETN_TIMER1                       0x00000012
#define SRST_RESETN_TIMER2                       0x00000013
#define SRST_RESETN_TIMER3                       0x00000014
#define SRST_RESETN_TIMER4                       0x00000015
#define SRST_RESETN_TIMER5                       0x00000016
#define SRST_PRESETN_STIMER                      0x00000017
#define SRST_RESETN_STIMER0                      0x00000018
#define SRST_RESETN_STIMER1                      0x00000019

// PERICRU_SOFTRST_CON02(Offset:0xA08)
#define SRST_PRESETN_WDT_NS                      0x00000020
#define SRST_TRESETN_WDT_NS                      0x00000021
#define SRST_PRESETN_WDT_S                       0x00000022
#define SRST_TRESETN_WDT_S                       0x00000023
#define SRST_PRESETN_WDT_HPMCU                   0x00000024
#define SRST_TRESETN_WDT_HPMCU                   0x00000025
#define SRST_PRESETN_I2C1                        0x00000026
#define SRST_RESETN_I2C1                         0x00000027
#define SRST_PRESETN_I2C2                        0x00000028
#define SRST_RESETN_I2C2                         0x00000029
#define SRST_PRESETN_I2C3                        0x0000002A
#define SRST_RESETN_I2C3                         0x0000002B
#define SRST_PRESETN_I2C4                        0x0000002C
#define SRST_RESETN_I2C4                         0x0000002D

// PERICRU_SOFTRST_CON03(Offset:0xA0C)
#define SRST_PRESETN_UART2                       0x00000030
#define SRST_SRESETN_UART2                       0x00000031
#define SRST_PRESETN_UART1                       0x00000032
#define SRST_SRESETN_UART1                       0x00000033
#define SRST_PRESETN_SPI0                        0x0000003A
#define SRST_RESETN_SPI0                         0x0000003B

// PERICRU_SOFTRST_CON04(Offset:0xA10)
#define SRST_PRESETN_PWM1                        0x00000046
#define SRST_RESETN_PWM1                         0x00000047
#define SRST_PRESETN_PWM2                        0x0000004C
#define SRST_RESETN_PWM2                         0x0000004D

// PERICRU_SOFTRST_CON05(Offset:0xA14)
#define SRST_ARESETN_RKDMA                       0x00000058
#define SRST_PRESETN_TSADC                       0x00000059
#define SRST_RESETN_TSADC                        0x0000005A
#define SRST_PRESETN_SARADC                      0x0000005C
#define SRST_RESETN_SARADC                       0x0000005D

// PERICRU_SOFTRST_CON06(Offset:0xA18)
#define SRST_RESETN_SARADC_PHY                   0x00000060
#define SRST_PRESETN_RTC_TEST                    0x00000061
#define SRST_PRESETN_GPIO2                       0x00000063
#define SRST_DBRESETN_GPIO2                      0x00000064
#define SRST_PRESETN_IOC_VCCIO6                  0x00000065
#define SRST_PRESETN_PERI_SGRF                   0x00000066
#define SRST_PRESETN_PERI_GRF                    0x00000067
#define SRST_PRESETN_CRU_PERI                    0x00000068
#define SRST_ARESETN_USBOTG                      0x00000069

// PERICRU_SOFTRST_CON07(Offset:0xA1C)
#define SRST_HRESETN_SDMMC1                      0x00000070
#define SRST_HRESETN_SAI                         0x00000071
#define SRST_MRESETN_SAI                         0x00000072

// PERICRU_SOFTRST_CON08(Offset:0xA20)
#define SRST_RESETN_CORE_CRYPTO                  0x00000080
#define SRST_RESETN_PKA_CRYPTO                   0x00000081
#define SRST_ARESETN_CRYPTO                      0x00000082
#define SRST_HRESETN_CRYPTO                      0x00000083
#define SRST_HRESETN_RK_RNG_NS                   0x00000084
#define SRST_HRESETN_RK_RNG_S                    0x00000085
#define SRST_PRESETN_OTPC_NS                     0x00000086
#define SRST_RESETN_SBPI_OTPC_NS                 0x00000088
#define SRST_RESETN_USER_OTPC_NS                 0x00000089
#define SRST_PRESETN_OTPC_S                      0x0000008A
#define SRST_RESETN_SBPI_OTPC_S                  0x0000008C
#define SRST_RESETN_USER_OTPC_S                  0x0000008D
#define SRST_RESETN_OTPC_ARB                     0x0000008E
#define SRST_PRESETN_OTP_MASK                    0x0000008F

// PERICRU_SOFTRST_CON09(Offset:0xA24)
#define SRST_HRESETN_RGA                         0x00000090
#define SRST_ARESETN_RGA                         0x00000091
#define SRST_RESETN_CORE_RGA                     0x00000092
#define SRST_ARESETN_MAC                         0x00000093
#define SRST_RESETN_MACPHY                       0x0000009B

// PERICRU_SOFTRST_CON10(Offset:0xA28)
#define SRST_ARESETN_SPINLOCK                    0x000000A0
#define SRST_HRESETN_CACHE                       0x000000A1
#define SRST_PRESETN_HPMCU_MAILBOX               0x000000A2
#define SRST_PRESETN_HPMCU_INTMUX                0x000000A3
#define SRST_RESETN_HPMCU_FULL_CLUSTER           0x000000A4
#define SRST_RESETN_HPMCU_PWUP                   0x000000A5
#define SRST_RESETN_HPMCU_ONLY_CORE              0x000000A6
#define SRST_TRESETN_HPMCU_JTAG                  0x000000A7

// PERICRU_SOFTRST_CON11(Offset:0xA2C)
#define SRST_DRESETN_DECOM                       0x000000B0
#define SRST_ARESETN_DECOM                       0x000000B1
#define SRST_PRESETN_DECOM                       0x000000B2
#define SRST_ARESETN_SYS_SRAM                    0x000000B3
#define SRST_PRESETN_DMA2DDR                     0x000000B4
#define SRST_ARESETN_DMA2DDR                     0x000000B5
#define SRST_PRESETN_DCF                         0x000000B6
#define SRST_ARESETN_DCF                         0x000000B7
#define SRST_RESETN_USBPHY_POR                   0x000000BC
#define SRST_RESETN_USBPHY_OTG                   0x000000BD

// PERICRU_GATE_CON00(Offset:0x800)
#define PCLK_PERI_ROOT_GATE                      0x00000000
#define CLK_TIMER_ROOT_GATE                      0x00000001
#define ACLK_PERI_BIU_GATE                       0x00000002
#define HCLK_HPMCU_BIU_GATE                      0x00000003
#define LSCLK_PERI_BIU_GATE                      0x00000004
#define PCLK_PERI_BIU_GATE                       0x00000005
#define PCLK_RTC_BIU_GATE                        0x00000006
#define HCLK_BOOTROM_GATE                        0x00000007
#define PCLK_RTC_ROOT_GATE                       0x00000008

// PERICRU_GATE_CON01(Offset:0x804)
#define PCLK_TIMER_GATE                          0x00000010
#define CLK_TIMER0_GATE                          0x00000011
#define CLK_TIMER1_GATE                          0x00000012
#define CLK_TIMER2_GATE                          0x00000013
#define CLK_TIMER3_GATE                          0x00000014
#define CLK_TIMER4_GATE                          0x00000015
#define CLK_TIMER5_GATE                          0x00000016
#define PCLK_STIMER_GATE                         0x00000017
#define CLK_STIMER0_GATE                         0x00000018
#define CLK_STIMER1_GATE                         0x00000019

// PERICRU_GATE_CON02(Offset:0x808)
#define PCLK_WDT_NS_GATE                         0x00000020
#define TCLK_WDT_NS_GATE                         0x00000021
#define PCLK_WDT_S_GATE                          0x00000022
#define TCLK_WDT_S_GATE                          0x00000023
#define PCLK_WDT_HPMCU_GATE                      0x00000024
#define TCLK_WDT_HPMCU_GATE                      0x00000025
#define PCLK_I2C1_GATE                           0x00000026
#define CLK_I2C1_GATE                            0x00000027
#define PCLK_I2C2_GATE                           0x00000028
#define CLK_I2C2_GATE                            0x00000029
#define PCLK_I2C3_GATE                           0x0000002A
#define CLK_I2C3_GATE                            0x0000002B
#define PCLK_I2C4_GATE                           0x0000002C
#define CLK_I2C4_GATE                            0x0000002D

// PERICRU_GATE_CON03(Offset:0x80C)
#define PCLK_UART2_GATE                          0x00000030
#define PCLK_UART1_GATE                          0x00000032
#define PCLK_SPI0_GATE                           0x0000003A

// PERICRU_GATE_CON04(Offset:0x810)
#define PCLK_PWM1_GATE                           0x00000046
#define CLK_OSC_PWM1_GATE                        0x00000048
#define PCLK_PWM2_GATE                           0x0000004C
#define CLK_OSC_PWM2_GATE                        0x0000004D

// PERICRU_GATE_CON05(Offset:0x814)
#define ACLK_RKDMA_GATE                          0x00000058
#define PCLK_TSADC_GATE                          0x00000059
#define CLK_TSADC_GATE                           0x0000005A
#define CLK_TSADC_TSEN_GATE                      0x0000005B
#define PCLK_SARADC_GATE                         0x0000005C
#define CLK_SARADC_GATE                          0x0000005D

// PERICRU_GATE_CON06(Offset:0x818)
#define PCLK_RTC_TEST_GATE                       0x00000061
#define PCLK_GPIO2_GATE                          0x00000063
#define DBCLK_GPIO2_GATE                         0x00000064
#define PCLK_IOC_VCCIO6_GATE                     0x00000065
#define PCLK_PERI_SGRF_GATE                      0x00000066
#define PCLK_PERI_GRF_GATE                       0x00000067
#define PCLK_CRU_PERI_GATE                       0x00000068
#define ACLK_USBOTG_GATE                         0x00000069
#define CLK_REF_USBOTG_GATE                      0x0000006A

// PERICRU_GATE_CON07(Offset:0x81C)
#define HCLK_SDMMC1_GATE                         0x00000070
#define HCLK_SAI_GATE                            0x00000071
#define MCLK_SAI_GATE                            0x00000072

// PERICRU_GATE_CON08(Offset:0x820)
#define ACLK_CRYPTO_GATE                         0x00000082
#define HCLK_CRYPTO_GATE                         0x00000083
#define HCLK_RK_RNG_NS_GATE                      0x00000084
#define HCLK_RK_RNG_S_GATE                       0x00000085
#define PCLK_OTPC_NS_GATE                        0x00000086
#define CLK_OTPC_ROOT_NS_GATE                    0x00000087
#define CLK_SBPI_OTPC_NS_GATE                    0x00000088
#define CLK_USER_OTPC_NS_GATE                    0x00000089
#define PCLK_OTPC_S_GATE                         0x0000008A
#define CLK_OTPC_ROOT_S_GATE                     0x0000008B
#define CLK_SBPI_OTPC_S_GATE                     0x0000008C
#define CLK_USER_OTPC_S_GATE                     0x0000008D
#define CLK_OTPC_ARB_GATE                        0x0000008E
#define PCLK_OTP_MASK_GATE                       0x0000008F

// PERICRU_GATE_CON09(Offset:0x824)
#define HCLK_RGA_GATE                            0x00000090
#define ACLK_RGA_GATE                            0x00000091
#define ACLK_MAC_GATE                            0x00000093
#define PCLK_MAC_GATE                            0x00000094
#define CLK_MACPHY_GATE                          0x0000009B

// PERICRU_GATE_CON10(Offset:0x828)
#define ACLK_SPINLOCK_GATE                       0x000000A0
#define HCLK_CACHE_GATE                          0x000000A1
#define PCLK_HPMCU_MAILBOX_GATE                  0x000000A2
#define PCLK_HPMCU_INTMUX_GATE                   0x000000A3
#define CLK_HPMCU_GATE                           0x000000A4
#define CLK_HPMCU_RTC_GATE                       0x000000A8

// PERICRU_GATE_CON11(Offset:0x82C)
#define DCLK_DECOM_GATE                          0x000000B0
#define ACLK_DECOM_GATE                          0x000000B1
#define PCLK_DECOM_GATE                          0x000000B2
#define ACLK_SYS_SRAM_GATE                       0x000000B3
#define PCLK_DMA2DDR_GATE                        0x000000B4
#define ACLK_DMA2DDR_GATE                        0x000000B5
#define PCLK_DCF_GATE                            0x000000B6
#define ACLK_DCF_GATE                            0x000000B7
#define MCLK_ACODEC_TX_GATE                      0x000000B9
#define CLK_REF_USBPHY_GATE                      0x000000BC
#define CLK_TESTOUT_PERI_GATE                    0x000000BF

// PERICRU_CLKSEL_CON00(Offset:0x300)
#define PCLK_PERI_ROOT_DIV                       0x02000000
#define CLK_TSADC_DIV                            0x05040000
#define CLK_TSADC_TSEN_DIV                       0x050A0000

// PERICRU_CLKSEL_CON01(Offset:0x304)
#define CLK_SARADC_DIV                           0x03000001
#define CLK_USER_OTPC_NS_DIV                     0x03040001
#define CLK_USER_OTPC_S_DIV                      0x03080001

// PERICRU_CLKSEL_CON02(Offset:0x308)
#define MCLK_ACODEC_TX_DIV                       0x03000002
#define PCLK_RTC_ROOT_DIV                        0x040C0002

// PERICRU_CLKSEL_CON03(Offset:0x30C)
#define CLK_TESTOUT_PERI_DIV                     0x05040003
#define CLK_TESTOUT_PERI_SEL                     0x050A0003

// ======================= VEPUCRU module definition bank=1 =======================
// VEPUCRU_SOFTRST_CON00(Offset:0xA00)
#define SRST_ARESETN_VEPU_BIU                    0x00001001
#define SRST_LSRESETN_VEPU_BIU                   0x00001002
#define SRST_RESETN_REF_PVTPLL_VEPU              0x00001003
#define SRST_HRESETN_VEPU                        0x00001004
#define SRST_ARESETN_VEPU                        0x00001005
#define SRST_RESETN_CORE_VEPU                    0x00001006
#define SRST_PRESETN_VEPU_PVTPLL                 0x00001007
#define SRST_PRESETN_CRU_VEPU                    0x00001008
#define SRST_PRESETN_VEPU_GRF                    0x0000100A
#define SRST_PRESETN_IOC_VCCIO3                  0x0000100B
#define SRST_PRESETN_ACODEC                      0x0000100D
#define SRST_PRESETN_USBPHY                      0x0000100E

// VEPUCRU_GATE_CON00(Offset:0x800)
#define LSCLK_VEPU_ROOT_GATE                     0x00001000
#define ACLK_VEPU_BIU_GATE                       0x00001001
#define LSCLK_VEPU_BIU_GATE                      0x00001002
#define CLK_REF_PVTPLL_VEPU_GATE                 0x00001003
#define HCLK_VEPU_GATE                           0x00001004
#define ACLK_VEPU_GATE                           0x00001005
#define CLK_CORE_VEPU_GATE                       0x00001006
#define PCLK_VEPU_PVTPLL_GATE                    0x00001007
#define PCLK_CRU_VEPU_GATE                       0x00001008
#define PCLK_VEPU_GRF_GATE                       0x0000100A
#define PCLK_IOC_VCCIO3_GATE                     0x0000100B
#define PCLK_ACODEC_GATE                         0x0000100D
#define PCLK_USBPHY_GATE                         0x0000100E
#define CLK_TESTOUT_VEPU_GATE                    0x0000100F

// VEPUCRU_CLKSEL_CON00(Offset:0x300)
#define LSCLK_VEPU_ROOT_DIV                      0x02020100
#define CLK_VEPU_PVTPLL_SRC_SEL                  0x01000100
#define CLK_VEPU_PVTPLL_SRC_SEL_CLK_DEEPSLOW     0U
#define CLK_VEPU_PVTPLL_SRC_SEL_CLK_VEPU_PVTPLL  1U
#define CLK_CORE_VEPU_SEL                        0x01010100
#define CLK_CORE_VEPU_SEL_CLK_VEPU_GPLL_SRC      0U
#define CLK_CORE_VEPU_SEL_CLK_VEPU_PVTPLL_SRC    1U
#define MBIST_CLK_CLK_CORE_VEPU_SEL              0x01040100
#define MBIST_CLK_CLK_CORE_VEPU_SEL_MBIST_CLK_VEPU 0U
#define MBIST_CLK_CLK_CORE_VEPU_SEL_CLK_VEPU_PVTPLL_SRC 1U

// VEPUCRU_CLKSEL_CON02(Offset:0x308)
#define CLK_TESTOUT_VEPU_DIV                     0x05040102
#define CLK_TESTOUT_VEPU_SEL                     0x020A0102
#define CLK_TESTOUT_VEPU_SEL_CLK_CORE_VEPU       0U
#define CLK_TESTOUT_VEPU_SEL_ACLK_VEPU_ROOT      1U
#define CLK_TESTOUT_VEPU_SEL_LSCLK_VEPU_ROOT     2U
#define CLK_TESTOUT_VEPU_SEL_TEST_CLKOUT_VEPU_PVTPLL 3U

// ======================= NPUCRU module definition bank=2 ========================
// NPUCRU_SOFTRST_CON00(Offset:0xA00)
#define SRST_RESETN_REF_PVTPLL_NPU               0x00002000
#define SRST_ARESETN_NPU_BIU                     0x00002002
#define SRST_LSRESETN_NPU_BIU                    0x00002003
#define SRST_HRESETN_RKNN                        0x00002004
#define SRST_ARESETN_RKNN                        0x00002005
#define SRST_PRESETN_NPU_PVTPLL                  0x00002006
#define SRST_PRESETN_CRU_NPU                     0x00002007
#define SRST_PRESETN_NPU_GRF                     0x00002009

// NPUCRU_GATE_CON00(Offset:0x800)
#define CLK_REF_PVTPLL_NPU_GATE                  0x00002000
#define ACLK_NPU_ROOT_GATE                       0x00002001
#define ACLK_NPU_BIU_GATE                        0x00002002
#define LSCLK_NPU_BIU_GATE                       0x00002003
#define HCLK_RKNN_GATE                           0x00002004
#define ACLK_RKNN_GATE                           0x00002005
#define PCLK_NPU_PVTPLL_GATE                     0x00002006
#define PCLK_CRU_NPU_GATE                        0x00002007
#define PCLK_NPU_GRF_GATE                        0x00002009
#define CLK_TESTOUT_NPU_GATE                     0x0000200F

// NPUCRU_CLKSEL_CON00(Offset:0x300)
#define CLK_TESTOUT_NPU_DIV                      0x05040200
#define CLK_NPU_PVTPLL_SRC_SEL                   0x01000200
#define CLK_NPU_PVTPLL_SRC_SEL_CLK_DEEPSLOW      0U
#define CLK_NPU_PVTPLL_SRC_SEL_CLK_NPU_PVTPLL    1U
#define ACLK_NPU_ROOT_SEL                        0x01010200
#define ACLK_NPU_ROOT_SEL_CLK_NPU_GPLL_SRC       0U
#define ACLK_NPU_ROOT_SEL_CLK_NPU_PVTPLL_SRC     1U
#define MBIST_CLK_ACLK_NPU_SEL                   0x01020200
#define MBIST_CLK_ACLK_NPU_SEL_MBIST_CLK_NPU     0U
#define MBIST_CLK_ACLK_NPU_SEL_CLK_NPU_PVTPLL_SRC 1U

// NPUCRU_CLKSEL_CON02(Offset:0x308)
#define CLK_TESTOUT_NPU_SEL                      0x020A0202
#define CLK_TESTOUT_NPU_SEL_ACLK_NPU_ROOT        0U
#define CLK_TESTOUT_NPU_SEL_LSCLK_NPU_ROOT       1U
#define CLK_TESTOUT_NPU_SEL_TEST_CLKOUT_NPU_PVTPLL 2U

// ======================== VICRU module definition bank=3 ========================
// VICRU_SOFTRST_CON00(Offset:0xA00)
#define SRST_LSRESETN_VI_BIU                     0x00003001
#define SRST_ARESETN_VI_BIU                      0x00003002
#define SRST_RESETN_REF_PVTPLL_ISP               0x00003003
#define SRST_RESETN_CORE_ISP                     0x00003006

// VICRU_SOFTRST_CON01(Offset:0xA04)
#define SRST_DRESETN_VICAP                       0x00003010
#define SRST_ARESETN_VICAP                       0x00003012
#define SRST_HRESETN_VICAP                       0x00003013
#define SRST_ISP0RESETN_VICAP                    0x00003018
#define SRST_PRESETN_CSI2HOST0                   0x00003019
#define SRST_PRESETN_CSI2HOST1                   0x0000301B
#define SRST_SRESETN_SFC_2X                      0x0000301C
#define SRST_HRESETN_EMMC                        0x0000301D
#define SRST_HRESETN_SFC                         0x0000301E
#define SRST_HRESETN_SFC_XIP                     0x0000301F

// VICRU_SOFTRST_CON02(Offset:0xA08)
#define SRST_HRESETN_SDMMC0                      0x00003020
#define SRST_PRESETN_CSIPHY                      0x00003022
#define SRST_PRESETN_GPIO1                       0x00003023
#define SRST_DBRESETN_GPIO1                      0x00003024
#define SRST_PRESETN_IOC_VCCIO47                 0x00003025
#define SRST_PRESETN_VI_GRF                      0x00003026
#define SRST_PRESETN_CRU_VI                      0x00003028
#define SRST_PRESETN_VI_PVTPLL                   0x00003029

// VICRU_GATE_CON00(Offset:0x800)
#define LSCLK_VI_ROOT_GATE                       0x00003000
#define LSCLK_VI_BIU_GATE                        0x00003001
#define ACLK_VI_BIU_GATE                         0x00003002
#define CLK_REF_PVTPLL_ISP_GATE                  0x00003003
#define HCLK_ISP_GATE                            0x00003004
#define ACLK_ISP_GATE                            0x00003005
#define CLK_CORE_ISP_GATE                        0x00003006

// VICRU_GATE_CON01(Offset:0x804)
#define ACLK_VICAP_GATE                          0x00003012
#define HCLK_VICAP_GATE                          0x00003013
#define ISP0CLK_VICAP_GATE                       0x00003018
#define PCLK_CSI2HOST0_GATE                      0x00003019
#define PCLK_CSI2HOST1_GATE                      0x0000301B
#define HCLK_EMMC_GATE                           0x0000301D
#define HCLK_SFC_GATE                            0x0000301E
#define HCLK_SFC_XIP_GATE                        0x0000301F

// VICRU_GATE_CON02(Offset:0x808)
#define HCLK_SDMMC0_GATE                         0x00003020
#define PCLK_CSIPHY_GATE                         0x00003022
#define PCLK_GPIO1_GATE                          0x00003023
#define DBCLK_GPIO1_GATE                         0x00003024
#define PCLK_IOC_VCCIO47_GATE                    0x00003025
#define PCLK_VI_GRF_GATE                         0x00003026
#define PCLK_CRU_VI_GATE                         0x00003028
#define PCLK_VI_PVTPLL_GATE                      0x00003029
#define CLK_TESTOUT_VI_GATE                      0x0000302F

// VICRU_CLKSEL_CON00(Offset:0x300)
#define CLK_TESTOUT_VI_DIV                       0x04040300
#define CLK_ISP_PVTPLL_SRC_SEL                   0x01000300
#define CLK_ISP_PVTPLL_SRC_SEL_CLK_DEEPSLOW      0U
#define CLK_ISP_PVTPLL_SRC_SEL_CLKSP_PVTPLL      1U
#define CLK_CORE_ISP_SEL                         0x01010300
#define CLK_CORE_ISP_SEL_CLK_ISP_GPLL_SRC        0U
#define CLK_CORE_ISP_SEL_CLK_ISP_PVTPLL_SRC      1U
#define MBIST_CLK_CLK_CORE_ISP_SEL               0x01020300
#define MBIST_CLK_CLK_CORE_ISP_SEL_MBIST_CLK_ISP 0U
#define MBIST_CLK_CLK_CORE_ISP_SEL_CLK_ISP_PVTPLL_SRC 1U
#define LSCLK_VI_ROOT_SEL                        0x01030300
#define LSCLK_VI_ROOT_SEL_CLK_MATRIX_200M_SRC    0U
#define LSCLK_VI_ROOT_SEL_LSCLK_VI_100M          1U
#define CLK_TESTOUT_VI_SEL                       0x040A0300
#define CLK_TESTOUT_VI_SEL_ACLK_VI_ROOT          0U
#define CLK_TESTOUT_VI_SEL_LSCLK_VI_ROOT         1U
#define CLK_TESTOUT_VI_SEL_CLK_CORE_ISP          2U
#define CLK_TESTOUT_VI_SEL_TEST_CLKOUT_VI_PVTPLL 3U
#define CLK_TESTOUT_VI_SEL_DCLK_VICAP            4U
#define CLK_TESTOUT_VI_SEL_SCLK_SFC_2X           5U
#define CLK_TESTOUT_VI_SEL_CCLK_SDMMC0           6U
#define CLK_TESTOUT_VI_SEL_CCLK_EMMC             7U
#define CLK_TESTOUT_VI_SEL_CLK_RXBYTECLKHS_0_IO  8U
#define CLK_TESTOUT_VI_SEL_CLK_RXBYTECLKHS_1_IO  9U

// ======================= CORECRU module definition bank=4 =======================
// CORECRU_SOFTRST_CON00(Offset:0xA00)
#define SRST_RESETN_REF_PVTPLL_CORE              0x00004000
#define SRST_NCOREPORESET                        0x00004001
#define SRST_NCORESET                            0x00004002
#define SRST_NDBGRESET                           0x00004003
#define SRST_NL2RESET                            0x00004004
#define SRST_ARESETN_CORE_BIU                    0x00004005
#define SRST_PRESETN_CORE_BIU                    0x00004006
#define SRST_HRESETN_CORE_BIU                    0x00004007
#define SRST_PRESETN_DBG                         0x00004008
#define SRST_POTRESETN_DBG                       0x00004009
#define SRST_NTRESETN_DBG                        0x0000400A

// CORECRU_SOFTRST_CON01(Offset:0xA04)
#define SRST_PRESETN_CORE_PVTPLL                 0x00004010
#define SRST_PRESETN_CRU_CORE                    0x00004011
#define SRST_PRESETN_CORE_GRF                    0x00004012
#define SRST_PRESETN_DFT2APB                     0x00004014

// CORECRU_GATE_CON00(Offset:0x800)
#define CLK_SCANHS_CLK_CORE_GATE                 0x00004000
#define CLK_REF_PVTPLL_CORE_GATE                 0x00004001
#define CLK_CORE_SRC_GATE                        0x00004002
#define CLK_CORE_ROOT_GATE                       0x00004003
#define CLK_CORE_GATE                            0x00004004
#define ACLK_CORE_ROOT_GATE                      0x00004005
#define PCLK_CORE_ROOT_GATE                      0x00004006
#define ACLK_CORE_BIU_GATE                       0x00004007
#define PCLK_CORE_BIU_GATE                       0x00004008
#define HCLK_CORE_BIU_GATE                       0x00004009
#define PCLK_DBG_GATE                            0x0000400A
#define CLK_TESTOUT_CORE_GATE                    0x0000400F

// CORECRU_GATE_CON01(Offset:0x804)
#define PCLK_CORE_PVTPLL_GATE                    0x00004010
#define PCLK_CRU_CORE_GATE                       0x00004011
#define PCLK_CORE_GRF_GATE                       0x00004012
#define PCLK_DFT2APB_GATE                        0x00004014

// CORECRU_CLKSEL_CON00(Offset:0x300)
#define CLK_TESTOUT_CORE_DIV                     0x05040400
#define CLK_CORE_PVTPLL_SRC_SEL                  0x01000400
#define CLK_CORE_PVTPLL_SRC_SEL_CLK_DEEPSLOW     0U
#define CLK_CORE_PVTPLL_SRC_SEL_CLK_CORE_PVTPLL  1U
#define CLK_CORE_SRC_SEL                         0x01010400
#define CLK_CORE_SRC_SEL_CLK_CORE_GPLL_SRC       0U
#define CLK_CORE_SRC_SEL_CLK_CORE_PVTPLL_SRC     1U
#define MBIST_CLK_A7_NDFT_SEL                    0x01020400
#define MBIST_CLK_A7_NDFT_SEL_MBIST_CLK_CORE     0U
#define MBIST_CLK_A7_NDFT_SEL_CLK_CORE_PVTPLL_SRC 1U
#define CLK_TESTOUT_CORE_SEL                     0x030A0400
#define CLK_TESTOUT_CORE_SEL_CLK_CORE_ROOT       0U
#define CLK_TESTOUT_CORE_SEL_ACLK_CORE_ROOT      1U
#define CLK_TESTOUT_CORE_SEL_PCLK_CORE_ROOT      2U
#define CLK_TESTOUT_CORE_SEL_HCLK_CORE_BIU       3U
#define CLK_TESTOUT_CORE_SEL_TEST_CLKOUT_CORE_PVTPLL 4U
#define CLK_TESTOUT_CORE_SEL_SWCLKTCK_IO         5U

// CORECRU_CLKSEL_CON01(Offset:0x304)
#define CLK_SCANHS_CLK_CORE_DIV2_DIV             0x05000401
#define CLK_SCANHS_CLK_CORE_DIV8_DIV             0x05080401

// CORECRU_CLKSEL_CON02(Offset:0x308)
#define ACLK_CORE_ROOT_DIV                       0x05000402
#define PCLK_CORE_ROOT_DIV                       0x05080402

// ======================= DDRCRU module definition bank=5 ========================
// DDRCRU_SOFTRST_CON00(Offset:0xA00)
#define SRST_LSRESETN_DDR_BIU                    0x00005001
#define SRST_PRESETN_DDRC                        0x00005002
#define SRST_PRESETN_DDRMON                      0x00005003
#define SRST_RESETN_TIMER_DDRMON                 0x00005004
#define SRST_PRESETN_DFICTRL                     0x00005005
#define SRST_PRESETN_DDR_GRF                     0x00005006
#define SRST_PRESETN_CRU_DDR                     0x00005007
#define SRST_HRESETN_DDRPHY                      0x00005008

// DDRCRU_GATE_CON00(Offset:0x800)
#define LSCLK_DDR_ROOT_GATE                      0x00005000
#define LSCLK_DDR_BIU_GATE                       0x00005001
#define PCLK_DDRC_GATE                           0x00005002
#define PCLK_DDRMON_GATE                         0x00005003
#define CLK_TIMER_DDRMON_GATE                    0x00005004
#define PCLK_DFICTRL_GATE                        0x00005005
#define PCLK_DDR_GRF_GATE                        0x00005006
#define PCLK_CRU_DDR_GATE                        0x00005007
#define HCLK_DDRPHY_GATE                         0x00005008
#define CLK_TESTOUT_DDR_GATE                     0x0000500F

// DDRCRU_CLKSEL_CON00(Offset:0x300)
#define CLK_TESTOUT_DDR_DIV                      0x05000500
#define CLK_TESTOUT_DDR_SEL                      0x01080500
#define CLK_TESTOUT_DDR_SEL_CLK_CORE_DDRC        0U
#define CLK_TESTOUT_DDR_SEL_LSCLK_DDR_ROOT       1U

// ====================== SUBDDRCRU module definition bank=6 ======================
// SUBDDRCRU_SOFTRST_CON00(Offset:0xA00)
#define SRST_RESETN_DDR_BIU                      0x00006001
#define SRST_ARESETN_DDRSCH_CPU                  0x00006002
#define SRST_ARESETN_DDRSCH_VI                   0x00006004
#define SRST_ARESETN_DDRSCH_NPVD                 0x00006005
#define SRST_RESETN_CORE_DDRC                    0x00006006
#define SRST_RESETN_DDRMON                       0x00006007
#define SRST_RESETN_DFICTRL                      0x00006008
#define SRST_RESETN_DFI_SCRAMBLE                 0x00006009

// SUBDDRCRU_GATE_CON00(Offset:0x800)
#define CLK_DDR_BIU_GATE                         0x00006001
#define ACLK_DDRSCH_CPU_GATE                     0x00006002
#define ACLK_DDRSCH_VI_GATE                      0x00006004
#define ACLK_DDRSCH_NPVD_GATE                    0x00006005
#define CLK_CORE_DDRC_GATE                       0x00006006
#define CLK_DDRMON_GATE                          0x00006007
#define CLK_DFICTRL_GATE                         0x00006008
#define CLK_DFI_SCRAMBLE_GATE                    0x00006009

// ======================= TOPCRU module definition bank=7 ========================
// TOPCRU_SOFTRST_CON00(Offset:0xA00)
#define SRST_PRESETN_CRU                         0x00007000
#define SRST_PRESETN_CRU_BIU                     0x00007001
#define SRST_RESETN_DDRPHY                       0x0000700C

// TOPCRU_GATE_CON00(Offset:0x800)
#define CLK_MATRIX_050M_SRC_GATE                 0x00007000
#define CLK_MATRIX_100M_SRC_GATE                 0x00007001
#define CLK_MATRIX_200M_SRC_GATE                 0x00007003
#define CLK_MATRIX_300M_SRC_GATE                 0x00007005
#define CLK_MATRIX_400M_SRC_GATE                 0x00007007
#define CLK_MATRIX_480M_SRC_GATE                 0x00007008
#define CLK_MATRIX_600M_SRC_GATE                 0x00007009
#define CLK_DPLL_DDRPHY_GATE                     0x0000700B
#define CLK4X_SCANHS_DDR_SRC_GATE                0x0000700C

// TOPCRU_GATE_CON01(Offset:0x804)
#define CLK_UART0_SRC_GATE                       0x00007010
#define CLK_UART1_SRC_GATE                       0x00007011
#define CLK_UART2_SRC_GATE                       0x00007012
#define CLK_UART0_FRAC_GATE                      0x00007016
#define CLK_UART1_FRAC_GATE                      0x00007017
#define CLK_UART2_FRAC_GATE                      0x00007018
#define CLK_SAI_GATE                             0x0000701C
#define CLK_SAI_FRAC_GATE                        0x0000701D

// TOPCRU_GATE_CON02(Offset:0x808)
#define LSCLK_NPU_SRC_GATE                       0x00007020
#define ACLK_VEPU_SRC_GATE                       0x00007024
#define ACLK_VI_SRC_GATE                         0x00007028
#define DCLK_VICAP_SRC_GATE                      0x00007029
#define CCLK_EMMC_SRC_GATE                       0x0000702A
#define CCLK_SDMMC0_SRC_GATE                     0x0000702B
#define SCLK_SFC_2X_SRC_GATE                     0x0000702C

// TOPCRU_GATE_CON03(Offset:0x80C)
#define LSCLK_PERI_SRC_GATE                      0x00007030
#define ACLK_PERI_SRC_GATE                       0x00007031
#define HCLK_HPMCU_SRC_GATE                      0x00007032
#define SCLK_UART0_SRC_GATE                      0x00007033
#define SCLK_UART1_SRC_GATE                      0x00007034
#define SCLK_UART2_SRC_GATE                      0x00007038

// TOPCRU_GATE_CON04(Offset:0x810)
#define CLK_I2C_PMU_SRC_GATE                     0x00007040
#define CLK_I2C_PERI_SRC_GATE                    0x00007044
#define CLK_SPI0_SRC_GATE                        0x00007045
#define CLK_PWM0_SRC_GATE                        0x0000704A
#define CLK_PWM1_SRC_GATE                        0x0000704B
#define CLK_PWM2_SRC_GATE                        0x0000704C

// TOPCRU_GATE_CON05(Offset:0x814)
#define DCLK_DECOM_SRC_GATE                      0x00007050
#define CCLK_SDMMC1_SRC_GATE                     0x00007051
#define CLK_CORE_CRYPTO_SRC_GATE                 0x00007052
#define CLK_PKA_CRYPTO_SRC_GATE                  0x00007053
#define CLK_CORE_RGA_SRC_GATE                    0x00007054
#define MCLK_SAI_SRC_GATE                        0x00007055
#define CLK_FREQ_PWM0_SRC_GATE                   0x00007056
#define CLK_COUNTER_PWM0_SRC_GATE                0x00007057
#define CLK_NPU_GPLL_SRC_GATE                    0x0000705C
#define CLK_VEPU_GPLL_SRC_GATE                   0x0000705D
#define CLK_ISP_GPLL_SRC_GATE                    0x0000705E
#define CLK_CORE_GPLL_SRC_GATE                   0x0000705F

// TOPCRU_GATE_CON06(Offset:0x818)
#define PCLK_TOP_ROOT_GATE                       0x00007060
#define PCLK_CRU_GATE                            0x00007061
#define PCLK_CRU_BIU_GATE                        0x00007062
#define CLK_REF_MIPI0_SRC_GATE                   0x00007063
#define CLK_MIPI0_OUT2IO_GATE                    0x00007064
#define CLK_REF_MIPI1_SRC_GATE                   0x00007065
#define CLK_MIPI1_OUT2IO_GATE                    0x00007066
#define MCLK_SAI_OUT2IO_GATE                     0x00007069
#define CLK_GPLLTEST_GATE                        0x0000706C
#define CLK_DPLLTEST_GATE                        0x0000706D
#define CLK_TESTOUT_TOP_GATE                     0x0000706F

// TOPCRU_CLKSEL_CON00(Offset:0x300)
#define CLK_MATRIX_050M_SRC_DIV                  0x05000700
#define CLK_MATRIX_100M_SRC_DIV                  0x05050700

// TOPCRU_CLKSEL_CON01(Offset:0x304)
#define CLK_MATRIX_200M_SRC_DIV                  0x05000701
#define CLK_MATRIX_300M_SRC_DIV                  0x050A0701

// TOPCRU_CLKSEL_CON02(Offset:0x308)
#define CLK_MATRIX_400M_SRC_DIV                  0x05000702
#define CLK_MATRIX_480M_SRC_DIV                  0x05050702
#define CLK_MATRIX_600M_SRC_DIV                  0x050A0702

// TOPCRU_CLKSEL_CON05(Offset:0x314)
#define CLK_UART0_SRC_DIV                        0x05000705
#define CLK_UART1_SRC_DIV                        0x05050705
#define CLK_UART2_SRC_DIV                        0x050A0705

// TOPCRU_CLKSEL_CON10(Offset:0x328)
#define CLK_UART0_FRAC_DIV                       0x2000070A

// TOPCRU_CLKSEL_CON11(Offset:0x32C)
#define CLK_UART1_FRAC_DIV                       0x2000070B

// TOPCRU_CLKSEL_CON12(Offset:0x330)
#define CLK_UART2_FRAC_DIV                       0x2000070C

// TOPCRU_CLKSEL_CON20(Offset:0x350)
#define CLK_SAI_SRC_DIV                          0x05000714

// TOPCRU_CLKSEL_CON21(Offset:0x354)
#define CLK_SAI_FRAC_DIV                         0x20000715

// TOPCRU_CLKSEL_CON30(Offset:0x378)
#define LSCLK_NPU_SRC_SEL                        0x0100071E
#define LSCLK_NPU_SRC_SEL_CLK_MATRIX_200M_SRC    0U
#define LSCLK_NPU_SRC_SEL_CLK_MATRIX_100M_SRC    1U
#define ACLK_VEPU_SRC_SEL                        0x0208071E
#define ACLK_VEPU_SRC_SEL_CLK_MATRIX_480M_SRC    0U
#define ACLK_VEPU_SRC_SEL_CLK_MATRIX_400M_SRC    1U
#define ACLK_VEPU_SRC_SEL_CLK_MATRIX_300M_SRC    2U
#define ACLK_VEPU_SRC_SEL_CLK_MATRIX_200M_SRC    3U
#define ACLK_VI_SRC_SEL                          0x020C071E
#define ACLK_VI_SRC_SEL_CLK_MATRIX_480M_SRC      0U
#define ACLK_VI_SRC_SEL_CLK_MATRIX_400M_SRC      1U
#define ACLK_VI_SRC_SEL_CLK_MATRIX_300M_SRC      2U
#define DCLK_VICAP_SRC_SEL                       0x010E071E
#define DCLK_VICAP_SRC_SEL_CLK_MATRIX_300M_SRC   0U
#define DCLK_VICAP_SRC_SEL_CLK_MATRIX_200M_SRC   1U

// TOPCRU_CLKSEL_CON31(Offset:0x37C)
#define CCLK_EMMC_SRC_DIV                        0x0800071F
#define LSCLK_PERI_SRC_SEL                       0x0109071F
#define LSCLK_PERI_SRC_SEL_CLK_MATRIX_300M_SRC   0U
#define LSCLK_PERI_SRC_SEL_CLK_MATRIX_200M_SRC   1U
#define ACLK_PERI_SRC_SEL                        0x020A071F
#define ACLK_PERI_SRC_SEL_CLK_MATRIX_600M_SRC    0U
#define ACLK_PERI_SRC_SEL_CLK_MATRIX_480M_SRC    1U
#define ACLK_PERI_SRC_SEL_CLK_MATRIX_400M_SRC    2U
#define HCLK_HPMCU_SRC_SEL                       0x010C071F
#define HCLK_HPMCU_SRC_SEL_CLK_MATRIX_400M_SRC   0U
#define HCLK_HPMCU_SRC_SEL_CLK_MATRIX_300M_SRC   1U
#define CCLK_EMMC_SRC_SEL                        0x010F071F
#define CCLK_EMMC_SRC_SEL_CLK_GPLL_MUX           0U
#define CCLK_EMMC_SRC_SEL_XIN_OSC0_FUNC          1U

// TOPCRU_CLKSEL_CON32(Offset:0x380)
#define CCLK_SDMMC0_SRC_DIV                      0x08000720
#define SCLK_UART0_SRC_SEL                       0x02080720
#define SCLK_UART0_SRC_SEL_CLK_UART0_SRC         0U
#define SCLK_UART0_SRC_SEL_CLK_UART0_FRAC        1U
#define SCLK_UART0_SRC_SEL_XIN_OSC0_FUNC         2U
#define SCLK_UART1_SRC_SEL                       0x020A0720
#define SCLK_UART1_SRC_SEL_CLK_UART1_SRC         0U
#define SCLK_UART1_SRC_SEL_CLK_UART1_FRAC        1U
#define SCLK_UART1_SRC_SEL_XIN_OSC0_FUNC         2U
#define SCLK_UART2_SRC_SEL                       0x020C0720
#define SCLK_UART2_SRC_SEL_CLK_UART2_SRC         0U
#define SCLK_UART2_SRC_SEL_CLK_UART2_FRAC        1U
#define SCLK_UART2_SRC_SEL_XIN_OSC0_FUNC         2U
#define CCLK_SDMMC0_SRC_SEL                      0x010F0720
#define CCLK_SDMMC0_SRC_SEL_CLK_GPLL_MUX         0U
#define CCLK_SDMMC0_SRC_SEL_XIN_OSC0_FUNC        1U

// TOPCRU_CLKSEL_CON33(Offset:0x384)
#define SCLK_SFC_2X_SRC_DIV                      0x08000721
#define SCLK_SFC_2X_SRC_SEL                      0x010F0721
#define SCLK_SFC_2X_SRC_SEL_CLK_GPLL_MUX         0U
#define SCLK_SFC_2X_SRC_SEL_XIN_OSC0_FUNC        1U

// TOPCRU_CLKSEL_CON34(Offset:0x388)
#define CLK_I2C_PMU_SRC_SEL                      0x01000722
#define CLK_I2C_PMU_SRC_SEL_CLK_MATRIX_100M_SRC  0U
#define CLK_I2C_PMU_SRC_SEL_XIN_OSC0_FUNC        1U
#define CLK_I2C_PERI_SRC_SEL                     0x01010722
#define CLK_I2C_PERI_SRC_SEL_CLK_MATRIX_200M_SRC 0U
#define CLK_I2C_PERI_SRC_SEL_XIN_OSC0_FUNC       1U
#define CLK_SPI0_SRC_SEL                         0x02020722
#define CLK_SPI0_SRC_SEL_CLK_MATRIX_200M_SRC     0U
#define CLK_SPI0_SRC_SEL_CLK_MATRIX_100M_SRC     1U
#define CLK_SPI0_SRC_SEL_CLK_MATRIX_050M_SRC     2U
#define CLK_SPI0_SRC_SEL_XIN_OSC0_FUNC           3U
#define CLK_PWM0_SRC_SEL                         0x010C0722
#define CLK_PWM0_SRC_SEL_CLK_MATRIX_100M_SRC     0U
#define CLK_PWM0_SRC_SEL_XIN_OSC0_FUNC           1U
#define CLK_PWM1_SRC_SEL                         0x010D0722
#define CLK_PWM1_SRC_SEL_CLK_MATRIX_100M_SRC     0U
#define CLK_PWM1_SRC_SEL_XIN_OSC0_FUNC           1U
#define CLK_PWM2_SRC_SEL                         0x010E0722
#define CLK_PWM2_SRC_SEL_CLK_MATRIX_100M_SRC     0U
#define CLK_PWM2_SRC_SEL_XIN_OSC0_FUNC           1U

// TOPCRU_CLKSEL_CON35(Offset:0x38C)
#define DCLK_DECOM_SRC_SEL                       0x02000723
#define DCLK_DECOM_SRC_SEL_CLK_MATRIX_480M_SRC   0U
#define DCLK_DECOM_SRC_SEL_CLK_MATRIX_400M_SRC   1U
#define DCLK_DECOM_SRC_SEL_CLK_MATRIX_300M_SRC   2U
#define CLK_CORE_CRYPTO_SRC_SEL                  0x02020723
#define CLK_CORE_CRYPTO_SRC_SEL_CLK_MATRIX_300M_SRC 0U
#define CLK_CORE_CRYPTO_SRC_SEL_CLK_MATRIX_200M_SRC 1U
#define CLK_CORE_CRYPTO_SRC_SEL_CLK_MATRIX_100M_SRC 2U
#define CLK_PKA_CRYPTO_SRC_SEL                   0x02040723
#define CLK_PKA_CRYPTO_SRC_SEL_CLK_MATRIX_300M_SRC 0U
#define CLK_PKA_CRYPTO_SRC_SEL_CLK_MATRIX_200M_SRC 1U
#define CLK_PKA_CRYPTO_SRC_SEL_CLK_MATRIX_100M_SRC 2U
#define CLK_CORE_RGA_SRC_SEL                     0x01080723
#define CLK_CORE_RGA_SRC_SEL_CLK_MATRIX_400M_SRC 0U
#define CLK_CORE_RGA_SRC_SEL_CLK_MATRIX_300M_SRC 1U
#define MCLK_SAI_SRC_SEL                         0x020A0723
#define MCLK_SAI_SRC_SEL_CLK_SAI_SRC             0U
#define MCLK_SAI_SRC_SEL_CLK_SAI_FRAC            1U
#define MCLK_SAI_SRC_SEL_MCLK_SAI_FROM_IO        2U
#define MCLK_SAI_SRC_SEL_XIN_OSC0_HALF           3U
#define CLK_FREQ_PWM0_SRC_SEL                    0x020C0723
#define CLK_FREQ_PWM0_SRC_SEL_SCLK_SAI_FROM_IO   0U
#define CLK_FREQ_PWM0_SRC_SEL_MCLK_SAI_FROM_IO   1U
#define CLK_FREQ_PWM0_SRC_SEL_CLK_TESTOUT_OUT    2U
#define CLK_COUNTER_PWM0_SRC_SEL                 0x020E0723
#define CLK_COUNTER_PWM0_SRC_SEL_SCLK_SAI_FROM_IO 0U
#define CLK_COUNTER_PWM0_SRC_SEL_MCLK_SAI_FROM_IO 1U
#define CLK_COUNTER_PWM0_SRC_SEL_CLK_TESTOUT_OUT 2U

// TOPCRU_CLKSEL_CON36(Offset:0x390)
#define CCLK_SDMMC1_SRC_DIV                      0x08000724
#define CCLK_SDMMC1_SRC_SEL                      0x010F0724
#define CCLK_SDMMC1_SRC_SEL_CLK_GPLL_MUX         0U
#define CCLK_SDMMC1_SRC_SEL_XIN_OSC0_FUNC        1U

// TOPCRU_CLKSEL_CON37(Offset:0x394)
#define CLK_NPU_GPLL_SRC_DIV                     0x02010725
#define CLK_VEPU_GPLL_SRC_DIV                    0x02050725
#define CLK_ISP_GPLL_SRC_DIV                     0x02090725
#define CLK_CORE_GPLL_SRC_DIV                    0x030D0725
#define CLK_NPU_GPLL_SRC_SEL                     0x01000725
#define CLK_NPU_GPLL_SRC_SEL_CLK_GPLL_MUX        0U
#define CLK_NPU_GPLL_SRC_SEL_XIN_OSC0_FUNC       1U
#define CLK_VEPU_GPLL_SRC_SEL                    0x01040725
#define CLK_VEPU_GPLL_SRC_SEL_CLK_GPLL_MUX       0U
#define CLK_VEPU_GPLL_SRC_SEL_XIN_OSC0_FUNC      1U
#define CLK_ISP_GPLL_SRC_SEL                     0x01080725
#define CLK_ISP_GPLL_SRC_SEL_CLK_GPLL_MUX        0U
#define CLK_ISP_GPLL_SRC_SEL_XIN_OSC0_FUNC       1U
#define CLK_CORE_GPLL_SRC_SEL                    0x010C0725
#define CLK_CORE_GPLL_SRC_SEL_CLK_GPLL_MUX       0U
#define CLK_CORE_GPLL_SRC_SEL_XIN_OSC0_FUNC      1U

// TOPCRU_CLKSEL_CON40(Offset:0x3A0)
#define CLK_REF_MIPI0_SRC_DIV                    0x05000728
#define CLK_REF_MIPI1_SRC_DIV                    0x05080728
#define CLK_MIPI0_OUT2IO_SEL                     0x01060728
#define CLK_MIPI0_OUT2IO_SEL_CLK_REF_MIPI0_SRC   0U
#define CLK_MIPI0_OUT2IO_SEL_XIN_OSC0_FUNC       1U
#define CLK_MIPI1_OUT2IO_SEL                     0x010E0728
#define CLK_MIPI1_OUT2IO_SEL_CLK_REF_MIPI1_SRC   0U
#define CLK_MIPI1_OUT2IO_SEL_XIN_OSC0_FUNC       1U

// TOPCRU_CLKSEL_CON41(Offset:0x3A4)
#define CLK_TESTOUT_TOP_DIV                      0x02080729
#define MCLK_SAI_OUT2IO_DIV                      0x030D0729
#define MCLK_SAI_OUT2IO_SEL                      0x01070729
#define MCLK_SAI_OUT2IO_SEL_MCLK_SAI_SRC         0U
#define MCLK_SAI_OUT2IO_SEL_XIN_OSC0_HALF        1U
#define CLK_TESTOUT_TOP_SEL                      0x030A0729
#define CLK_TESTOUT_TOP_SEL_PCLK_TOP_ROOT        0U
#define CLK_TESTOUT_TOP_SEL_MCLK_SAI_OUT2IO      1U
#define CLK_TESTOUT_TOP_SEL_CLK_MIPI0_OUT2IO     2U
#define CLK_TESTOUT_TOP_SEL_CLK_MIPI1_OUT2IO     3U

//======================= PMUCRU module definition bank=8 ========================
// PMUCRU_SOFTRST_CON00(Offset:0xA00)
#define SRST_PRESETN_PMU_GPIO0                   0x00008004
#define SRST_DBRESETN_PMU_GPIO0                  0x00008005
#define SRST_RESETN_DDR_FAIL_SAFE                0x00008008
#define SRST_PRESETN_PMU_HP_TIMER                0x00008009
#define SRST_RESETN_PMU_HP_TIMER                 0x0000800A
#define SRST_RESETN_PMU_32K_HP_TIMER             0x0000800B
#define SRST_PRESETN_I2C0                        0x0000800C
#define SRST_RESETN_I2C0                         0x0000800D
#define SRST_PRESETN_UART0                       0x0000800E
#define SRST_SRESETN_UART0                       0x0000800F

// PMUCRU_SOFTRST_CON01(Offset:0xA04)
#define SRST_PRESETN_IOC_PMUIO0                  0x00008010
#define SRST_PRESETN_CRU_PMU                     0x00008011
#define SRST_PRESETN_PMU_GRF                     0x00008012
#define SRST_PRESETN_PMU_SGRF                    0x00008013
#define SRST_PRESETN_PMU_SGRF_REMAP              0x00008014
#define SRST_RESETN_PREROLL                      0x00008016
#define SRST_RESETN_PREROLL_32K                  0x00008017
#define SRST_HRESETN_PMU_SRAM                    0x00008018
#define SRST_PRESETN_PWM0                        0x00008019
#define SRST_RESETN_PWM0                         0x0000801A

// PMUCRU_SOFTRST_CON02(Offset:0xA08)
#define SRST_RESETN_LPMCU                        0x00008020
#define SRST_RESETN_LPMCU_PWRUP                  0x00008021
#define SRST_RESETN_LPMCU_CPU                    0x00008022
#define SRST_TRESETN_LPMCU_CPU                   0x00008023

// PMUCRU_GATE_CON00(Offset:0x800)
#define LSCLK_PMU_ROOT_GATE                      0x00008000
#define CLK_PMU_GATE                             0x00008001
#define PCLK_PMU_GATE                            0x00008002
#define XIN_RC_DIV_GATE                          0x00008003
#define PCLK_PMU_GPIO0_GATE                      0x00008004
#define DBCLK_PMU_GPIO0_GATE                     0x00008005
#define CLK_DDR_FAIL_SAFE_GATE                   0x00008008
#define PCLK_PMU_HP_TIMER_GATE                   0x00008009
#define CLK_PMU_HP_TIMER_GATE                    0x0000800A
#define CLK_PMU_32K_HP_TIMER_GATE                0x0000800B
#define PCLK_I2C0_GATE                           0x0000800C
#define CLK_I2C0_GATE                            0x0000800D
#define PCLK_UART0_GATE                          0x0000800E

// PMUCRU_GATE_CON01(Offset:0x804)
#define PCLK_IOC_PMUIO0_GATE                     0x00008010
#define PCLK_CRU_PMU_GATE                        0x00008011
#define PCLK_PMU_GRF_GATE                        0x00008012
#define PCLK_PMU_SGRF_GATE                       0x00008013
#define CLK_REFOUT_GATE                          0x00008014
#define CLK_PREROLL_GATE                         0x00008016
#define CLK_PREROLL_32K_GATE                     0x00008017
#define HCLK_PMU_SRAM_GATE                       0x00008018
#define LSCLK_PMU_32K_GATE                       0x0000801C
#define CLK_TESTOUT_PMU_GATE                     0x0000801E
#define CLK_TESTOUT_OUT_GATE                     0x0000801F

// PMUCRU_GATE_CON02(Offset:0x808)
#define PCLK_PWM0_GATE                           0x00008020
#define CLK_PWM0_GATE                            0x00008021
#define CLK_OSC_PWM0_GATE                        0x00008022
#define CLK_RC_PWM0_GATE                         0x00008023
#define CLK_FREQ_PWM0_GATE                       0x00008024
#define CLK_COUNTER_PWM0_GATE                    0x00008025
#define CLK_LPMCU_PMU_GATE                       0x0000802C

// PMUCRU_CLKSEL_CON00(Offset:0x300)
#define CLK_TESTOUT_PMU_DIV                      0x03040800
#define CLK_DEEPSLOW_SEL                         0x02000800
#define CLK_DEEPSLOW_SEL_XIN_RC_DIV              0U
#define CLK_DEEPSLOW_SEL_CLK_32K_RTC             1U
#define CLK_DEEPSLOW_SEL_CLK_32K_IO              2U
#define XIN_RC_DIV_SEL                           0x01020800
#define XIN_RC_DIV_SEL_XIN_OSC0_FUNC             0U
#define XIN_RC_DIV_SEL_CLK_RC_OSC_IO             1U
#define DBCLK_PMU_GPIO0_SEL                      0x01030800
#define DBCLK_PMU_GPIO0_SEL_XIN_OSC0_FUNC        0U
#define DBCLK_PMU_GPIO0_SEL_CLK_DEEPSLOW         1U
#define CLK_TESTOUT_PMU_SEL                      0x03080800
#define CLK_TESTOUT_PMU_SEL_LSCLK_PMU_ROOT       0U
#define CLK_TESTOUT_PMU_SEL_CLK_RC_400K_DFT      1U
#define CLK_TESTOUT_PMU_SEL_XIN_RC_DIV           2U
#define CLK_TESTOUT_PMU_SEL_CLK_DEEPSLOW         3U
#define CLK_TESTOUT_PMU_SEL_SCLK_UART0_SRC       4U
#define CLK_TESTOUT_PMU_SEL_CLK_I2C_PMU_SRC      5U
#define CLK_TESTOUT_PMU_SEL_CLK_RC_OSC_IO        6U
#define CLK_TESTOUT_PMU_SEL_CLK_PWM0             7U
#define CLK_TESTOUT_OUT_SEL                      0x030D0800
#define CLK_TESTOUT_OUT_SEL_CLK_TESTOUT_TOP      0U
#define CLK_TESTOUT_OUT_SEL_CLK_TESTOUT_CORE     1U
#define CLK_TESTOUT_OUT_SEL_CLK_TESTOUT_DDR      2U
#define CLK_TESTOUT_OUT_SEL_CLK_TESTOUT_PMU      3U
#define CLK_TESTOUT_OUT_SEL_CLK_TESTOUT_PERI     4U
#define CLK_TESTOUT_OUT_SEL_CLK_TESTOUT_NPU      5U
#define CLK_TESTOUT_OUT_SEL_CLK_TESTOUT_VI       6U
#define CLK_TESTOUT_OUT_SEL_CLK_TESTOUT_VEPU     7U

// PMUCRU_CLKSEL_CON01(Offset:0x304)
#define XIN_RC_DIV_DIV                           0x20000801

// PMUCRU_CLKSEL_CON02(Offset:0x308)
#define LSCLK_PMU_ROOT_DIV                       0x02000802
#define LSCLK_PMU_ROOT_SEL                       0x01040802
#define LSCLK_PMU_ROOT_SEL_LSCLK_PMU_32K         0U
#define LSCLK_PMU_ROOT_SEL_CLK_RC_OSC_IO         1U

// ======================= PMU1CRU module definition bank=9 =======================
// PMU1CRU_SOFTRST_CON00(Offset:0xA00)
#define SRST_PRESETN_SPI2AHB                     0x00009000
#define SRST_HRESETN_SPI2AHB                     0x00009001
#define SRST_SRESETN_SPI2AHB                     0x00009002
#define SRST_LSRESETN_PMU_BIU                    0x00009003
#define SRST_PRESETN_WDT_LPMCU                   0x00009009
#define SRST_TRESETN_WDT_LPMCU                   0x0000900A
#define SRST_HRESETN_SFC_PMU1                    0x0000900C
#define SRST_HRESETN_SFC_XIP_PMU1                0x0000900D
#define SRST_SRESETN_SFC_2X_PMU1                 0x0000900E

// PMU1CRU_SOFTRST_CON01(Offset:0xA04)
#define SRST_PRESETN_LPMCU_MAILBOX               0x00009018
#define SRST_PRESETN_IOC_PMUIO1                  0x00009019
#define SRST_PRESETN_CRU_PMU1                    0x0000901A

// PMU1CRU_GATE_CON00(Offset:0x800)
#define PCLK_SPI2AHB_GATE                        0x00009000
#define HCLK_SPI2AHB_GATE                        0x00009001
#define SCLK_SPI2AHB_GATE                        0x00009002
#define LSCLK_PMU_BIU_GATE                       0x00009003
#define PCLK_WDT_LPMCU_GATE                      0x00009009
#define TCLK_WDT_LPMCU_GATE                      0x0000900A
#define HCLK_SFC_PMU1_GATE                       0x0000900C
#define HCLK_SFC_XIP_PMU1_GATE                   0x0000900D
#define SCLK_SFC_2X_PMU1_GATE                    0x0000900E

// PMU1CRU_GATE_CON01(Offset:0x804)
#define CLK_LPMCU_GATE                           0x00009010
#define CLK_LPMCU_RTC_GATE                       0x00009014
#define PCLK_LPMCU_MAILBOX_GATE                  0x00009018
#define PCLK_IOC_PMUIO1_GATE                     0x00009019
#define PCLK_CRU_PMU1_GATE                       0x0000901A
#define XIN_OSC0_DFT_GATE                        0x0000901B

// PMU1CRU_CLKSEL_CON00(Offset:0x300)
#define SCLK_SFC_2X_PMU1_SEL                     0x01080900
#define SCLK_SFC_2X_PMU1_SEL_CLK_MATRIX_100M_SRC 0U
#define SCLK_SFC_2X_PMU1_SEL_CLK_RC_OSC_IO       1U

#endif /* __RV1103B_CRU_H */