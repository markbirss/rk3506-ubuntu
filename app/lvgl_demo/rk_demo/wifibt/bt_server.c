#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "main.h"

#if BT_EN
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include <RkBtSink.h>
#include <RkBtBase.h>

#include "utility.h"
#include "wifibt.h"

#define log(fmt, ...)    printf("[BT] (%s %d) "fmt "\n", __func__, __LINE__, ##__VA_ARGS__)

enum
{
    A2DP_SOURCE,
    A2DP_SINK
};

enum
{
    ACL_NORMAL_PRIORITY,
    ACL_HIGH_PRIORITY
};

static RkBtContent bt_content = {.init = 0};
static struct bt_info bt_info = {.bt_state = BT_STATE_OFF};

static int fd[2];
static sem_t sem;
struct timeval start, now;
static ssize_t totalBytes;
static int listening = 0;
static char bt_name[64];

static int bt_sink_info(struct bt_info *info)
{
    memcpy(info, &bt_info, sizeof(bt_info));
}

static int write_flush_timeout(int fd, uint16_t handle,
                               unsigned int timeout_ms)
{
    uint16_t timeout = (timeout_ms * 1000) / 625;  // timeout units of 0.625ms
    unsigned char hci_write_flush_cmd[] =
    {
        0x01,               // HCI command packet
        0x28, 0x0C,         // HCI_Write_Automatic_Flush_Timeout
        0x04,               // Length
        0x00, 0x00,         // Handle
        0x00, 0x00,         // Timeout
    };

    hci_write_flush_cmd[4] = (uint8_t)handle;
    hci_write_flush_cmd[5] = (uint8_t)(handle >> 8);
    hci_write_flush_cmd[6] = (uint8_t)timeout;
    hci_write_flush_cmd[7] = (uint8_t)(timeout >> 8);

    int ret = write(fd, hci_write_flush_cmd, sizeof(hci_write_flush_cmd));
    if (ret < 0)
    {
        log("write(): %s (%d)]", strerror(errno), errno);
        return -1;
    }
    else if (ret != sizeof(hci_write_flush_cmd))
    {
        log("write(): unexpected length %d", ret);
        return -1;
    }
    return 0;
}

static int vendor_high_priority(int fd, uint16_t handle, uint8_t priority,
                                uint8_t direction)
{
    unsigned char hci_high_priority_cmd[] =
    {
        0x01,               // HCI command packet
        0x1a, 0xfd,         // Write_A2DP_Connection
        0x04,               // Length
        0x00, 0x00,         // Handle
        0x00, 0x00          // Priority, Direction
    };

    hci_high_priority_cmd[4] = (uint8_t)handle;
    hci_high_priority_cmd[5] = (uint8_t)(handle >> 8);
    hci_high_priority_cmd[6] = (uint8_t)priority;
    hci_high_priority_cmd[7] = (uint8_t)direction;

    int ret = write(fd, hci_high_priority_cmd, sizeof(hci_high_priority_cmd));
    if (ret < 0)
    {
        log("write(): %s (%d)]", strerror(errno), errno);
        return -1;
    }
    else if (ret != sizeof(hci_high_priority_cmd))
    {
        log("write(): unexpected length %d", ret);
        return -1;
    }
    return 0;
}

static int get_acl_handle(int fd, char *bdaddr)
{
    int i;
    int ret = -1;
    struct hci_conn_list_req *conn_list;
    struct hci_conn_info *conn_info;
    int max_conn = 10;
    char addr[20];

    conn_list = malloc(max_conn * (
                           sizeof(struct hci_conn_list_req) + sizeof(struct hci_conn_info)));
    if (!conn_list)
    {
        log("Out of memory in %s\n", __FUNCTION__);
        return -1;
    }

    conn_list->dev_id = 0;  /* hardcoded to HCI device 0 */
    conn_list->conn_num = max_conn;

    if (ioctl(fd, HCIGETCONNLIST, (void *)conn_list))
    {
        log("Failed to get connection list\n");
        goto out;
    }
    log("XXX %d\n", conn_list->conn_num);

    for (i = 0; i < conn_list->conn_num; i++)
    {
        conn_info = &conn_list->conn_info[i];
        memset(addr, 0, sizeof(addr));
        snprintf(addr, sizeof(addr), "%02x:%02x:%02x:%02x:%02x:%02x",
                 conn_info->bdaddr.b[5],
                 conn_info->bdaddr.b[4],
                 conn_info->bdaddr.b[3],
                 conn_info->bdaddr.b[2],
                 conn_info->bdaddr.b[1],
                 conn_info->bdaddr.b[0]);
        log("XXX %d %s:%s\n", conn_info->type, bdaddr, addr);
        if (conn_info->type == ACL_LINK &&
                !strcasecmp(addr, bdaddr))
        {
            ret = conn_info->handle;
            goto out;
        }
    }

    ret = 0;

out:
    free(conn_list);
    return ret;
}

