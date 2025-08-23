/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_gpio.c
  * @author  Jay Xu
  * @version V0.1
  * @date    2019/5/15
  * @brief   GPIO Driver
  *
  ******************************************************************************
  */

/** @addtogroup RKBSP_Driver_Reference
 *  @{
 */

/** @addtogroup GPIO
 *  @{
 */

/** @defgroup GPIO_How_To_Use How To Use
 *  @{

 The GPIO driver use to configure or control GPIO pins on SoCs, it can be used in
 the following three scenarios:

- (A) The GPIO PIN APIs provide by pin component:
    - 1) rt_pin_read - get pin level, pin number caculated by BANK_PIN(banknum, pinnum);
    - 2) rt_pin_write- set pin level, pin number caculated by BANK_PIN(banknum, pinnum);
    - 3) rt_pin_mode - set pin input/output, pin number caculated by BANK_PIN(banknum, pinnum);
- (B) The GPIO IRQ APIs provide by pin component:
    - 1) pin_attach_irq;
    - 2) pin_detach_irq;
    - 3) pin_irq_enable;
- (C) The GPIO PIN NUMBER calculated by BANK_PIN(b,p), such as
    BANK_PIN(0, 5) means GPIO0_A5
    BANK_PIN(1, 8) means GPIO1_B0

 See more information, click [here](http://www.rt-thread.org/document/site/programming-manual/device/pin/pin/)

 @} */

#include <rthw.h>
#include <rtdevice.h>
#include <rtthread.h>

#ifdef RT_USING_PIN

#include "hal_base.h"
#include "drv_gpio.h"

/********************* Private MACRO Definition ******************************/
#define PIN_NUM(p) ((p & GPIO_PIN_MASK) >> GPIO_PIN_SHIFT)
#define PIN_BANK(p) ((p & GPIO_BANK_MASK) >> GPIO_BANK_SHIFT)

#define BANK_PIN_DEFAULT (-1)

#ifdef HAL_GPIO_VIRTUAL_MODEL_FEATURE_ENABLED
#ifdef GPIO0
#undef GPIO0
#define GPIO0 GPIO0_EXP
#undef GPIO0_IRQn
#define GPIO0_IRQn GPIO0_EXP_IRQn
#endif
#ifdef GPIO1
#undef GPIO1
#define GPIO1 GPIO1_EXP
#undef GPIO1_IRQn
#define GPIO1_IRQn GPIO1_EXP_IRQn
#endif
#ifdef GPIO2
#undef GPIO2
#define GPIO2 GPIO2_EXP
#undef GPIO2_IRQn
#define GPIO2_IRQn GPIO2_EXP_IRQn
#endif
#ifdef GPIO3
#undef GPIO3
#define GPIO3 GPIO3_EXP
#undef GPIO3_IRQn
#define GPIO3_IRQn GPIO3_EXP_IRQn
#endif
#ifdef GPIO4
#undef GPIO4
#define GPIO4 GPIO4_EXP
#undef GPIO4_IRQn
#define GPIO4_IRQn GPIO4_EXP_IRQn
#endif
#endif
/********************* Private Structure Definition **************************/

static struct GPIO_REG *GPIO_GROUP[] =
{
#ifdef GPIO0
    GPIO0,
#endif
#ifdef GPIO1
    GPIO1,
#endif
#ifdef GPIO2
    GPIO2,
#endif
#ifdef GPIO3
    GPIO3,
#endif
#ifdef GPIO4
    GPIO4,
#endif
};

#define GPIO_BANK_NUM       HAL_ARRAY_SIZE(GPIO_GROUP)

#define get_st_gpio(p)      (GPIO_GROUP[PIN_BANK(p)])
#define get_st_pin(p)       (HAL_BIT(PIN_NUM(p)))

static struct rt_pin_irq_hdr pin_irq_hdr_tab[GPIO_BANK_NUM * PIN_NUMBER_PER_BANK];

/********************* Private Function Definition ***************************/
/** @defgroup GPIO_Private_Function Private Function
 *  @{
 */

