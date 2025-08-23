#ifndef __WIFIBT_H__
#define __WIFIBT_H__

#include <RkBtBase.h>
#include <RkBtSink.h>
#include <RkBtSource.h>
#include <RkBle.h>
#include <RkBtSpp.h>
#include <RkBleClient.h>

/* WiFi server state */
enum
{
    WIFI_STATE_OFF,
    WIFI_STATE_ON,
    WIFI_STATE_CONNECTED,
};

/* WiFi server cmds */
enum
{
    WIFI_ENABLE,
    WIFI_DISABLE,
    WIFI_SCAN,
    WIFI_CONNECT,
    WIFI_DISCONNECT,
};

/* BT server state */
enum
{
    BT_STATE_OFF,
    BT_STATE_ON,
    BT_STATE_CONNECTED,
    BT_STATE_PAUSED,
    BT_STATE_PLAYING,
};
/* BT server cmds */
enum
{
    BT_ENABLE,
    BT_DISABLE,
    BT_SINK_ENABLE,
    BT_SINK_DISABLE,
    BT_SINK_PLAY,
    BT_SINK_PAUSE,
    BT_SINK_PREV,
    BT_SINK_NEXT,
    BT_SINK_VOL,
    BT_SINK_MUTE,
    BT_SINK_TRACK_CLEAR,
    BT_SINK_POS_CLEAR,
    BT_INFO,
};

struct wifibt_cmdarg
{
    int cmd;
    void *val;
    bool wait;
};

struct bt_info
{
    RK_BT_STATE bt_state;
    char bt_name[32];
    bool track_changed;
    bool pos_changed;
    char title[256];
    char artist[256];
    int pos;
    int dur;
    int vol;
};

int run_wifibt_server(void);

int run_wifi_server(void);
int wifi_query(void *buf, int len);
int wifi_query_wait(void *buf, int len);
int wifi_scanning_done(void);
int wifi_enabled(void);
int wifi_connected(void);
int wifi_init_done(void);

int run_bt_server(void);
int bt_query(void *buf, int len);
int bt_query_wait(void *buf, int len);

#endif