static int get_hci_sock(void)
{
    int sock = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
    struct sockaddr_hci addr;
    int opt;

    if (sock < 0)
    {
        log("Can't create raw HCI socket!");
        return -1;
    }

    opt = 1;
    if (setsockopt(sock, SOL_HCI, HCI_DATA_DIR, &opt, sizeof(opt)) < 0)
    {
        log("Error setting data direction\n");
        return -1;
    }

    /* Bind socket to the HCI device */
    memset(&addr, 0, sizeof(addr));
    addr.hci_family = AF_BLUETOOTH;
    addr.hci_dev = 0;  // hci0
    if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
    {
        log("Can't attach to device hci0. %s(%d)\n",
            strerror(errno),
            errno);
        return -1;
    }
    return sock;
}

static int vendor_set_high_priority(char *ba, uint8_t priority,
                                    uint8_t direction)
{
    int ret;
    int fd = get_hci_sock();
    int acl_handle;

    if (fd < 0)
        return fd;

    acl_handle = get_acl_handle(fd, ba);
    if (acl_handle <= 0)
    {
        ret = acl_handle;
        goto out;
    }

    ret = vendor_high_priority(fd, acl_handle, priority, direction);
    if (ret < 0)
        goto out;
    ret = write_flush_timeout(fd, acl_handle, 200);

out:
    close(fd);

    return ret;
}

static int bt_ba2str(const bdaddr_t *ba, char *str)
{
    return sprintf(str, "%2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X",
                   ba->b[5], ba->b[4], ba->b[3], ba->b[2], ba->b[1], ba->b[0]);
}

static void bt_set_local_name(void)
{
    int ctl;
    static struct hci_dev_info di;
    char addr[18], name[32];

    /* Open HCI socket  */
    if ((ctl = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI)) < 0)
    {
        perror("Can't open HCI socket.");
        return;
    }

    di.dev_id = 0;
    if (ioctl(ctl, HCIGETDEVINFO, (void *) &di))
    {
        perror("Can't get device info");
        return;
    }

    bt_ba2str(&di.bdaddr, addr);
    memset(bt_name, 0, sizeof(bt_name));
    sprintf(bt_name, "SCO_AUDIO_%x", di.bdaddr.b[0]);

    log("set new name: %s, %x, bdaddr: %s\n", bt_name, di.bdaddr.b[0], addr);
    //rk_bt_set_loacal_name(name);
}

static void bt_test_ble_recv_data_callback(const char *uuid, char *data,
        int *len, RK_BLE_GATT_STATE state)
{
    switch (state)
    {
        //SERVER ROLE
    case RK_BLE_GATT_SERVER_READ_BY_REMOTE:
        //The remote dev reads characteristic and put data to *data.
        log("+++ ble server is read by remote uuid: %s\n", uuid);
        *len = strlen("hello rockchip");
        memcpy(data, "hello rockchip", strlen("hello rockchip"));
        break;
    case RK_BLE_GATT_SERVER_WRITE_BY_REMOTE:
        //The remote dev writes data to characteristic so print there.
        log("+++ ble server is writeen by remote uuid: %s\n", uuid);
        for (int i = 0 ; i < *len; i++)
        {
            log("%02x ", data[i]);
        }
        log("\n");
        break;
    case RK_BLE_GATT_SERVER_ENABLE_NOTIFY_BY_REMOTE:
    case RK_BLE_GATT_SERVER_DISABLE_NOTIFY_BY_REMOTE:
        //The remote dev enable notify for characteristic
        log("+++ ble server notify is %s by remote uuid: %s\n",
            (state == RK_BLE_GATT_SERVER_ENABLE_NOTIFY_BY_REMOTE) ? "enable" : "disabled",
            uuid);
        break;
    case RK_BLE_GATT_MTU:
        //
        log("+++ ble server MTU: %d ===\n", *(uint16_t *)data);
        break;
    case RK_BLE_GATT_SERVER_INDICATE_RESP_BY_REMOTE:
        //The service sends notify to remote dev and recv indicate from remote dev.
        log("+++ ble server receive remote indicate resp uuid: %s\n", uuid);
        break;

        //CLIENT ROLE
    case RK_BLE_GATT_SERVER_READ_NOT_PERMIT_BY_REMOTE:
        //error handle: org.bluez.Error.NotPermitted
        log("+++ ble client recv error: %s +++\n", data);
    case RK_BLE_GATT_CLIENT_READ_BY_LOCAL:
        //log("+++ ble client recv from remote data uuid: %s:%d===\n", uuid, *len);
        //for (int i = 0 ; i < *len; i++) {
        //  log("%02x ", data[i]);
        //}
        //log("\n");
        //log("%02x %02x %02x \n", data[0], data[123], data[246]);
        totalBytes += *len * 8; // 转换为位
        gettimeofday(&now, NULL);
        long elapsed = (now.tv_sec - start.tv_sec) * 1000000 + now.tv_usec -
                       start.tv_usec;
        if (elapsed >= 1000000)   // 每秒计算一次
        {
            log("Rate: %ld bits/sec [%s]\n", totalBytes / (elapsed / 1000000), uuid);
            totalBytes = 0; // 重置计数器
            start = now; // 重置时间
        }
        break;
    case RK_BLE_GATT_CLIENT_WRITE_RESP_BY_LOCAL:
        break;
    case RK_BLE_GATT_CLIENT_NOTIFY_ENABLE:
    case RK_BLE_GATT_CLIENT_NOTIFY_DISABLE:
        log("+++ ble client uuid: %s notify is %s \n",
            uuid,
            (state == RK_BLE_GATT_CLIENT_NOTIFY_ENABLE) ? "enable" : "disabled"
           );
        break;
    default:
        break;
    }
}

