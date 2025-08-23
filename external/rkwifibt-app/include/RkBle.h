#ifndef __BLUETOOTH_BLE_H__
#define __BLUETOOTH_BLE_H__

#include <RkBtBase.h>

#ifdef __cplusplus
extern "C" {
#endif

void rk_ble_service_changed(void);

/**
 * @ingroup  rk_bt_basic
 * @brief  rk_ble_send_notify
 *
 * @par Description
        rk_ble_send_notify
 *
 * @attention
 * @param  uuid [IN]
 * @param  data [IN]
 * @param  len [IN]
 *
 * @retval 0 Excute successfully, see attention.
 * @retval -1 Error code
 * @see  NULL
 */
int rk_ble_send_notify(const char *uuid, char *data, int len);

/*smallest value: 32(32 * 0.625ms = 20ms)*/
int rk_ble_set_adv_interval(unsigned short adv_int_min, unsigned short adv_int_max);

/**
 * @ingroup  rk_bt_basic
 * @brief  rk_ble_adv_start
 *
 * @par Description
        rk_ble_adv_start
 *
 * @attention
 * @param  NULL
 *
 * @retval 0 Excute successfully, see attention.
 * @retval -1 Error code
 * @see  NULL
 */
int rk_ble_adv_start(void);

/**
 * @ingroup  rk_bt_basic
 * @brief  rk_ble_adv_stop
 *
 * @par Description
        rk_ble_adv_stop
 *
 * @attention
 * @param  NULL
 *
 * @retval 0 Excute successfully, see attention.
 * @retval -1 Error code
 * @see  NULL
 */
int rk_ble_adv_stop();

#ifdef __cplusplus
}
#endif

#endif /* __BLUETOOTH_BLE_H__ */
