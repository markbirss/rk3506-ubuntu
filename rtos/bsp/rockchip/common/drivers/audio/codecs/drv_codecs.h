/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2019-07-31     Xing Zheng        first version
 */

#ifndef __DRV_CODECS_H__
#define __DRV_CODECS_H__

#include "drv_clock.h"

#ifdef RT_USING_CODEC_AW8896
extern const struct codec_desc codec_aw8896;
#endif

#ifdef RT_USING_CODEC_ES7243
extern const struct codec_desc codec_es7243;
#endif

#ifdef RT_USING_CODEC_ES8156
extern const struct codec_desc codec_es8156;
#endif

#ifdef RT_USING_CODEC_ES8311
extern const struct codec_desc codec_es8311;
#endif

#ifdef RT_USING_CODEC_ES8388
extern const struct codec_desc codec_es8388;
#endif

/**
  * @brief  Codec Interface Type
  */
typedef enum
{
    IF_TYPE_I2C = 0,
    IF_TYPE_SPI,
} eInterfaceType;

/**
  * @brief  Codec Device information definition
  */
struct codec_desc
{
    eInterfaceType if_type;
    char name[RT_NAME_MAX];
    char i2c_bus[RT_NAME_MAX];
    uint8_t i2c_addr;
    uint32_t mclk_gate;
    bool mclk_always_on;
    void *private;
};

#endif /* __DRV_CODECS_H__ */