static void bt_test_state_cb(RkBtRemoteDev *rdev, RK_BT_STATE state)
{
    switch (state)
    {
        //BASE STATE
    case RK_BT_STATE_TURNING_ON:
        log("++ RK_BT_STATE_TURNING_ON\n");
        break;
    case RK_BT_STATE_INIT_ON:
        log("++ RK_BT_STATE_INIT_ON\n");
        bt_content.init = true;
        bt_info.bt_state = BT_STATE_ON;
        log("bt_state >> %d\n", bt_info.bt_state);
        rk_bt_set_discoverable(1);
        break;
    case RK_BT_STATE_INIT_OFF:
        log("++ RK_BT_STATE_INIT_OFF\n");
        bt_content.init = false;
        bt_info.bt_state = BT_STATE_OFF;
        log("bt_state >> %d\n", bt_info.bt_state);
        break;

        //SCAN STATE
    case RK_BT_STATE_SCAN_NEW_REMOTE_DEV:
        if (rdev != NULL)
        {
            if (rdev->paired)
                log("+ PAIRED_DEV: [%s|%d]:%s:%s\n", rdev->remote_address, rdev->rssi,
                    rdev->remote_address_type, rdev->remote_alias);
            else
                log("+ SCAN_NEW_DEV: [%s|%d]:%s:%s\n", rdev->remote_address, rdev->connected,
                    rdev->remote_address_type, rdev->remote_alias);
        }
        break;
    case RK_BT_STATE_SCAN_CHG_REMOTE_DEV:
        if (rdev != NULL)
        {
            log("+ SCAN_CHG_DEV: [%s|%d]:%s:%s|%s\n", rdev->remote_address, rdev->rssi,
                rdev->remote_address_type, rdev->remote_alias, rdev->change_name);

            if (!strcmp(rdev->change_name, "UUIDs"))
            {
                for (int index = 0; index < 36; index++)
                {
                    if (!strcmp(rdev->remote_uuids[index], "NULL"))
                        break;
                    log("\tUUIDs: %s\n", rdev->remote_uuids[index]);
                }
            }
            else if (!strcmp(rdev->change_name, "Icon"))
            {
                log("\tIcon: %s\n", rdev->icon);
            }
            else if (!strcmp(rdev->change_name, "Class"))
            {
                log("\tClass: 0x%x\n", rdev->cod);
            }
            else if (!strcmp(rdev->change_name, "Modalias"))
            {
                log("\tModalias: %s\n", rdev->modalias);
            }
        }
        break;
    case RK_BT_STATE_SCAN_DEL_REMOTE_DEV:
        if (rdev != NULL)
            log("+ SCAN_DEL_DEV: [%s]:%s:%s\n", rdev->remote_address,
                rdev->remote_address_type, rdev->remote_alias);
        break;

        //LINK STATE
    case RK_BT_STATE_CONNECTED:
    case RK_BT_STATE_DISCONN:
        bt_info.bt_state = rdev->connected ? BT_STATE_ON : BT_STATE_OFF;
        log("bt_state >> %d\n", bt_info.bt_state);
        if (rdev != NULL)
            log("+ %s [%s|%d]:%s:%s\n",
                rdev->connected ? "STATE_CONNECTED" : "STATE_DISCONNECTED",
                rdev->remote_address,
                rdev->rssi,
                rdev->remote_address_type,
                rdev->remote_alias);
        break;
    case RK_BT_STATE_PAIRED:
    case RK_BT_STATE_PAIR_NONE:
        if (rdev != NULL)
            log("+ %s [%s|%d]:%s:%s\n", rdev->paired ? "STATE_PAIRED" : "STATE_PAIR_NONE",
                rdev->remote_address,
                rdev->rssi,
                rdev->remote_address_type,
                rdev->remote_alias);
        break;
    case RK_BT_STATE_DEL_DEV_OK:
        if (rdev != NULL)
            log("+ RK_BT_STATE_DEL_DEV_OK: %s:%s:%s\n",
                rdev->remote_address,
                rdev->remote_address_type,
                rdev->remote_alias);
        break;
    case RK_BT_STATE_PAIR_FAILED:
        log("+ STATE_BOND/PAIR FAILED\n");
        break;

    case RK_BT_STATE_CONNECT_FAILED:
        if (rdev != NULL)
            log("+ STATE_FAILED [%s|%d]:%s:%s reason: %s\n",
                rdev->remote_address,
                rdev->rssi,
                rdev->remote_address_type,
                rdev->remote_alias,
                rdev->change_name);
        break;
    case RK_BT_STATE_DISCONN_ALREADY:
        bt_info.bt_state = BT_STATE_ON;
        log("bt_state >> %d\n", bt_info.bt_state);
        log("+ STATE_DISCONNECTED: RK_BT_STATE_DISCONN_ALREADY\n");
        break;
    case RK_BT_STATE_DISCONN_FAILED:
        log("+ STATE_FAILED: RK_BT_STATE_DISCONN_FAILED\n");
        break;

    case RK_BT_STATE_CONNECTED_ALREADY:
        bt_info.bt_state = BT_STATE_CONNECTED;
        log("bt_state >> %d\n", bt_info.bt_state);
        log("+ STATE_CONNECTED: RK_BT_STATE_CONNECTED_ALREADY\n");
        break;
    case RK_BT_STATE_CONNECT_FAILED_INVAILD_ADDR:
        log("+ STATE_FAILED: RK_BT_STATE_CONNECT_FAILED_INVAILD_ADDR\n");
        break;
    case RK_BT_STATE_CONNECT_FAILED_NO_FOUND_DEVICE:
        log("+ STATE_FAILED: RK_BT_STATE_CONNECT_FAILED_NO_FOUND_DEVICE\n");
        break;
    case RK_BT_STATE_CONNECT_FAILED_SCANNING:
        log("+ STATE_FAILED: RK_BT_STATE_CONNECT_FAILED_SCANNING\n");
        break;

    case RK_BT_STATE_DEL_DEV_FAILED:
        log("+ STATE_FAILED: RK_BT_STATE_DEL_DEV_FAILED\n");
        break;

        //MEDIA A2DP SOURCE
    case RK_BT_STATE_SRC_ADD:
    case RK_BT_STATE_SRC_DEL:
        if (rdev != NULL)
        {
            log("+ STATE SRC MEDIA %s [%s|%d]:%s:%s\n",
                (state == RK_BT_STATE_SRC_ADD) ? "ADD" : "DEL",
                rdev->remote_address,
                rdev->rssi,
                rdev->remote_address_type,
                rdev->remote_alias);
            log("+ codec: %s, freq: %s, chn: %s\n",
                rdev->media.codec == 0 ? "SBC" : "UNKNOW",
                rdev->media.sbc.frequency == 1 ? "48K" : "44.1K",
                rdev->media.sbc.channel_mode == 1 ? "JOINT_STEREO" : "STEREO");
        }
        break;

        //MEDIA AVDTP TRANSPORT
    case RK_BT_STATE_TRANSPORT_VOLUME:
        if (rdev != NULL)
            log("+ STATE AVDTP TRASNPORT VOLUME[%d] [%s|%d]:%s:%s\n",
                rdev->media.volume,
                rdev->remote_address,
                rdev->rssi,
                rdev->remote_address_type,
                rdev->remote_alias);
        break;
    case RK_BT_STATE_TRANSPORT_IDLE:
        if (rdev != NULL)
        {
            log("+ STATE AVDTP TRASNPORT IDLE [%s|%d]:%s:%s\n",
                rdev->remote_address,
                rdev->rssi,
                rdev->remote_address_type,
                rdev->remote_alias);
            //low priority for broadcom
            vendor_set_high_priority(rdev->remote_address, ACL_NORMAL_PRIORITY,
                                     bt_content.profile & PROFILE_A2DP_SINK_HF ? A2DP_SINK : A2DP_SOURCE);
        }
        break;
    case RK_BT_STATE_TRANSPORT_PENDING:
        if (rdev != NULL)
            log("+ STATE AVDTP TRASNPORT PENDING [%s|%d]:%s:%s\n",
                rdev->remote_address,
                rdev->rssi,
                rdev->remote_address_type,
                rdev->remote_alias);
        break;
    case RK_BT_STATE_TRANSPORT_ACTIVE:
        if (rdev != NULL)
        {
            log("+ STATE AVDTP TRASNPORT ACTIVE [%s|%d]:%s:%s\n",
                rdev->remote_address,
                rdev->rssi,
                rdev->remote_address_type,
                rdev->remote_alias);
            //high priority for broadcom
            vendor_set_high_priority(rdev->remote_address, ACL_HIGH_PRIORITY,
                                     bt_content.profile & PROFILE_A2DP_SINK_HF ? A2DP_SINK : A2DP_SOURCE);
        }
        break;
    case RK_BT_STATE_TRANSPORT_SUSPENDING:
        if (rdev != NULL)
            log("+ STATE AVDTP TRASNPORT SUSPEND [%s|%d]:%s:%s\n",
                rdev->remote_address,
                rdev->rssi,
                rdev->remote_address_type,
                rdev->remote_alias);
        break;

        //MEDIA A2DP SINK
    case RK_BT_STATE_SINK_ADD:
    case RK_BT_STATE_SINK_DEL:
        if (rdev != NULL)
        {
            log("+ STATE SINK MEDIA %s [%s|%d]:%s:%s\n",
                (state == RK_BT_STATE_SINK_ADD) ? "ADD" : "DEL",
                rdev->remote_address,
                rdev->rssi,
                rdev->remote_address_type,
                rdev->remote_alias);
            log("+ codec: %s, freq: %s, chn: %s\n",
                rdev->media.codec == 0 ? "SBC" : "UNKNOW",
                rdev->media.sbc.frequency == 1 ? "48K" : "44.1K",
                rdev->media.sbc.channel_mode == 1 ? "JOINT_STEREO" : "STEREO");
        }
        break;
    case RK_BT_STATE_SINK_PLAY:
        bt_info.bt_state = BT_STATE_PLAYING;
        log("bt_state >> %d\n", bt_info.bt_state);
        if (rdev != NULL)
            log("+ STATE SINK PLAYER PLAYING [%s|%d]:%s:%s\n",
                rdev->remote_address,
                rdev->rssi,
                rdev->remote_address_type,
                rdev->remote_alias);
        break;
    case RK_BT_STATE_SINK_STOP:
        bt_info.bt_state = BT_STATE_PAUSED;
        log("bt_state >> %d\n", bt_info.bt_state);
        if (rdev != NULL)
            log("+ STATE SINK PLAYER STOP [%s|%d]:%s:%s\n",
                rdev->remote_address,
                rdev->rssi,
                rdev->remote_address_type,
                rdev->remote_alias);
        break;
    case RK_BT_STATE_SINK_PAUSE:
        bt_info.bt_state = BT_STATE_PAUSED;
        log("bt_state >> %d\n", bt_info.bt_state);
        if (rdev != NULL)
            log("+ STATE SINK PLAYER PAUSE [%s|%d]:%s:%s\n",
                rdev->remote_address,
                rdev->rssi,
                rdev->remote_address_type,
                rdev->remote_alias);
        break;
    case RK_BT_STATE_SINK_TRACK:
        log("+ STATE SINK TRACK INFO [%s|%d]:%s:%s track[%s]-[%s]\n",
            rdev->remote_address,
            rdev->rssi,
            rdev->remote_address_type,
            rdev->remote_alias,
            rdev->title,
            rdev->artist);
        strcpy(bt_info.title, rdev->title);
        strcpy(bt_info.artist, rdev->artist);
        bt_info.track_changed = 1;
        break;
    case RK_BT_STATE_SINK_POSITION:
        log("+ STATE SINK TRACK POSITION:[%s|%d]:%s:%s [%u-%u]\n",
            rdev->remote_address,
            rdev->rssi,
            rdev->remote_address_type,
            rdev->remote_alias,
            rdev->player_position,
            rdev->player_total_len);
        bt_info.pos = rdev->player_position;
        bt_info.dur = rdev->player_total_len;
        bt_info.pos_changed = 1;
        break;

        //ADV
    case RK_BT_STATE_ADAPTER_BLE_ADV_START:
        bt_content.ble_content.ble_advertised = true;
        log("RK_BT_STATE_ADAPTER_BLE_ADV_START successful\n");
        break;
    case RK_BT_STATE_ADAPTER_BLE_ADV_STOP:
        bt_content.ble_content.ble_advertised = false;
        log("RK_BT_STATE_ADAPTER_BLE_ADV_STOP successful\n");
        break;

        //ADAPTER STATE
    case RK_BT_STATE_ADAPTER_NO_DISCOVERYABLED:
        bt_content.discoverable = false;
        log("RK_BT_STATE_ADAPTER_NO_DISCOVERYABLED successful\n");
        rk_bt_set_discoverable(1);
        break;
    case RK_BT_STATE_ADAPTER_DISCOVERYABLED:
        bt_content.discoverable = true;
        log("RK_BT_STATE_ADAPTER_DISCOVERYABLED successful\n");
        break;
    case RK_BT_STATE_ADAPTER_NO_PAIRABLED:
        bt_content.pairable = false;
        log("RK_BT_STATE_ADAPTER_NO_PAIRABLED successful\n");
        break;
    case RK_BT_STATE_ADAPTER_PAIRABLED:
        bt_content.pairable = true;
        log("RK_BT_STATE_ADAPTER_PAIRABLED successful\n");
        break;
    case RK_BT_STATE_ADAPTER_NO_SCANNING:
        bt_content.scanning = false;
        log("RK_BT_STATE_ADAPTER_NO_SCANNING successful\n");
        break;
    case RK_BT_STATE_ADAPTER_SCANNING:
        bt_content.scanning = true;
        log("RK_BT_STATE_ADAPTER_SCANNING successful\n");
        break;
    case RK_BT_STATE_ADAPTER_POWER_ON:
        bt_content.power = true;
        log("RK_BT_STATE_ADAPTER_POWER_ON successful\n");
        break;
    case RK_BT_STATE_ADAPTER_POWER_OFF:
        bt_content.power = false;
        log("RK_BT_STATE_ADAPTER_POWER_OFF successful\n");
        break;

    case RK_BT_STATE_COMMAND_RESP_OK:
        log("RK_BT_STATE CMD OK\n");
        break;
    case RK_BT_STATE_COMMAND_RESP_ERR:
        log("RK_BT_STATE CMD ERR\n");
        break;

    default:
        if (rdev != NULL)
            log("+ DEFAULT STATE %d: %s:%s:%s RSSI: %d [CBP: %d:%d:%d]\n", state,
                rdev->remote_address,
                rdev->remote_address_type,
                rdev->remote_alias,
                rdev->rssi,
                rdev->connected,
                rdev->paired,
                rdev->bonded);
        break;
    }
}