static rt_err_t pin_attach_irq(struct rt_device *device, rt_int32_t pin,
                               rt_uint32_t mode, void (*hdr)(void *args), void *args)
{
    rt_base_t level;

    if (pin < 0 || pin >= HAL_ARRAY_SIZE(pin_irq_hdr_tab))
    {
        return RT_ENOSYS;
    }

    level = rt_hw_interrupt_disable();
    if (pin_irq_hdr_tab[pin].pin == pin &&
            pin_irq_hdr_tab[pin].hdr == hdr &&
            pin_irq_hdr_tab[pin].mode == mode &&
            pin_irq_hdr_tab[pin].args == args)
    {
        rt_hw_interrupt_enable(level);
        return RT_EOK;
    }

    if (pin_irq_hdr_tab[pin].pin != BANK_PIN_DEFAULT && pin_irq_hdr_tab[pin].hdr != RT_NULL)
    {
        rt_hw_interrupt_enable(level);
        return RT_EBUSY;
    }

    pin_irq_hdr_tab[pin].pin = pin;
    pin_irq_hdr_tab[pin].hdr = hdr;
    pin_irq_hdr_tab[pin].mode = mode;
    pin_irq_hdr_tab[pin].args = args;

    rt_hw_interrupt_enable(level);

    return RT_EOK;
}

static rt_err_t pin_detach_irq(struct rt_device *device, rt_int32_t pin)
{
    rt_base_t level;

    if (pin < 0 || pin >= HAL_ARRAY_SIZE(pin_irq_hdr_tab))
    {
        return RT_ENOSYS;
    }

    level = rt_hw_interrupt_disable();
    if (pin_irq_hdr_tab[pin].pin == BANK_PIN_DEFAULT)
    {
        rt_hw_interrupt_enable(level);
        return RT_EOK;
    }

    pin_irq_hdr_tab[pin].pin = BANK_PIN_DEFAULT;
    pin_irq_hdr_tab[pin].hdr = RT_NULL;
    pin_irq_hdr_tab[pin].mode = 0;
    pin_irq_hdr_tab[pin].args = RT_NULL;

    rt_hw_interrupt_enable(level);

    return RT_EOK;
}

static rt_err_t pin_irq_enable(struct rt_device *dev, rt_base_t pin, rt_uint32_t enabled)
{
    rt_base_t level;
    eGPIO_intType mode;

    RT_ASSERT(PIN_BANK(pin) < GPIO_BANK_NUM);

    if (enabled == PIN_IRQ_ENABLE)
    {
        if (pin < 0 || pin >= HAL_ARRAY_SIZE(pin_irq_hdr_tab))
        {
            return RT_ENOSYS;
        }
        level = rt_hw_interrupt_disable();
        if (pin_irq_hdr_tab[pin].pin == BANK_PIN_DEFAULT)
        {
            rt_hw_interrupt_enable(level);
            return RT_ENOSYS;
        }

        switch (pin_irq_hdr_tab[pin].mode)
        {
        case PIN_IRQ_MODE_RISING:
            mode = GPIO_INT_TYPE_EDGE_RISING;
            break;
        case PIN_IRQ_MODE_FALLING:
            mode = GPIO_INT_TYPE_EDGE_FALLING;
            break;
        case PIN_IRQ_MODE_RISING_FALLING:
            mode = GPIO_INT_TYPE_EDGE_BOTH;
            break;
        case PIN_IRQ_MODE_LOW_LEVEL:
            mode = GPIO_INT_TYPE_LEVEL_LOW;
            break;
        case PIN_IRQ_MODE_HIGH_LEVEL:
            mode = GPIO_INT_TYPE_LEVEL_HIGH;
            break;
        default:
            rt_hw_interrupt_enable(level);
            return RT_EINVAL;
        }

        HAL_GPIO_SetIntType(get_st_gpio(pin), get_st_pin(pin), mode);
        HAL_GPIO_EnableIRQ(get_st_gpio(pin), get_st_pin(pin));
        rt_hw_interrupt_enable(level);
    }
    else if (enabled == PIN_IRQ_DISABLE)
    {
        HAL_GPIO_DisableIRQ(get_st_gpio(pin), get_st_pin(pin));
    }
    else
    {
        return RT_ENOSYS;
    }

    return RT_EOK;
}

