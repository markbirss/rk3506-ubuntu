#ifndef __BT_TEST_H__
#define __BT_TEST_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <RkBtBase.h>

void bt_test_connect_spp_by_addr(char *data);

/******************************************/
/*        BT base server init             */
/******************************************/
void bt_test_bluetooth_init(char *data);
void bt_test_bluetooth_deinit(char *data);
void bt_test_bluetooth_onoff_init(char *data);
void bt_test_version(char *data);

void bt_test_set_power(char *data);
void bt_test_set_discoverable(char *data);
void bt_test_set_pairable(char *data);
void bt_test_get_all_devices(char *data);
void bt_test_sink_media_control(char *data);
void bt_test_a2dp_test_volume(char *data);
void bt_test_set_local_name(char *data);

void bt_test_connect_by_addr(char *data);
void bt_test_disconnect_by_addr(char *data);
void bt_test_pair_by_addr(char *data);
void bt_test_unpair_by_addr(char *data);
void bt_test_start_discovery(char *data);
void bt_test_cancel_discovery(char *data);
void bt_test_read_remote_device_info(char *data);
void bt_test_sink_media_control(char *data);


void bt_test_enable_a2dp_source(char *data);
void bt_test_enable_a2dp_sink(char *data);
void bt_test_source_play(char *data);

/******************************************/
/*               BLE Test                 */
/******************************************/
void bt_test_ble_start(char *data);
void bt_test_ble_write(char *data);
void bt_test_ble_set_address(char *data);
void bt_test_ble_set_adv_interval(char *data);
void bt_test_ble_service_changed(char *data);
void bt_test_ble_stop(char *data);

/******************************************/
/*            BLE CLient Test             */
/******************************************/
void bt_test_ble_client_get_service_info(char *data);
void bt_test_ble_client_read(char *data);
void bt_test_ble_client_write(char *data);
void bt_test_ble_client_is_notify(char *data);
void bt_test_ble_client_notify_on(char *data);
void bt_test_ble_client_notify_off(char *data);
void bt_test_ble_client_enable_ancs(char *data);

/******************************************/
/*             A2DP SINK Test             */
/******************************************/
void bt_test_sink_get_play_status(char *data);
void bt_test_sink_get_poschange(char *data);

//pbap
void bt_test_pbap_get_vcf(char *data);

//opp
void bt_test_opp_send(char *data);

void bt_test_rfcomm_open(char *data);
void bt_test_rfcomm_close(char *data);
void bt_test_rfcomm_send(char *data);

void bt_test_adapter_connect(char *data);

void bt_test_state_cb(RkBtRemoteDev *rdev, RK_BT_STATE state);
bool bt_test_vendor_cb(bool enable);
int create_bt_conf(struct bt_conf *conf);

#ifdef __cplusplus
}
#endif

#endif /* __BT_TEST_H__ */