static bool bt_test_vendor_cb(bool enable)
{
    int times = 100;

    if (enable)
    {
        //wait hci0 appear
        while (times-- > 0 && access("/sys/class/bluetooth/hci0", F_OK))
        {
            usleep(100 * 1000);
        }

        if (access("/sys/class/bluetooth/hci0", F_OK) != 0)
        {
            log("The hci0 init failure!\n");
            return false;
        }

        //TODO
        //bt_set_local_name();
        //bt_content.bt_name = bt_name;

        //start bluetoothd
        //exec_command_system("/usr/libexec/bluetooth/bluetoothd -d -n -P battery,hostname,gap,wiimote -f /data/main.conf &");

        exec_command_system("killall bluetoothd && sleep 0.5");
        exec_command_system("/usr/libexec/bluetooth/bluetoothd -n -P battery,hostname,gap,wiimote -f /data/main.conf &");

        //check bluetoothd
        times = 100;
        while (times-- > 0 && !(get_ps_pid("bluetoothd")))
        {
            usleep(100 * 1000);
        }

        if (!get_ps_pid("bluetoothd"))
        {
            log("The bluetoothd boot failure!\n");
            return false;
        }
    }
    else
    {
        //CLEAN
        exec_command_system("hciconfig hci0 down");
        exec_command_system("killall bluetoothd");

        //vendor deinit
        //if (get_ps_pid("brcm_patchram_plus1"))
        //    kill_task("killall brcm_patchram_plus1");
        //if (get_ps_pid("rtk_hciattach"))
        //    kill_task("killall rtk_hciattach");

        //audio server deinit
        if (get_ps_pid("bluealsa"))
            kill_task("bluealsa");
        if (get_ps_pid("bluealsa-alay"))
            kill_task("bluealsa-alay");
    }

    return true;
}