static void pin_mode(struct rt_device *dev, rt_base_t pin, rt_base_t mode)
{
    RT_ASSERT(PIN_BANK(pin) < GPIO_BANK_NUM);

    switch (mode)
    {
    case PIN_MODE_OUTPUT:
    case PIN_MODE_OUTPUT_OD:
#ifdef HAL_PINCTRL_MODULE_ENABLED
        HAL_PINCTRL_SetIOMUX(PIN_BANK(pin), HAL_BIT(pin & 0x1f), PIN_CONFIG_MUX_FUNC0);
#endif
        HAL_GPIO_SetPinDirection(get_st_gpio(pin), get_st_pin(pin), GPIO_OUT);
        break;

    case PIN_MODE_INPUT:
#ifdef HAL_PINCTRL_MODULE_ENABLED
        HAL_PINCTRL_SetIOMUX(PIN_BANK(pin), HAL_BIT(pin & 0x1f), PIN_CONFIG_MUX_FUNC0);
#endif
        HAL_GPIO_SetPinDirection(get_st_gpio(pin), get_st_pin(pin), GPIO_IN);
        break;

    case PIN_MODE_INPUT_PULLUP:
#ifdef HAL_PINCTRL_MODULE_ENABLED
        HAL_PINCTRL_SetIOMUX(PIN_BANK(pin), HAL_BIT(pin & 0x1f), PIN_CONFIG_MUX_FUNC0 | PIN_CONFIG_PUL_UP);
#endif
        HAL_GPIO_SetPinDirection(get_st_gpio(pin), get_st_pin(pin), GPIO_IN);
        break;

    case PIN_MODE_INPUT_PULLDOWN:
#ifdef HAL_PINCTRL_MODULE_ENABLED
        HAL_PINCTRL_SetIOMUX(PIN_BANK(pin), HAL_BIT(pin & 0x1f), PIN_CONFIG_MUX_FUNC0 | PIN_CONFIG_PUL_DOWN);
#endif
        HAL_GPIO_SetPinDirection(get_st_gpio(pin), get_st_pin(pin), GPIO_IN);
        break;

    default:
        break;
    }
}

static void pin_write(struct rt_device *dev, rt_base_t pin, rt_base_t value)
{
    RT_ASSERT(PIN_BANK(pin) < GPIO_BANK_NUM);
    HAL_GPIO_SetPinLevel(get_st_gpio(pin), get_st_pin(pin), value);
}

static int pin_read(struct rt_device *dev, rt_base_t pin)
{
    RT_ASSERT(PIN_BANK(pin) < GPIO_BANK_NUM);
    return HAL_GPIO_GetPinLevel(get_st_gpio(pin), get_st_pin(pin));;
}

/**
 * Px.y
 *  x: gpio bank number, 0, 1, 2, ... valid num
 *  y: gpio pin number, 0, 1, 2, ..., 31.
 *
 * Rockchip SoC trm or hardware maybe called GPIO0_A1, the '0' is 'x',
 * and the A1 convert to 'y' by following rules:
 *  A0~A7 is 0~7
 *  B0~B7 is 8~15
 *  C0~C7 is 16~23
 *  D0~D7 is 24~31
 *
 * Example of use: Px.0 ~ Px.31, x:0,1,2,3,...
 */
static rt_base_t pin_get(const char *name)
{
    rt_base_t pin = 0;
    int bank_num = 0, pin_num = 0;

    if (name[0] != 'P' && name[0] != 'p')
    {
        return -RT_EINVAL;
    }

    bank_num = atoi(&name[1]);
    if (bank_num >= GPIO_BANK_NUM)
    {
        return -RT_EINVAL;
    }

    if (name[2] == '.')
    {
        pin_num = atoi(&name[3]);
    }
    else
    {
        pin_num = atoi(&name[2]);
    }
    if (pin_num >= 32)
    {
        return -RT_EINVAL;
    }

    rt_kprintf("%s --> gpio%d.%02d --> gpio%d.%c%d\n", name, bank_num, pin_num, bank_num, 'A' + pin_num / 8, pin_num % 8);

    pin = BANK_PIN(bank_num, pin_num);

    return pin;
}
/** @} */

