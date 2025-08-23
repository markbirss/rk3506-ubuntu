/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#if defined(HAL_PINCTRL_MODULE_ENABLED) && (defined(RKMCU_RK2118) || defined(RK2118_DSP_CORE0) || defined(RK2118_DSP_CORE1) || defined(RK2118_DSP_CORE2))

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup PINCTRL
 *  @{
 */

/** @defgroup PINCTRL_How_To_Use How To Use
 *  @{

 The pinctrl setting registers actually is bus grf registers, which include
 iomux, drive strength, pull mode, slew rate and schmitt trigger.

 The pinctrl driver provides APIs:
   - HAL_PINCTRL_SetIOMUX() to set pin iomux
   - HAL_PINCTRL_SetParam() to set pin iomux/pull/drive strength/slew rate/schmitt trigger
   - HAL_PINCTRL_SetRMIO() to set Rockchip Matrix IO

 Example:

     HAL_PINCTRL_SetIOMUX(GPIO_BANK4,
                          GPIO_PIN_B3,  // UART1_TX
                          PIN_CONFIG_MUX_FUNC1);

     HAL_PINCTRL_SetParam(GPIO_BANK4,
                          GPIO_PIN_B3,  // UART1_TX
                          PIN_CONFIG_MUX_FUNC1 |
                          PIN_CONFIG_PUL_UP |
                          PIN_CONFIG_DRV_LEVEL2);

     Note! Please refer to RK2118 TRM Rockchip Matrix IO Chapter and eRMIO_Name in soc.h
     Use HAL_PINCTRL_SetRMIO for configuration instead of using HAL_PINCTRL_SetIOMUX.

     HAL_PINCTRL_SetRMIO(GPIO_BANK0,
                         GPIO_PIN_A0,
                         RMIO_I2C0_SCL);

 @} */

/** @defgroup PINCTRL_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/

#define _TO_MASK(w)          ((1U << (w)) - 1U)
#define _TO_OFFSET(gp, w)    ((gp) * (w))
#define RK_GEN_VAL(gp, v, w) ((_TO_MASK(w) << (_TO_OFFSET(gp, w) + 16)) | (((v) & _TO_MASK(w)) << _TO_OFFSET(gp, w)))

/*
 * Use HAL_DBG("pinctrl: write val = 0x%" PRIx32 " to register %p\n", VAL, &REG);
 * and HAL_DBG("pinctrl: readback register %p = 0x%" PRIx32 "\n", &REG, REG);
 * for debug
 */
#define _PINCTRL_WRITE(REG, VAL) \
{                                \
    REG = VAL;                   \
}

#define IOMUX_4_BIT_PER_PIN                 (4)
#define IOMUX_4_PIN_PER_REG                 (4)
#define IOMUX_0(__B, __G)                   (GPIO##__B##_IOC->GPIO##__B##__G##_IOMUX_SEL_0)
#define IOMUX_1(__B, __G)                   (GPIO##__B##_IOC->GPIO##__B##__G##_IOMUX_SEL_1)
#define IOMUX_AB_0(__B, __G)                (GPIO##__B##_AB_IOC->GPIO##__B##__G##_IOMUX_SEL_0)
#define IOMUX_AB_1(__B, __G)                (GPIO##__B##_AB_IOC->GPIO##__B##__G##_IOMUX_SEL_1)
#define IOMUX_CD_0(__B, __G)                (GPIO##__B##_CD_IOC->GPIO##__B##__G##_IOMUX_SEL_0)
#define IOMUX_CD_1(__B, __G)                (GPIO##__B##_CD_IOC->GPIO##__B##__G##_IOMUX_SEL_1)
#define SET_IOMUX_0(_B, _G, _GP, _V, _W)    _PINCTRL_WRITE(IOMUX_0(_B, _G), RK_GEN_VAL(_GP, _V, _W))
#define SET_IOMUX_1(_B, _G, _GP, _V, _W)    _PINCTRL_WRITE(IOMUX_1(_B, _G), RK_GEN_VAL(_GP, _V, _W))
#define SET_IOMUX_AB_0(_B, _G, _GP, _V, _W) _PINCTRL_WRITE(IOMUX_AB_0(_B, _G), RK_GEN_VAL(_GP, _V, _W))
#define SET_IOMUX_AB_1(_B, _G, _GP, _V, _W) _PINCTRL_WRITE(IOMUX_AB_1(_B, _G), RK_GEN_VAL(_GP, _V, _W))
#define SET_IOMUX_CD_0(_B, _G, _GP, _V, _W) _PINCTRL_WRITE(IOMUX_CD_0(_B, _G), RK_GEN_VAL(_GP, _V, _W))
#define SET_IOMUX_CD_1(_B, _G, _GP, _V, _W) _PINCTRL_WRITE(IOMUX_CD_1(_B, _G), RK_GEN_VAL(_GP, _V, _W))
#define RK_SET_IOMUX_0(B, G, BP, V)         SET_IOMUX_0(B, G, BP % IOMUX_4_PIN_PER_REG, V, IOMUX_4_BIT_PER_PIN)
#define RK_SET_IOMUX_1(B, G, BP, V)         SET_IOMUX_1(B, G, BP % IOMUX_4_PIN_PER_REG, V, IOMUX_4_BIT_PER_PIN)
#define RK_SET_IOMUX_AB_0(B, G, BP, V)      SET_IOMUX_AB_0(B, G, BP % IOMUX_4_PIN_PER_REG, V, IOMUX_4_BIT_PER_PIN)
#define RK_SET_IOMUX_AB_1(B, G, BP, V)      SET_IOMUX_AB_1(B, G, BP % IOMUX_4_PIN_PER_REG, V, IOMUX_4_BIT_PER_PIN)
#define RK_SET_IOMUX_CD_0(B, G, BP, V)      SET_IOMUX_CD_0(B, G, BP % IOMUX_4_PIN_PER_REG, V, IOMUX_4_BIT_PER_PIN)
#define RK_SET_IOMUX_CD_1(B, G, BP, V)      SET_IOMUX_CD_1(B, G, BP % IOMUX_4_PIN_PER_REG, V, IOMUX_4_BIT_PER_PIN)
#define SET_IOMUX_4_BIT(BANK, GROUP, BANK_PIN, VAL) \
{                                                   \
    if ((BANK_PIN % 8) < 4) {                       \
        RK_SET_IOMUX_0(BANK, GROUP, BANK_PIN, VAL); \
    } else {                                        \
        RK_SET_IOMUX_1(BANK, GROUP, BANK_PIN, VAL); \
    }                                               \
}

#define SET_IOMUX_AB_4_BIT(BANK, GROUP, BANK_PIN, VAL) \
{                                                      \
    if ((BANK_PIN % 8) < 4) {                          \
        RK_SET_IOMUX_AB_0(BANK, GROUP, BANK_PIN, VAL); \
    } else {                                           \
        RK_SET_IOMUX_AB_1(BANK, GROUP, BANK_PIN, VAL); \
    }                                                  \
}

#define SET_IOMUX_CD_4_BIT(BANK, GROUP, BANK_PIN, VAL) \
{                                                      \
    if ((BANK_PIN % 8) < 4) {                          \
        RK_SET_IOMUX_CD_0(BANK, GROUP, BANK_PIN, VAL); \
    } else {                                           \
        RK_SET_IOMUX_CD_1(BANK, GROUP, BANK_PIN, VAL); \
    }                                                  \
}

#define PULL_2_BIT_PER_PIN                 (2)
#define PULL_8_PIN_PER_REG                 (8)
#define PULL_0(__B, __G)                   (GPIO##__B##_IOC->GPIO##__B##__G##_PULL)
#define PULL_AB_0(__B, __G)                (GPIO##__B##_AB_IOC->GPIO##__B##__G##_PULL)
#define PULL_CD_0(__B, __G)                (GPIO##__B##_CD_IOC->GPIO##__B##__G##_PULL)
#define SET_PULL_0(_B, _G, _GP, _V, _W)    _PINCTRL_WRITE(PULL_0(_B, _G), RK_GEN_VAL(_GP, _V, _W))
#define SET_PULL_AB_0(_B, _G, _GP, _V, _W) _PINCTRL_WRITE(PULL_AB_0(_B, _G), RK_GEN_VAL(_GP, _V, _W))
#define SET_PULL_CD_0(_B, _G, _GP, _V, _W) _PINCTRL_WRITE(PULL_CD_0(_B, _G), RK_GEN_VAL(_GP, _V, _W))
#define RK_SET_PULL_0(B, G, BP, V)         SET_PULL_0(B, G, BP % PULL_8_PIN_PER_REG, V, PULL_2_BIT_PER_PIN)
#define RK_SET_PULL_AB_0(B, G, BP, V)      SET_PULL_AB_0(B, G, BP % PULL_8_PIN_PER_REG, V, PULL_2_BIT_PER_PIN)
#define RK_SET_PULL_CD_0(B, G, BP, V)      SET_PULL_CD_0(B, G, BP % PULL_8_PIN_PER_REG, V, PULL_2_BIT_PER_PIN)

#define DRV_8_BIT_PER_PIN                 (8)
#define DRV_2_PIN_PER_REG                 (2)
#define DRV_0(__B, __G)                   (GPIO##__B##_IOC->GPIO##__B##__G##_DS_0)
#define DRV_1(__B, __G)                   (GPIO##__B##_IOC->GPIO##__B##__G##_DS_1)
#define DRV_2(__B, __G)                   (GPIO##__B##_IOC->GPIO##__B##__G##_DS_2)
#define DRV_3(__B, __G)                   (GPIO##__B##_IOC->GPIO##__B##__G##_DS_3)
#define DRV_AB_0(__B, __G)                (GPIO##__B##_AB_IOC->GPIO##__B##__G##_DS_0)
#define DRV_AB_1(__B, __G)                (GPIO##__B##_AB_IOC->GPIO##__B##__G##_DS_1)
#define DRV_AB_2(__B, __G)                (GPIO##__B##_AB_IOC->GPIO##__B##__G##_DS_2)
#define DRV_AB_3(__B, __G)                (GPIO##__B##_AB_IOC->GPIO##__B##__G##_DS_3)
#define DRV_CD_0(__B, __G)                (GPIO##__B##_CD_IOC->GPIO##__B##__G##_DS_0)
#define DRV_CD_1(__B, __G)                (GPIO##__B##_CD_IOC->GPIO##__B##__G##_DS_1)
#define DRV_CD_2(__B, __G)                (GPIO##__B##_CD_IOC->GPIO##__B##__G##_DS_2)
#define DRV_CD_3(__B, __G)                (GPIO##__B##_CD_IOC->GPIO##__B##__G##_DS_3)
#define SET_DRV_0(_B, _G, _GP, _V, _W)    _PINCTRL_WRITE(DRV_0(_B, _G), RK_GEN_VAL(_GP, _V, _W))
#define SET_DRV_1(_B, _G, _GP, _V, _W)    _PINCTRL_WRITE(DRV_1(_B, _G), RK_GEN_VAL(_GP, _V, _W))
#define SET_DRV_2(_B, _G, _GP, _V, _W)    _PINCTRL_WRITE(DRV_2(_B, _G), RK_GEN_VAL(_GP, _V, _W))
#define SET_DRV_3(_B, _G, _GP, _V, _W)    _PINCTRL_WRITE(DRV_3(_B, _G), RK_GEN_VAL(_GP, _V, _W))
#define SET_DRV_AB_0(_B, _G, _GP, _V, _W) _PINCTRL_WRITE(DRV_AB_0(_B, _G), RK_GEN_VAL(_GP, _V, _W))
#define SET_DRV_AB_1(_B, _G, _GP, _V, _W) _PINCTRL_WRITE(DRV_AB_1(_B, _G), RK_GEN_VAL(_GP, _V, _W))
#define SET_DRV_AB_2(_B, _G, _GP, _V, _W) _PINCTRL_WRITE(DRV_AB_2(_B, _G), RK_GEN_VAL(_GP, _V, _W))
#define SET_DRV_AB_3(_B, _G, _GP, _V, _W) _PINCTRL_WRITE(DRV_AB_3(_B, _G), RK_GEN_VAL(_GP, _V, _W))
#define SET_DRV_CD_0(_B, _G, _GP, _V, _W) _PINCTRL_WRITE(DRV_CD_0(_B, _G), RK_GEN_VAL(_GP, _V, _W))
#define SET_DRV_CD_1(_B, _G, _GP, _V, _W) _PINCTRL_WRITE(DRV_CD_1(_B, _G), RK_GEN_VAL(_GP, _V, _W))
#define SET_DRV_CD_2(_B, _G, _GP, _V, _W) _PINCTRL_WRITE(DRV_CD_2(_B, _G), RK_GEN_VAL(_GP, _V, _W))
#define SET_DRV_CD_3(_B, _G, _GP, _V, _W) _PINCTRL_WRITE(DRV_CD_3(_B, _G), RK_GEN_VAL(_GP, _V, _W))
#define RK_SET_DRV_0(B, G, BP, V)         SET_DRV_0(B, G, BP % DRV_2_PIN_PER_REG, V, DRV_8_BIT_PER_PIN)
#define RK_SET_DRV_1(B, G, BP, V)         SET_DRV_1(B, G, BP % DRV_2_PIN_PER_REG, V, DRV_8_BIT_PER_PIN)
#define RK_SET_DRV_2(B, G, BP, V)         SET_DRV_2(B, G, BP % DRV_2_PIN_PER_REG, V, DRV_8_BIT_PER_PIN)
#define RK_SET_DRV_3(B, G, BP, V)         SET_DRV_3(B, G, BP % DRV_2_PIN_PER_REG, V, DRV_8_BIT_PER_PIN)
#define RK_SET_DRV_AB_0(B, G, BP, V)      SET_DRV_AB_0(B, G, BP % DRV_2_PIN_PER_REG, V, DRV_8_BIT_PER_PIN)
#define RK_SET_DRV_AB_1(B, G, BP, V)      SET_DRV_AB_1(B, G, BP % DRV_2_PIN_PER_REG, V, DRV_8_BIT_PER_PIN)
#define RK_SET_DRV_AB_2(B, G, BP, V)      SET_DRV_AB_2(B, G, BP % DRV_2_PIN_PER_REG, V, DRV_8_BIT_PER_PIN)
#define RK_SET_DRV_AB_3(B, G, BP, V)      SET_DRV_AB_3(B, G, BP % DRV_2_PIN_PER_REG, V, DRV_8_BIT_PER_PIN)
#define RK_SET_DRV_CD_0(B, G, BP, V)      SET_DRV_CD_0(B, G, BP % DRV_2_PIN_PER_REG, V, DRV_8_BIT_PER_PIN)
#define RK_SET_DRV_CD_1(B, G, BP, V)      SET_DRV_CD_1(B, G, BP % DRV_2_PIN_PER_REG, V, DRV_8_BIT_PER_PIN)
#define RK_SET_DRV_CD_2(B, G, BP, V)      SET_DRV_CD_2(B, G, BP % DRV_2_PIN_PER_REG, V, DRV_8_BIT_PER_PIN)
#define RK_SET_DRV_CD_3(B, G, BP, V)      SET_DRV_CD_3(B, G, BP % DRV_2_PIN_PER_REG, V, DRV_8_BIT_PER_PIN)
#define SET_DRV_8_BIT_1(BANK, GROUP, BANK_PIN, VAL) \
{                                                   \
    RK_SET_DRV_0(BANK, GROUP, BANK_PIN, VAL);       \
}

#define SET_DRV_8_BIT_2(BANK, GROUP, BANK_PIN, VAL) \
{                                                   \
    if ((BANK_PIN % 8) < 2) {                       \
        RK_SET_DRV_0(BANK, GROUP, BANK_PIN, VAL);   \
    } else {                                        \
        RK_SET_DRV_1(BANK, GROUP, BANK_PIN, VAL);   \
    }                                               \
}

#define SET_DRV_8_BIT_3(BANK, GROUP, BANK_PIN, VAL) \
{                                                   \
    if ((BANK_PIN % 8) < 2) {                       \
        RK_SET_DRV_0(BANK, GROUP, BANK_PIN, VAL);   \
    } else if ((BANK_PIN % 8) < 4) {                \
        RK_SET_DRV_1(BANK, GROUP, BANK_PIN, VAL);   \
    } else {                                        \
        RK_SET_DRV_2(BANK, GROUP, BANK_PIN, VAL);   \
    }                                               \
}

#define SET_DRV_8_BIT_4(BANK, GROUP, BANK_PIN, VAL) \
{                                                   \
    if ((BANK_PIN % 8) < 2) {                       \
        RK_SET_DRV_0(BANK, GROUP, BANK_PIN, VAL);   \
    } else if ((BANK_PIN % 8) < 4) {                \
        RK_SET_DRV_1(BANK, GROUP, BANK_PIN, VAL);   \
    } else if ((BANK_PIN % 8) < 6) {                \
        RK_SET_DRV_2(BANK, GROUP, BANK_PIN, VAL);   \
    } else {                                        \
        RK_SET_DRV_3(BANK, GROUP, BANK_PIN, VAL);   \
    }                                               \
}

#define SET_DRV_8_BIT_4_AB(BANK, GROUP, BANK_PIN, VAL) \
{                                                      \
    if ((BANK_PIN % 8) < 2) {                          \
        RK_SET_DRV_AB_0(BANK, GROUP, BANK_PIN, VAL);   \
    } else if ((BANK_PIN % 8) < 4) {                   \
        RK_SET_DRV_AB_1(BANK, GROUP, BANK_PIN, VAL);   \
    } else if ((BANK_PIN % 8) < 6) {                   \
        RK_SET_DRV_AB_2(BANK, GROUP, BANK_PIN, VAL);   \
    } else {                                           \
        RK_SET_DRV_AB_3(BANK, GROUP, BANK_PIN, VAL);   \
    }                                                  \
}

#define SET_DRV_8_BIT_4_CD(BANK, GROUP, BANK_PIN, VAL) \
{                                                      \
    if ((BANK_PIN % 8) < 2) {                          \
        RK_SET_DRV_CD_0(BANK, GROUP, BANK_PIN, VAL);   \
    } else if ((BANK_PIN % 8) < 4) {                   \
        RK_SET_DRV_CD_1(BANK, GROUP, BANK_PIN, VAL);   \
    } else if ((BANK_PIN % 8) < 6) {                   \
        RK_SET_DRV_CD_2(BANK, GROUP, BANK_PIN, VAL);   \
    } else {                                           \
        RK_SET_DRV_CD_3(BANK, GROUP, BANK_PIN, VAL);   \
    }                                                  \
}

#define SR_2_BIT_PER_PIN                 (2)
#define SR_8_PIN_PER_REG                 (8)
#define SR_0(__B, __G)                   (GPIO##__B##_IOC->GPIO##__B##__G##_SL)
#define SR_AB_0(__B, __G)                (GPIO##__B##_AB_IOC->GPIO##__B##__G##_SL)
#define SR_CD_0(__B, __G)                (GPIO##__B##_CD_IOC->GPIO##__B##__G##_SL)
#define SET_SR_0(_B, _G, _GP, _V, _W)    _PINCTRL_WRITE(SR_0(_B, _G), RK_GEN_VAL(_GP, _V, _W))
#define SET_SR_AB_0(_B, _G, _GP, _V, _W) _PINCTRL_WRITE(SR_AB_0(_B, _G), RK_GEN_VAL(_GP, _V, _W))
#define SET_SR_CD_0(_B, _G, _GP, _V, _W) _PINCTRL_WRITE(SR_CD_0(_B, _G), RK_GEN_VAL(_GP, _V, _W))
#define RK_SET_SR_0(B, G, BP, V)         SET_SR_0(B, G, BP % SR_8_PIN_PER_REG, V, SR_2_BIT_PER_PIN)
#define RK_SET_SR_AB_0(B, G, BP, V)      SET_SR_AB_0(B, G, BP % SR_8_PIN_PER_REG, V, SR_2_BIT_PER_PIN)
#define RK_SET_SR_CD_0(B, G, BP, V)      SET_SR_CD_0(B, G, BP % SR_8_PIN_PER_REG, V, SR_2_BIT_PER_PIN)

#define SMT_1_BIT_PER_PIN                 (1)
#define SMT_8_PIN_PER_REG                 (8)
#define SMT_0(__B, __G)                   (GPIO##__B##_IOC->GPIO##__B##__G##_SMT)
#define SMT_AB_0(__B, __G)                (GPIO##__B##_AB_IOC->GPIO##__B##__G##_SMT)
#define SMT_CD_0(__B, __G)                (GPIO##__B##_CD_IOC->GPIO##__B##__G##_SMT)
#define SET_SMT_0(_B, _G, _GP, _V, _W)    _PINCTRL_WRITE(SMT_0(_B, _G), RK_GEN_VAL(_GP, _V, _W))
#define SET_SMT_AB_0(_B, _G, _GP, _V, _W) _PINCTRL_WRITE(SMT_AB_0(_B, _G), RK_GEN_VAL(_GP, _V, _W))
#define SET_SMT_CD_0(_B, _G, _GP, _V, _W) _PINCTRL_WRITE(SMT_CD_0(_B, _G), RK_GEN_VAL(_GP, _V, _W))
#define RK_SET_SMT_0(B, G, BP, V)         SET_SMT_0(B, G, BP % SMT_8_PIN_PER_REG, V, SMT_1_BIT_PER_PIN)
#define RK_SET_SMT_AB_0(B, G, BP, V)      SET_SMT_AB_0(B, G, BP % SMT_8_PIN_PER_REG, V, SMT_1_BIT_PER_PIN)
#define RK_SET_SMT_CD_0(B, G, BP, V)      SET_SMT_CD_0(B, G, BP % SMT_8_PIN_PER_REG, V, SMT_1_BIT_PER_PIN)

#define RMIO_5_BIT_PER_PIN      (5)
#define RMIO_6_BIT_PER_PIN      (6)
#define RMIO_SET_FUNC(__W, __V) ((_TO_MASK(__W) << 16) | ((__V) & _TO_MASK(__W)))
#define RMIO_SET_FUNC_5_BIT(_V) RMIO_SET_FUNC(RMIO_5_BIT_PER_PIN, _V)
#define RMIO_SET_FUNC_6_BIT(_V) RMIO_SET_FUNC(RMIO_6_BIT_PER_PIN, _V)
#define SET_RM0_IO(_G, _P)      (RM0_IO->rm0_gpio0##_G##_P##_sel)
#define RK_SET_RM0_IO(G, P, V)  (SET_RM0_IO(G, P) = RMIO_SET_FUNC_5_BIT(V))
#define SET_RM1_IO(_G, _P)      (RM1_IO->rm1_gpio3##_G##_P##_sel)
#define RK_SET_RM1_IO(G, P, V)  (SET_RM1_IO(G, P) = RMIO_SET_FUNC_6_BIT(V))
#define SET_RM2_IO(_G, _P)      (RM2_IO->rm2_gpio2##_G##_P##_sel)
#define RK_SET_RM2_IO(G, P, V)  (SET_RM2_IO(G, P) = RMIO_SET_FUNC_5_BIT(V))
#define SET_RM3_IO(_G, _P)      (RM3_IO->rm3_gpio4##_G##_P##_sel)
#define RK_SET_RM3_IO(G, P, V)  (SET_RM3_IO(G, P) = RMIO_SET_FUNC_6_BIT(V))
#define SET_RM4_IO(_G, _P)      (RM4_IO->rm4_gpio4##_G##_P##_sel)
#define RK_SET_RM4_IO(G, P, V)  (SET_RM4_IO(G, P) = RMIO_SET_FUNC_6_BIT(V))
#define RMIO_SET_RM0(BANK_PIN, GROUP, GROUP_PIN)   \
{                                                  \
        if (pin == BANK_PIN) {                     \
            RK_SET_RM0_IO(GROUP, GROUP_PIN, func); \
        }                                          \
}
#define RMIO_SET_RM1(BANK_PIN, GROUP, GROUP_PIN)   \
{                                                  \
        if (pin == BANK_PIN) {                     \
            RK_SET_RM1_IO(GROUP, GROUP_PIN, func); \
        }                                          \
}
#define RMIO_SET_RM2(BANK_PIN, GROUP, GROUP_PIN)   \
{                                                  \
        if (pin == BANK_PIN) {                     \
            RK_SET_RM2_IO(GROUP, GROUP_PIN, func); \
        }                                          \
}
#define RMIO_SET_RM3(BANK_PIN, GROUP, GROUP_PIN)   \
{                                                  \
        if (pin == BANK_PIN) {                     \
            RK_SET_RM3_IO(GROUP, GROUP_PIN, func); \
        }                                          \
}
#define RMIO_SET_RM4(BANK_PIN, GROUP, GROUP_PIN)   \
{                                                  \
        if (pin == BANK_PIN) {                     \
            RK_SET_RM4_IO(GROUP, GROUP_PIN, func); \
        }                                          \
}

/********************* Private Variable Definition ***************************/

/********************* Private Function Definition ***************************/

/**
 * @brief  Private function to set pin iomux.
 * @param  bank: pin bank.
 * @param  pin: bank pin number 0~31.
 * @param  val: value to write.
 * @return HAL_Status.
 */
static HAL_Status PINCTRL_SetIOMUX(eGPIO_bankId bank, uint8_t pin, uint32_t val)
{
    switch (bank) {
    case 0:
        if (pin < 8) {
            SET_IOMUX_4_BIT(0, A, pin, val);
        } else if (pin < 12) {
            RK_SET_IOMUX_0(0, B, pin, val);
        }
        break;
    case 1:
        if (pin < 8) {
            SET_IOMUX_4_BIT(1, A, pin, val);
        } else if (pin < 12) {
            RK_SET_IOMUX_0(1, B, pin, val);
        }
        break;
    case 2:
        if (pin < 8) {
            SET_IOMUX_4_BIT(2, A, pin, val);
        } else if (pin < 12) {
            RK_SET_IOMUX_0(2, B, pin, val);
        }
        break;
    case 3:
        if (pin < 8) {
            SET_IOMUX_4_BIT(3, A, pin, val);
        } else if (pin < 16) {
            SET_IOMUX_4_BIT(3, B, pin, val);
        } else if (pin < 24) {
            SET_IOMUX_4_BIT(3, C, pin, val);
        } else if (pin < 28) {
            RK_SET_IOMUX_0(3, D, pin, val);
        }
        break;
    case 4:
        if (pin < 8) {
            SET_IOMUX_AB_4_BIT(4, A, pin, val);
        } else if (pin < 16) {
            SET_IOMUX_AB_4_BIT(4, B, pin, val);
        } else if (pin < 24) {
            SET_IOMUX_CD_4_BIT(4, C, pin, val);
        } else if (pin < 32) {
            SET_IOMUX_CD_4_BIT(4, D, pin, val);
        }
        break;
    default:
        HAL_DBG("unknown gpio%d\n", bank);
        break;
    }

    return HAL_OK;
}

/**
 * @brief  Private function to set GRF select IOC.
 * @param  bank: pin bank.
 * @param  pin: bank pin number 0~31.
 * @param  val: value to write.
 * @return HAL_Status.
 */
static HAL_Status PINCTRL_GRFSelIOC(eGPIO_bankId bank, uint8_t pin, uint32_t val)
{
    if (bank == 4) {
        if ((pin == 25) && (val == 1)) {
            GRF->SOC_CON4 = RK_GEN_VAL(5, 2, 2);
        }
        if ((pin == 26) && (val == 2)) {
            GRF->SOC_CON4 = RK_GEN_VAL(4, 2, 2);
        }
        if ((pin == 17) && (val == 1)) {
            GRF->SOC_CON4 = RK_GEN_VAL(3, 2, 2);
        }
        if ((pin == 18) && (val == 1)) {
            GRF->SOC_CON4 = RK_GEN_VAL(2, 2, 2);
        }
        if ((pin == 12) && (val == 1)) {
            GRF->SOC_CON4 = RK_GEN_VAL(1, 2, 2);
        }
        if ((pin == 13) && (val == 1)) {
            GRF->SOC_CON4 = RK_GEN_VAL(0, 2, 2);
        }
        if ((pin == 2) && (val == 2)) {
            GRF->SOC_CON8 = RK_GEN_VAL(9, 1, 1);
        }
        if ((pin == 27) && (val == 1)) {
            GRF->SOC_CON11 = RK_GEN_VAL(7, 2, 2);
        }
        if ((pin == 26) && (val == 1)) {
            GRF->SOC_CON11 = RK_GEN_VAL(6, 2, 2);
        }
        if ((pin == 6) && (val == 1)) {
            GRF->SOC_CON11 = RK_GEN_VAL(5, 2, 2);
        }
        if ((pin == 5) && (val == 1)) {
            GRF->SOC_CON11 = RK_GEN_VAL(4, 2, 2);
        }
        if ((pin == 4) && (val == 1)) {
            GRF->SOC_CON11 = RK_GEN_VAL(3, 2, 2);
        }
        if ((pin == 3) && (val == 1)) {
            GRF->SOC_CON11 = RK_GEN_VAL(2, 2, 2);
        }
        if ((pin == 2) && (val == 1)) {
            GRF->SOC_CON11 = RK_GEN_VAL(1, 2, 2);
        }
        if ((pin == 1) && (val == 1)) {
            GRF->SOC_CON11 = RK_GEN_VAL(0, 2, 2);
        }
        if ((pin == 31) && (val == 1)) {
            GRF->SOC_CON12 = RK_GEN_VAL(3, 2, 2);
        }
        if ((pin == 30) && (val == 1)) {
            GRF->SOC_CON12 = RK_GEN_VAL(2, 2, 2);
        }
        if ((pin == 29) && (val == 1)) {
            GRF->SOC_CON12 = RK_GEN_VAL(1, 2, 2);
        }
        if ((pin == 28) && (val == 1)) {
            GRF->SOC_CON12 = RK_GEN_VAL(0, 2, 2);
        }
        if ((pin == 23) && (val == 1)) {
            GRF->SOC_CON21 = RK_GEN_VAL(3, 1, 1);
        }
        if ((pin == 22) && (val == 1)) {
            GRF->SOC_CON21 = RK_GEN_VAL(2, 1, 1);
        }
        if ((pin == 21) && (val == 1)) {
            GRF->SOC_CON21 = RK_GEN_VAL(1, 1, 1);
        }
        if ((pin == 20) && (val == 1)) {
            GRF->SOC_CON21 = RK_GEN_VAL(0, 1, 1);
        }
        if ((pin == 31) && (val == 3)) {
            GRF_PMU->SOC_CON5 = RK_GEN_VAL(7, 2, 2);
        }
        if ((pin == 30) && (val == 3)) {
            GRF_PMU->SOC_CON5 = RK_GEN_VAL(6, 2, 2);
        }
        if ((pin == 29) && (val == 3)) {
            GRF_PMU->SOC_CON5 = RK_GEN_VAL(5, 2, 2);
        }
        if ((pin == 28) && (val == 3)) {
            GRF_PMU->SOC_CON5 = RK_GEN_VAL(4, 2, 2);
        }
        if ((pin == 27) && (val == 3)) {
            GRF_PMU->SOC_CON5 = RK_GEN_VAL(3, 2, 2);
        }
        if ((pin == 26) && (val == 3)) {
            GRF_PMU->SOC_CON5 = RK_GEN_VAL(2, 2, 2);
        }
        if ((pin == 25) && (val == 3)) {
            GRF_PMU->SOC_CON5 = RK_GEN_VAL(1, 2, 2);
        }
        if ((pin == 24) && (val == 3)) {
            GRF_PMU->SOC_CON5 = RK_GEN_VAL(0, 2, 2);
        }
        if ((pin == 10) && (val == 1)) {
            GRF_PMU->SOC_CON6 = RK_GEN_VAL(3, 1, 1);
        }
        if ((pin == 9) && (val == 1)) {
            GRF_PMU->SOC_CON6 = RK_GEN_VAL(2, 1, 1);
        }
        if ((pin == 8) && (val == 1)) {
            GRF_PMU->SOC_CON6 = RK_GEN_VAL(1, 1, 1);
        }
        if ((pin == 7) && (val == 1)) {
            GRF_PMU->SOC_CON6 = RK_GEN_VAL(0, 1, 1);
        }
    }

    return HAL_OK;
}

/**
 * @brief  Private function to set pin pull.
 * @param  bank: pin bank.
 * @param  pin: bank pin number 0~31.
 * @param  val: value to write.
 * @return HAL_Status.
 */
static HAL_Status PINCTRL_SetPULL(eGPIO_bankId bank, uint8_t pin, uint32_t val)
{
    switch (bank) {
    case 0:
        if (pin < 8) {
            RK_SET_PULL_0(0, A, pin, val);
        } else if (pin < 16) {
            RK_SET_PULL_0(0, B, pin, val);
        }
        break;
    case 1:
        if (pin < 8) {
            RK_SET_PULL_0(1, A, pin, val);
        } else if (pin < 16) {
            RK_SET_PULL_0(1, B, pin, val);
        }
        break;
    case 2:
        if (pin < 8) {
            RK_SET_PULL_0(2, A, pin, val);
        } else if (pin < 16) {
            RK_SET_PULL_0(2, B, pin, val);
        }
        break;
    case 3:
        if (pin < 8) {
            RK_SET_PULL_0(3, A, pin, val);
        } else if (pin < 16) {
            RK_SET_PULL_0(3, B, pin, val);
        } else if (pin < 24) {
            RK_SET_PULL_0(3, C, pin, val);
        } else if (pin < 32) {
            RK_SET_PULL_0(3, D, pin, val);
        }
        break;
    case 4:
        if (pin < 8) {
            RK_SET_PULL_AB_0(4, A, pin, val);
        } else if (pin < 16) {
            RK_SET_PULL_AB_0(4, B, pin, val);
        } else if (pin < 24) {
            RK_SET_PULL_CD_0(4, C, pin, val);
        } else if (pin < 32) {
            RK_SET_PULL_CD_0(4, D, pin, val);
        }
        break;
    default:
        HAL_DBG("unknown gpio%d\n", bank);
        break;
    }

    return HAL_OK;
}

/**
 * @brief  Private function to set pin drive strength.
 * @param  bank: pin bank.
 * @param  pin: bank pin number 0~31.
 * @param  val: value to write.
 * @return HAL_Status.
 */
static HAL_Status PINCTRL_SetDRV(eGPIO_bankId bank, uint8_t pin, uint32_t val)
{
    switch (bank) {
    case 0:
        if (pin < 8) {
            SET_DRV_8_BIT_4(0, A, pin, val);
        } else if (pin < 16) {
            SET_DRV_8_BIT_2(0, B, pin, val);
        }
        break;
    case 1:
        if (pin < 8) {
            SET_DRV_8_BIT_4(1, A, pin, val);
        } else if (pin < 16) {
            SET_DRV_8_BIT_2(1, B, pin, val);
        }
        break;
    case 2:
        if (pin < 8) {
            SET_DRV_8_BIT_4(2, A, pin, val);
        } else if (pin < 16) {
            SET_DRV_8_BIT_1(2, B, pin, val);
        }
        break;
    case 3:
        if (pin < 8) {
            SET_DRV_8_BIT_4(3, A, pin, val);
        } else if (pin < 16) {
            SET_DRV_8_BIT_4(3, B, pin, val);
        } else if (pin < 24) {
            SET_DRV_8_BIT_4(3, C, pin, val);
        } else if (pin < 32) {
            SET_DRV_8_BIT_2(3, D, pin, val);
        }
        break;
    case 4:
        if (pin < 8) {
            SET_DRV_8_BIT_4_AB(4, A, pin, val);
        } else if (pin < 16) {
            SET_DRV_8_BIT_4_AB(4, B, pin, val);
        } else if (pin < 24) {
            SET_DRV_8_BIT_4_CD(4, C, pin, val);
        } else if (pin < 32) {
            SET_DRV_8_BIT_4_CD(4, D, pin, val);
        }
        break;
    default:
        HAL_DBG("unknown gpio%d\n", bank);
        break;
    }

    return HAL_OK;
}

/**
 * @brief  Private function to set pin slew rate.
 * @param  bank: pin bank.
 * @param  pin: bank pin number 0~31.
 * @param  val: value to write.
 * @return HAL_Status.
 */
static HAL_Status PINCTRL_SetSR(eGPIO_bankId bank, uint8_t pin, uint32_t val)
{
    switch (bank) {
    case 0:
        if (pin < 8) {
            RK_SET_SR_0(0, A, pin, val);
        } else if (pin < 16) {
            RK_SET_SR_0(0, B, pin, val);
        }
        break;
    case 1:
        if (pin < 8) {
            RK_SET_SR_0(1, A, pin, val);
        } else if (pin < 16) {
            RK_SET_SR_0(1, B, pin, val);
        }
        break;
    case 2:
        if (pin < 8) {
            RK_SET_SR_0(2, A, pin, val);
        } else if (pin < 16) {
            RK_SET_SR_0(2, B, pin, val);
        }
        break;
    case 3:
        if (pin < 8) {
            RK_SET_SR_0(3, A, pin, val);
        } else if (pin < 16) {
            RK_SET_SR_0(3, B, pin, val);
        } else if (pin < 24) {
            RK_SET_SR_0(3, C, pin, val);
        } else if (pin < 32) {
            RK_SET_SR_0(3, D, pin, val);
        }
        break;
    case 4:
        if (pin < 8) {
            RK_SET_SR_AB_0(4, A, pin, val);
        } else if (pin < 16) {
            RK_SET_SR_AB_0(4, B, pin, val);
        } else if (pin < 24) {
            RK_SET_SR_CD_0(4, C, pin, val);
        } else if (pin < 32) {
            RK_SET_SR_CD_0(4, D, pin, val);
        }
        break;
    default:
        HAL_DBG("unknown gpio%d\n", bank);
        break;
    }

    return HAL_OK;
}

/**
 * @brief  Private function to set pin schmitt trigger.
 * @param  bank: pin bank.
 * @param  pin: bank pin number 0~31.
 * @param  val: value to write.
 * @return HAL_Status.
 */
static HAL_Status PINCTRL_SetSMT(eGPIO_bankId bank, uint8_t pin, uint32_t val)
{
    switch (bank) {
    case 0:
        if (pin < 8) {
            RK_SET_SMT_0(0, A, pin, val);
        } else if (pin < 16) {
            RK_SET_SMT_0(0, B, pin, val);
        }
        break;
    case 1:
        if (pin < 8) {
            RK_SET_SMT_0(1, A, pin, val);
        } else if (pin < 16) {
            RK_SET_SMT_0(1, B, pin, val);
        }
        break;
    case 2:
        if (pin < 8) {
            RK_SET_SMT_0(2, A, pin, val);
        } else if (pin < 16) {
            RK_SET_SMT_0(2, B, pin, val);
        }
        break;
    case 3:
        if (pin < 8) {
            RK_SET_SMT_0(3, A, pin, val);
        } else if (pin < 16) {
            RK_SET_SMT_0(3, B, pin, val);
        } else if (pin < 24) {
            RK_SET_SMT_0(3, C, pin, val);
        } else if (pin < 32) {
            RK_SET_SMT_0(3, D, pin, val);
        }
        break;
    case 4:
        if (pin < 8) {
            RK_SET_SMT_AB_0(4, A, pin, val);
        } else if (pin < 16) {
            RK_SET_SMT_AB_0(4, B, pin, val);
        } else if (pin < 24) {
            RK_SET_SMT_CD_0(4, C, pin, val);
        } else if (pin < 32) {
            RK_SET_SMT_CD_0(4, D, pin, val);
        }
        break;
    default:
        HAL_DBG("unknown gpio%d\n", bank);
        break;
    }

    return HAL_OK;
}

/**
 * @brief  Private function to set pin parameter.
 * @param  bank: pin bank.
 * @param  pin: bank pin number 0~31.
 * @param  param: multi parameters defined in @ref ePINCTRL_configParam,
 * @return HAL_Status.
 */
static HAL_Status PINCTRL_SetPinParam(eGPIO_bankId bank, uint8_t pin, uint32_t param)
{
    HAL_Status rc = HAL_OK;

    if (param & FLAG_MUX) {
        rc |= PINCTRL_SetIOMUX(bank, pin, (uint8_t)((param & MASK_MUX) >> SHIFT_MUX));
        rc |= PINCTRL_GRFSelIOC(bank, pin, (uint8_t)((param & MASK_MUX) >> SHIFT_MUX));
    }

    if (param & FLAG_PUL) {
        rc |= PINCTRL_SetPULL(bank, pin, (uint8_t)((param & MASK_PUL) >> SHIFT_PUL));
    }

    if (param & FLAG_DRV) {
        rc |= PINCTRL_SetDRV(bank, pin, (uint8_t)((param & MASK_DRV) >> SHIFT_DRV));
    }

    if (param & FLAG_SRT) {
        rc |= PINCTRL_SetSR(bank, pin, (uint8_t)((param & MASK_SRT) >> SHIFT_SRT));
    }

    if (param & FLAG_SMT) {
        rc |= PINCTRL_SetSMT(bank, pin, (uint8_t)((param & MASK_SMT) >> SHIFT_SMT));
    }

    return rc;
}

/**
 * @brief  Private function to set Rockchip Matrix IO for one pin.
 * @param  bank: pin bank.
 * @param  pin: bank pin number 0~31.
 * @param  func: RMIO function defined in @ref eRMIO_Name,
 * @return HAL_Status.
 */
static HAL_Status PINCTRL_SetRMIO(eGPIO_bankId bank, uint8_t pin, uint32_t func)
{
    switch (bank) {
    case 0:
        RMIO_SET_RM0(0, a, 0)
        RMIO_SET_RM0(1, a, 1)
        RMIO_SET_RM0(2, a, 2)
        RMIO_SET_RM0(3, a, 3)
        RMIO_SET_RM0(4, a, 4)
        RMIO_SET_RM0(5, a, 5)
        RMIO_SET_RM0(6, a, 6)
        RMIO_SET_RM0(7, a, 7)
        RMIO_SET_RM0(8, b, 0)
        RMIO_SET_RM0(9, b, 1)
        RMIO_SET_RM0(10, b, 2)
        if (pin > 10) {
            HAL_DBG("unknown rmio bank0 pin-%d\n", pin);
        }
        break;
    case 1:
        HAL_DBG("unknown rmio bank1 pin-%d\n", pin);
        break;
    case 2:
        RMIO_SET_RM2(0, a, 0)
        RMIO_SET_RM2(1, a, 1)
        RMIO_SET_RM2(2, a, 2)
        RMIO_SET_RM2(3, a, 3)
        RMIO_SET_RM2(4, a, 4)
        RMIO_SET_RM2(5, a, 5)
        RMIO_SET_RM2(6, a, 6)
        RMIO_SET_RM2(7, a, 7)
        RMIO_SET_RM2(8, b, 0)
        if (pin > 8) {
            HAL_DBG("unknown rmio bank2 pin-%d\n", pin);
        }
        break;
    case 3:
        RMIO_SET_RM1(0, a, 0)
        RMIO_SET_RM1(1, a, 1)
        RMIO_SET_RM1(2, a, 2)
        RMIO_SET_RM1(3, a, 3)
        RMIO_SET_RM1(4, a, 4)
        RMIO_SET_RM1(5, a, 5)
        RMIO_SET_RM1(6, a, 6)
        RMIO_SET_RM1(7, a, 7)
        RMIO_SET_RM1(8, b, 0)
        RMIO_SET_RM1(9, b, 1)
        RMIO_SET_RM1(10, b, 2)
        RMIO_SET_RM1(11, b, 3)
        RMIO_SET_RM1(12, b, 4)
        RMIO_SET_RM1(13, b, 5)
        RMIO_SET_RM1(14, b, 6)
        RMIO_SET_RM1(15, b, 7)
        RMIO_SET_RM1(16, c, 0)
        RMIO_SET_RM1(17, c, 1)
        RMIO_SET_RM1(18, c, 2)
        RMIO_SET_RM1(19, c, 3)
        RMIO_SET_RM1(20, c, 4)
        RMIO_SET_RM1(21, c, 5)
        RMIO_SET_RM1(22, c, 6)
        RMIO_SET_RM1(23, c, 7)
        RMIO_SET_RM1(24, d, 0)
        RMIO_SET_RM1(25, d, 1)
        RMIO_SET_RM1(26, d, 2)
        RMIO_SET_RM1(27, d, 3)
        if (pin > 27) {
            HAL_DBG("unknown rmio bank3 pin-%d\n", pin);
        }
        break;
    case 4:
        RMIO_SET_RM3(0, a, 0)
        RMIO_SET_RM3(1, a, 1)
        RMIO_SET_RM3(2, a, 2)
        RMIO_SET_RM3(3, a, 3)
        RMIO_SET_RM3(4, a, 4)
        RMIO_SET_RM3(5, a, 5)
        RMIO_SET_RM3(6, a, 6)
        RMIO_SET_RM3(7, a, 7)
        RMIO_SET_RM3(8, b, 0)
        RMIO_SET_RM3(9, b, 1)
        RMIO_SET_RM3(10, b, 2)
        RMIO_SET_RM3(11, b, 3)
        RMIO_SET_RM3(12, b, 4)
        RMIO_SET_RM3(13, b, 5)
        RMIO_SET_RM3(14, b, 6)
        RMIO_SET_RM3(15, b, 7)
        RMIO_SET_RM4(16, c, 0)
        RMIO_SET_RM4(17, c, 1)
        RMIO_SET_RM4(18, c, 2)
        RMIO_SET_RM4(19, c, 3)
        RMIO_SET_RM4(20, c, 4)
        RMIO_SET_RM4(21, c, 5)
        RMIO_SET_RM4(22, c, 6)
        RMIO_SET_RM4(23, c, 7)
        RMIO_SET_RM4(24, d, 0)
        RMIO_SET_RM4(25, d, 1)
        RMIO_SET_RM4(26, d, 2)
        RMIO_SET_RM4(27, d, 3)
        RMIO_SET_RM4(28, d, 4)
        RMIO_SET_RM4(29, d, 5)
        RMIO_SET_RM4(30, d, 6)
        RMIO_SET_RM4(31, d, 7)
        if (pin > 31) {
            HAL_DBG("unknown rmio bank4 pin-%d\n", pin);
        }
        break;
    default:
        HAL_DBG("unknown rmio bank-%d\n", bank);
        break;
    }

    return HAL_OK;
}

/**
 * @brief  Private function to set GRF select RMIO.
 * @param  bank: pin bank.
 * @param  pin: bank pin number 0~31.
 * @param  func: RMIO function defined in @ref eRMIO_Name,
 * @return HAL_Status.
 */
static HAL_Status PINCTRL_GRFSelRMIO(eGPIO_bankId bank, uint8_t pin, uint32_t func)
{
    switch (bank) {
    case 0:
        if (func == 0x14) {
            GRF->SOC_CON8 = RK_GEN_VAL(9, 0, 1);
        }
        if (func == 0x4) {
            GRF->SOC_CON11 = RK_GEN_VAL(3, 0, 2);
        }
        if (func == 0x3) {
            GRF->SOC_CON11 = RK_GEN_VAL(2, 0, 2);
        }
        if (func == 0x2) {
            GRF->SOC_CON11 = RK_GEN_VAL(1, 0, 2);
        }
        if (func == 0x1) {
            GRF->SOC_CON11 = RK_GEN_VAL(0, 0, 2);
        }
        if (func == 0xd) {
            GRF_PMU->SOC_CON5 = RK_GEN_VAL(3, 0, 2);
        }
        if (func == 0xc) {
            GRF_PMU->SOC_CON5 = RK_GEN_VAL(2, 0, 2);
        }
        if (func == 0xb) {
            GRF_PMU->SOC_CON5 = RK_GEN_VAL(1, 0, 2);
        }
        if (func == 0xa) {
            GRF_PMU->SOC_CON5 = RK_GEN_VAL(0, 0, 2);
        }
        if (func == 0x8) {
            GRF_PMU->SOC_CON6 = RK_GEN_VAL(3, 0, 1);
        }
        if (func == 0x7) {
            GRF_PMU->SOC_CON6 = RK_GEN_VAL(2, 0, 1);
        }
        if (func == 0x6) {
            GRF_PMU->SOC_CON6 = RK_GEN_VAL(1, 0, 1);
        }
        if (func == 0x5) {
            GRF_PMU->SOC_CON6 = RK_GEN_VAL(0, 0, 1);
        }
        break;
    case 1:
        break;
    case 2:
        if (func == 0x6) {
            GRF->SOC_CON4 = RK_GEN_VAL(3, 1, 2);
        }
        if (func == 0x7) {
            GRF->SOC_CON4 = RK_GEN_VAL(2, 1, 2);
        }
        if (func == 0x2) {
            GRF->SOC_CON4 = RK_GEN_VAL(1, 1, 2);
        }
        if (func == 0x3) {
            GRF->SOC_CON4 = RK_GEN_VAL(0, 1, 2);
        }
        if (func == 0xc) {
            GRF->SOC_CON11 = RK_GEN_VAL(7, 1, 2);
        }
        if (func == 0xb) {
            GRF->SOC_CON11 = RK_GEN_VAL(6, 1, 2);
        }
        if (func == 0xa) {
            GRF->SOC_CON11 = RK_GEN_VAL(5, 1, 2);
        }
        if (func == 0x9) {
            GRF->SOC_CON11 = RK_GEN_VAL(4, 1, 2);
        }
        if (func == 0x10) {
            GRF->SOC_CON12 = RK_GEN_VAL(3, 1, 2);
        }
        if (func == 0xf) {
            GRF->SOC_CON12 = RK_GEN_VAL(2, 1, 2);
        }
        if (func == 0xe) {
            GRF->SOC_CON12 = RK_GEN_VAL(1, 1, 2);
        }
        if (func == 0xd) {
            GRF->SOC_CON12 = RK_GEN_VAL(0, 1, 2);
        }
        break;
    case 3:
        if (func == 0xa) {
            GRF->SOC_CON4 = RK_GEN_VAL(5, 0, 2);
        }
        if (func == 0xb) {
            GRF->SOC_CON4 = RK_GEN_VAL(4, 0, 2);
        }
        if (func == 0x6) {
            GRF->SOC_CON4 = RK_GEN_VAL(3, 0, 2);
        }
        if (func == 0x7) {
            GRF->SOC_CON4 = RK_GEN_VAL(2, 0, 2);
        }
        if (func == 0x2) {
            GRF->SOC_CON4 = RK_GEN_VAL(1, 0, 2);
        }
        if (func == 0x3) {
            GRF->SOC_CON4 = RK_GEN_VAL(0, 0, 2);
        }
        if (func == 0x10) {
            GRF->SOC_CON11 = RK_GEN_VAL(7, 0, 2);
        }
        if (func == 0xf) {
            GRF->SOC_CON11 = RK_GEN_VAL(6, 0, 2);
        }
        if (func == 0xe) {
            GRF->SOC_CON11 = RK_GEN_VAL(5, 0, 2);
        }
        if (func == 0xd) {
            GRF->SOC_CON11 = RK_GEN_VAL(4, 0, 2);
        }
        if (func == 0x14) {
            GRF->SOC_CON12 = RK_GEN_VAL(3, 0, 2);
        }
        if (func == 0x13) {
            GRF->SOC_CON12 = RK_GEN_VAL(2, 0, 2);
        }
        if (func == 0x12) {
            GRF->SOC_CON12 = RK_GEN_VAL(1, 0, 2);
        }
        if (func == 0x11) {
            GRF->SOC_CON12 = RK_GEN_VAL(0, 0, 2);
        }
        if (func == 0x1a) {
            GRF->SOC_CON21 = RK_GEN_VAL(3, 0, 1);
        }
        if (func == 0x19) {
            GRF->SOC_CON21 = RK_GEN_VAL(2, 0, 1);
        }
        if (func == 0x18) {
            GRF->SOC_CON21 = RK_GEN_VAL(1, 0, 1);
        }
        if (func == 0x17) {
            GRF->SOC_CON21 = RK_GEN_VAL(0, 0, 1);
        }
        if (func == 0x23) {
            GRF_PMU->SOC_CON5 = RK_GEN_VAL(7, 1, 2);
        }
        if (func == 0x22) {
            GRF_PMU->SOC_CON5 = RK_GEN_VAL(6, 1, 2);
        }
        if (func == 0x21) {
            GRF_PMU->SOC_CON5 = RK_GEN_VAL(5, 1, 2);
        }
        if (func == 0x20) {
            GRF_PMU->SOC_CON5 = RK_GEN_VAL(4, 1, 2);
        }
        if (func == 0x1f) {
            GRF_PMU->SOC_CON5 = RK_GEN_VAL(3, 1, 2);
        }
        if (func == 0x1e) {
            GRF_PMU->SOC_CON5 = RK_GEN_VAL(2, 1, 2);
        }
        if (func == 0x1d) {
            GRF_PMU->SOC_CON5 = RK_GEN_VAL(1, 1, 2);
        }
        if (func == 0x1c) {
            GRF_PMU->SOC_CON5 = RK_GEN_VAL(0, 1, 2);
        }
        break;
    case 4:
        break;
    default:
        break;
    }

    return HAL_OK;
}

/** @} */

/********************* Public Function Definition ****************************/

/** @defgroup PINCTRL_Exported_Functions_Group1 Suspend and Resume Functions

 This section provides functions allowing to suspend and resume the module:

 *  @{
 */

/** @} */

/** @defgroup PINCTRL_Exported_Functions_Group2 Init and DeInit Functions

 This section provides functions allowing to init and deinit the module:

 *  @{
 */
HAL_Status HAL_PINCTRL_Init(void)
{
    return HAL_OK;
}

HAL_Status HAL_PINCTRL_DeInit(void)
{
    return HAL_OK;
}
/** @} */

/** @defgroup PINCTRL_Exported_Functions_Group3 IO Functions

 *  @{
 */

/**
 * @brief  Public function to set pin parameter for multi pins.
 * @param  bank: pin bank.
 * @param  mPins: multi pins defined in @ref ePINCTRL_GPIO_PINS.
 * @param  param: multi parameters defined in @ref ePINCTRL_configParam,
 * @return HAL_Status.
 */
HAL_Status HAL_PINCTRL_SetParam(eGPIO_bankId bank, uint32_t mPins, ePINCTRL_configParam param)
{
    uint8_t pin;
    uint32_t remainPins = mPins;
    HAL_Status rc;

    HAL_ASSERT(bank < GPIO_BANK_NUM);

    if (!(param & (FLAG_MUX | FLAG_PUL | FLAG_DRV | FLAG_SRT | FLAG_SMT))) {
        HAL_DBG("pinctrl: no parameter!\n");

        return HAL_ERROR;
    }

    while (remainPins) {
        pin = __builtin_ffs(remainPins) - 1;
        rc = PINCTRL_SetPinParam(bank, pin, param);
        if (rc) {
            return rc;
        }
        remainPins &= ~(1 << pin);
    }

    return HAL_OK;
}

/**
 * @brief  Public function to set pin iomux for multi pins.
 * @param  bank: pin bank.
 * @param  mPins: multi pins defined in @ref ePINCTRL_GPIO_PINS.
 * @param  param: multi parameters defined in @ref ePINCTRL_configParam,
 * @return HAL_Status.
 */
HAL_Status HAL_PINCTRL_SetIOMUX(eGPIO_bankId bank, uint32_t mPins, ePINCTRL_configParam param)
{
    return HAL_PINCTRL_SetParam(bank, mPins, param);
}

/**
 * @brief  Public function to set Rockchip Matrix IO for one pin.
 * @param  bank: pin bank.
 * @param  rmioPin: pin defined in @ref ePINCTRL_GPIO_PINS.
 * @param  rmioFunc: RMIO function defined in @ref eRMIO_Name,
 * @return HAL_Status.
 */
HAL_Status HAL_PINCTRL_SetRMIO(eGPIO_bankId bank, uint32_t rmioPin, eRMIO_Name rmioFunc)
{
    uint8_t pin;
    uint32_t remainPins = rmioPin;
    HAL_Status rc;

    HAL_ASSERT(bank < GPIO_BANK_NUM);

    pin = __builtin_ffs(remainPins) - 1;
    /* RK2118 RMIO IOC function value is PIN_CONFIG_MUX_FUNC5 */
    rc = PINCTRL_SetIOMUX(bank, pin, 5);
    if (rc) {
        return rc;
    }
    rc = PINCTRL_SetRMIO(bank, pin, rmioFunc);
    if (rc) {
        return rc;
    }
    rc = PINCTRL_GRFSelRMIO(bank, pin, rmioFunc);
    if (rc) {
        return rc;
    }
    remainPins &= ~(1 << pin);
    if (remainPins) {
        HAL_DBG("pinctrl: set one RMIO pin at a time!\n");

        return HAL_INVAL;
    }

    return HAL_OK;
}

/** @} */

/** @} */

/** @} */

#endif /* HAL_PINCTRL_MODULE_ENABLED */