static bool bt_test_audio_server_cb(bool enable)
{
    char rsp[64];

    log("%s bt_content.profile: 0x%x:0x%x:0x%x\n", __func__, bt_content.profile,
        (bt_content.profile & PROFILE_A2DP_SINK_HF),
        (bt_content.profile & PROFILE_A2DP_SOURCE_AG));

    if (bt_content.bluealsa == true)
    {
        //use bluealsa
        //exec_command("pactl list modules | grep bluetooth | grep policy", rsp, 64);
        exec_command("pactl list modules | grep bluetooth", rsp, 64);
        if (rsp[0])
        {
            exec_command_system("pactl unload-module module-bluetooth-policy");
            exec_command_system("pactl unload-module module-bluetooth-discover");
        }

        exec_command_system("killall bluealsa bluealsa-aplay");

        if ((bt_content.profile & PROFILE_A2DP_SINK_HF) == PROFILE_A2DP_SINK_HF)
        {
            exec_command_system("bluealsa -S --profile=a2dp-sink --profile=hfp-hf &");
            //Sound Card: default
            exec_command_system("bluealsa-aplay -S --profile-a2dp 00:00:00:00:00:00 &");
        }
        else if ((bt_content.profile & PROFILE_A2DP_SOURCE_AG) ==
                 PROFILE_A2DP_SOURCE_AG)
        {
            exec_command_system("bluealsa -S --profile=a2dp-source --profile=hfp-ag --a2dp-volume &");
        }
    }

    return true;
}