#ifdef GPIO0
void pin_gpio0_handler(void)
{
    rt_interrupt_enter();
    HAL_GPIO_IRQHandler(GPIO0, GPIO_BANK0);
    rt_interrupt_leave();
}
#endif
#ifdef GPIO1
void pin_gpio1_handler(void)
{
    rt_interrupt_enter();
    HAL_GPIO_IRQHandler(GPIO1, GPIO_BANK1);
    rt_interrupt_leave();
}
#endif
#ifdef GPIO2
void pin_gpio2_handler(void)
{
    rt_interrupt_enter();
    HAL_GPIO_IRQHandler(GPIO2, GPIO_BANK2);
    rt_interrupt_leave();
}
#endif
#ifdef GPIO3
void pin_gpio3_handler(void)
{
    rt_interrupt_enter();
    HAL_GPIO_IRQHandler(GPIO3, GPIO_BANK3);
    rt_interrupt_leave();
}
#endif
#ifdef GPIO4
void pin_gpio4_handler(void)
{
    rt_interrupt_enter();
    HAL_GPIO_IRQHandler(GPIO4, GPIO_BANK4);
    rt_interrupt_leave();
}
#endif

static const struct rt_pin_ops pin_ops =
{
    pin_mode,
    pin_write,
    pin_read,
    pin_attach_irq,
    pin_detach_irq,
    pin_irq_enable,
    pin_get,
};

/** @defgroup GPIO_Public_Functions Public Functions
 *  @{
 */
int rt_hw_gpio_init(void)
{
#ifdef GPIO0
    rt_hw_interrupt_install(GPIO0_IRQn, (void *)pin_gpio0_handler, RT_NULL, RT_NULL);
    rt_hw_interrupt_umask(GPIO0_IRQn);
#endif
#ifdef GPIO1
    rt_hw_interrupt_install(GPIO1_IRQn, (void *)pin_gpio1_handler, RT_NULL, RT_NULL);
    rt_hw_interrupt_umask(GPIO1_IRQn);
#endif
#ifdef GPIO2
    rt_hw_interrupt_install(GPIO2_IRQn, (void *)pin_gpio2_handler, RT_NULL, RT_NULL);
    rt_hw_interrupt_umask(GPIO2_IRQn);
#endif
#ifdef GPIO3
    rt_hw_interrupt_install(GPIO3_IRQn, (void *)pin_gpio3_handler, RT_NULL, RT_NULL);
    rt_hw_interrupt_umask(GPIO3_IRQn);
#endif
#ifdef GPIO4
    rt_hw_interrupt_install(GPIO4_IRQn, (void *)pin_gpio4_handler, RT_NULL, RT_NULL);
    rt_hw_interrupt_umask(GPIO4_IRQn);
#endif

    rt_device_pin_register("pin", &pin_ops, RT_NULL);

    return 0;
}
INIT_BOARD_EXPORT(rt_hw_gpio_init);
/** @} */

static void pin_irq_hdr(uint32_t pin)
{
    RT_ASSERT(pin >= 0);
    RT_ASSERT(pin < HAL_ARRAY_SIZE(pin_irq_hdr_tab));

    pin_irq_hdr_tab[pin].hdr(pin_irq_hdr_tab[pin].args);
}

void HAL_GPIO_IRQDispatch(eGPIO_bankId bank, uint32_t pin)
{
    uint32_t pin_num = BANK_PIN(bank, pin);
    RT_ASSERT(bank < GPIO_BANK_NUM);

    if (pin_irq_hdr_tab[pin_num].hdr != RT_NULL)
        pin_irq_hdr(pin_num);
}

#endif
/** @} */

/** @} */
