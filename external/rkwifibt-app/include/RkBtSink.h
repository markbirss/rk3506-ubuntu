#ifndef __BLUETOOTH_SINK_H__
#define __BLUETOOTH_SINK_H__

#include <RkBtBase.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct btmg_track_info_t {
	char title[256];
	char artist[256];
	char album[256];
	char track_num[64];
	char num_tracks[64];
	char genre[256];
	char playing_time[256];
} BtTrackInfo;

typedef void (*RK_BT_AVRCP_TRACK_CHANGE_CB)(const char *bd_addr, BtTrackInfo track_info);
typedef void (*RK_BT_AVRCP_PLAY_POSITION_CB)(const char *bd_addr, int song_len, int song_pos);

int rk_bt_sink_register_track_callback(RK_BT_AVRCP_TRACK_CHANGE_CB cb);
int rk_bt_sink_register_position_callback(RK_BT_AVRCP_PLAY_POSITION_CB cb);
int rk_bt_sink_get_state(RK_BT_STATE *p_state);
int rk_bt_sink_set_volume(int volume);

#ifdef __cplusplus
}
#endif

#endif /* __BLUETOOTH_SINK_H__ */