#define BT_CONF_DIR "/data/main.conf"
static int create_bt_conf(struct bt_conf *conf)
{
    FILE *fp;
    char cmdline[256] = {0};

    fp = fopen(BT_CONF_DIR, "wt+");
    if (NULL == fp)
        return -1;

    fputs("[General]\n", fp);

    //DiscoverableTimeout
    if (conf->discoverableTimeout)
    {
        sprintf(cmdline, "DiscoverableTimeout = %s\n", conf->discoverableTimeout);
        fputs(cmdline, fp);
    }

    //BleName
    if (conf->BleName)
    {
        sprintf(cmdline, "BleName = %s\n", conf->BleName);
        fputs(cmdline, fp);
    }

    //class
    if (conf->Class)
    {
        sprintf(cmdline, "Class = %s\n", conf->Class);
        fputs(cmdline, fp);
    }

    //SSP
    if (conf->ssp)
    {
        sprintf(cmdline, "SSP = %s\n", conf->ssp);
        fputs(cmdline, fp);
    }

    //mode
    if (conf->mode)
    {
        sprintf(cmdline, "ControllerMode = %s\n", conf->mode);
        fputs(cmdline, fp);
    }

    //default always
    conf->JustWorksRepairing = "always";
    sprintf(cmdline, "JustWorksRepairing = %s\n", conf->JustWorksRepairing);
    fputs(cmdline, fp);

    fputs("[GATT]\n", fp);
    //#Cache = always
    fputs("Cache = always", fp);

    fclose(fp);

    if (system("cat /data/main.conf"))
        log("cat /data/main.conf failed");

    return 0;
}

static int bt_ble_init(void)
{
    RkBleGattService *gatt;
    struct bt_conf conf;

    //"indicate"
    static char *chr_props[] = { "read", "write", "notify", "write-without-response", NULL };

    log("%s \n", __func__);

    //Must determine whether Bluetooth is turned on
    if (rk_bt_is_open())
    {
        log("%s: already open \n", __func__);
        bt_info.bt_state = BT_STATE_ON;
        return 0;
    }

    memset(&bt_content, 0, sizeof(RkBtContent));

    //BREDR CLASS BT NAME
    memset(bt_content.bt_name, 0, sizeof(bt_content.bt_name));
    strcpy(bt_content.bt_name, "rkbt");

    //BLE NAME
    memset(bt_content.ble_content.ble_name, 0,
           sizeof(bt_content.ble_content.ble_name));
    strcpy(bt_content.ble_content.ble_name, "rkble");

    char bt_id[6] = {0};
    FILE *fp = fopen("/data/bt_id.txt", "r+");
    if (fp)
    {
        fread(bt_id, 1, 6, fp);
        fclose(fp);
    }
    else
    {
        for (int i = 0; i < 6; i++)
        {
            bt_id[i] = '0' + (rand() % 10);
        }
        fp = fopen("/data/bt_id.txt", "w");
        fwrite(bt_id, 1, 6, fp);
        fclose(fp);
    }

    char buf[38];
    memset(buf, 38, sizeof(buf));
    snprintf(buf, sizeof(buf), "%s_%s", bt_content.bt_name, bt_id);
    memcpy(bt_content.bt_name, buf, sizeof(bt_content.bt_name));

    memcpy(bt_info.bt_name, bt_content.bt_name, sizeof(bt_info.bt_name));

    memset(buf, 38, sizeof(buf));
    snprintf(buf, sizeof(buf), "%s_%s", bt_content.ble_content.ble_name, bt_id);
    memcpy(bt_content.ble_content.ble_name, buf,
           sizeof(bt_content.ble_content.ble_name));

    //IO CAPABILITY
    bt_content.io_capability = IO_CAPABILITY_DISPLAYYESNO;

    /*
     * Only one can be enabled
     * a2dp sink and hfp-hf
     * a2dp source and hfp-ag
     */
    bt_content.profile = PROFILE_A2DP_SINK_HF;
    bt_content.bluealsa = true;

    rk_bt_register_state_callback(bt_test_state_cb);
    rk_bt_register_vendor_callback(bt_test_vendor_cb);
    rk_bt_register_audio_server_callback(bt_test_audio_server_cb);

    //default state
    bt_content.init = false;
    bt_content.connecting = false;
    bt_content.scanning = false;
    bt_content.discoverable = false;
    bt_content.pairable = false;
    bt_content.power = false;

    //bt config file
    memset(&conf, 0, sizeof(struct bt_conf));
    //both BR/EDR and LE enabled, "dual", "le" or "bredr"
    conf.mode = "dual";
    //0 = disable timer, i.e. stay discoverable forever
    conf.discoverableTimeout = "0";
    //"audio-headset"
    conf.Class = "0x240414";
    create_bt_conf(&conf);

    rk_debug_init(true);

    rk_bt_init(&bt_content);
}

static int bt_ble_deinit(void)
{
    rk_bt_deinit();
}

static void *bt_server(void *arg)
{
    struct wifibt_cmdarg *cmdarg;
    intptr_t intval;
    char buf[1024];
    char **key;
    int timeout;
    int len;

    while (access("/tmp/.lv_warmup", F_OK) == 0)
        sleep(1);

    listening = 1;
    while (1)
    {
        int num = read(fd[0], buf, sizeof(buf));
        cmdarg = (struct wifibt_cmdarg *)buf;
        switch (cmdarg->cmd)
        {
        case BT_ENABLE:
            log("BT_ENABLE\n");
            bt_ble_init();
            timeout = 3;
            while (!bt_content.init && (timeout--))
                sleep(1);
            if (timeout != 0)
            {
                rk_bt_set_profile(PROFILE_A2DP_SINK_HF, true);
            }
            break;
        case BT_DISABLE:
            log("BT_DISABLE\n");
            bt_ble_deinit();
            break;
        case BT_SINK_ENABLE:
            log("BT_SINK_ENABLE\n");
            if (!bt_content.init)
                break;
            rk_bt_set_profile(PROFILE_A2DP_SINK_HF, true);
            break;
        case BT_SINK_DISABLE:
            log("BT_SINK_DISABLE\n");
            rk_bt_sink_media_control("pause");
            break;
        case BT_SINK_PLAY:
            log("BT_SINK_PLAY\n");
            rk_bt_sink_media_control("play");
            break;
        case BT_SINK_PAUSE:
            log("BT_SINK_PAUSE\n");
            rk_bt_sink_media_control("pause");
            break;
        case BT_SINK_PREV:
            log("BT_SINK_PREV\n");
            rk_bt_sink_media_control("previous");
            break;
        case BT_SINK_NEXT:
            log("BT_SINK_NEXT\n");
            rk_bt_sink_media_control("next");
            break;
        case BT_SINK_VOL:
            intval = (intptr_t)cmdarg->val;
            log("BT_SINK_VOL %d\n", intval);
            rk_bt_sink_set_volume(intval);
            break;
        case BT_SINK_MUTE:
            log("BT_SINK_MUTE\n");
            rk_bt_sink_set_volume(0);
            break;
        case BT_INFO:
            bt_sink_info((struct bt_info *)cmdarg->val);
            break;
        case BT_SINK_TRACK_CLEAR:
            bt_info.track_changed = 0;
            break;
        case BT_SINK_POS_CLEAR:
            bt_info.pos_changed = 0;
            break;
        default:
            log("Unknow cmd %d\n", cmdarg->cmd);
            break;
        }
        if (cmdarg->wait)
            sem_post(&sem);
    }
}

int bt_query_wait(void *buf, int len)
{
    struct wifibt_cmdarg *cmdarg = (struct wifibt_cmdarg *)buf;

    if (!listening)
        return -1;

    cmdarg->wait = true;
    int ret = write(fd[1], buf, len);
    if (ret <= 0)
        return ret;
    sem_wait(&sem);

    return ret;
}

int bt_query(void *buf, int len)
{
    struct wifibt_cmdarg *cmdarg = (struct wifibt_cmdarg *)buf;

    if (!listening)
        return -1;

    cmdarg->wait = false;

    log("%d", fd[1]);
    return write(fd[1], buf, len);
}

int run_bt_server(void)
{
    pthread_t tid;
    int ret;

    sem_init(&sem, 0, 0);

    ret = pipe(fd);
    if (ret != 0)
    {
        log("bt server init failed\n");
        return ret;
    }
    log("%d %d", fd[0], fd[1]);

    ret = pthread_create(&tid, NULL, bt_server, NULL);
    if (ret < 0)
    {
        close(fd[0]);
        close(fd[1]);
        log("bt server start failed\n");
    }

    return ret;
}
#endif

