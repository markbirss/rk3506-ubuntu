// SPDX-License-Identifier: GPL-2.0
//#define _GNU_SOURCE
#ifndef LOG_TAG
#define LOG_TAG "hciattach_rk"
#endif

//#include <android/log.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <time.h>
#include <limits.h>
#include <stdint.h>
#include <stdbool.h>
#include <poll.h>
#include <sys/poll.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <utils/Log.h>
#include <assert.h>

#include "hciattach.h"

#ifndef FIRMWARE_DIR
#define FIRMWARE_DIR "/vendor/etc/firmware/"  /* maguro */
#endif


#define STREAM_TO_UINT16(u16, p) {u16 = ((uint16_t)(*(p)) + (((uint16_t)(*((p) + 1))) << 8)); (p) += 2;}
#define UINT8_TO_STREAM(p, u8)   {*(p)++ = (uint8_t)(u8);}
#define UINT16_TO_STREAM(p, u16) {*(p)++ = (uint8_t)(u16); *(p)++ = (uint8_t)((u16) >> 8);}
#define UINT32_TO_STREAM(p, u32) {*(p)++ = (uint8_t)(u32); *(p)++ = (uint8_t)((u32) >> 8); *(p)++ = (uint8_t)((u32) >> 16); *(p)++ = (uint8_t)((u32) >> 24);}

enum {
    RK960_C1 = 0,
    RK960_D1,
    RK960_E1,
};

enum {
    STAGE_UPDATE_LOADER_BAUDRATE = 0,
    STAGE_UPDATE_FW_WORK_BAUDRATE,
};

#define RK960_C1_STACK_BASE 0x20160000
#define RK960_D1_STACK_BASE 0x20173400
#define RK960_E1_STACK_BASE 0x20113400


#define RK960_C1_LOADER_ROM_BASE 0x20100000
#define RK960_D1_LOADER_ROM_BASE 0x20172000
#define RK960_E1_LOADER_ROM_BASE 0x20112000

#define RK960_FW_ROM_BASE 0x00200000

#define RK960_LOADER_NAME "rk960_bt_loader.bin"

#define RK960_FW1                              "rk960_bt_rf"
#define RK960_FW2                              "rk960_bt"
#define RK960_FW1_EXECUTION_TIME               400

#define PARAM_ID_BD_ADDRESS 0x01
#define PARAM_ID_DEVICE_NAME 0x02

#define PARAM_LEN_BD_ADDRESS 6
#define PARAM_LEN_DEVICE_NAME 248

#define SET_FW_RUN_ADDR_SIZE  4

//Buffer structure
struct buffer_tag
{
	// length of buffer
	uint8_t length;
	// data of 128 bytes length
	uint8_t data[128];
};

typedef struct hci_op_par_cmd
{
	//Parameter tag
	uint8_t param_tag;
	//Structure buffer
	struct buffer_tag buf;
} hci_op_par_cmd_t;

static int loader_type = RK960_D1;

static unsigned int do_js_hash(unsigned int hash, unsigned char *buf, unsigned int len)
{
    unsigned int i  = 0;

    for(i = 0; i < len; i++) {
        hash ^= ((hash << 5) + buf[i] + (hash >> 2));
    }
    return hash;
}

#define OSI_NO_INTR(fn)  do {} while ((fn) == -1 && errno == EINTR)

static int UartWrite(int fd, void *data, uint32_t size)
{
    int ret;

    OSI_NO_INTR(ret = write(fd, data, size));
    return ret;
}

static int UartRead(int fd, void *data, uint32_t size)
{
    int ret;

    OSI_NO_INTR(ret = read(fd, data, size));
    return ret;
}

static int UartReadn(int fd, uint8_t *data, uint32_t size)
{
    uint32_t nleft = size;
    int32_t nread;
    BT_ERROR("%s toRead size :%d\n", __func__, size);

    while(nleft > 0){
        if ((nread = UartRead(fd, data, nleft)) < 0) {
            BT_DBG("%s error:%d\n", __func__, errno);
            if(nleft == size)
                return (-1);
            else
                break;
        }
        else if(nread == 0){
            break; /* EOF */
        }
        nleft -= nread;
        data += nread;
        BT_DBG("%s toRead left :%d\n", __func__, nleft);
    }

    BT_DBG("%s return :%d, nread = %d\n", __func__, (size - nleft), nread);
    return (size - nleft);
 }

static void uart_set_hw_fctrl(int fd, uint8_t hw_fctrl)
{
    struct termios termios_old;

    if (fd == -1)
    {
        BT_ERROR("vnd_userial.fd is -1");
        return;
    }

    tcgetattr(fd, &termios_old);
    if(hw_fctrl)
    {
        if(termios_old.c_cflag & CRTSCTS)
        {
            BT_DBG("userial_vendor_set_hw_fctrl already hw flowcontrol on");
            return;
        }
        else
        {
            termios_old.c_cflag |= CRTSCTS;
            tcsetattr(fd, TCSANOW, &termios_old);
            BT_DBG("userial_vendor_set_hw_fctrl set hw flowcontrol on");
        }
    }
    else
    {
        if(termios_old.c_cflag & CRTSCTS)
        {
            termios_old.c_cflag &= ~CRTSCTS;
            tcsetattr(fd, TCSANOW, &termios_old);
            BT_DBG("userial_vendor_set_hw_fctrl set hw flowcontrol off");
            return;
        }
        else
        {
            BT_DBG("userial_vendor_set_hw_fctrl already hw flowcontrol off");
            return;
        }
    }
}

int set_loader_type(int type)
{
    loader_type = type;
    return 0;
}

int get_loader_type(void)
{
    return loader_type;
}

int check_loader_type(char * bin)
{
    int fw_fd;
    char fw_path[128] = {0};
    uint32_t loader_stack_base = 0;
    char fw_patchfile_path[256] = FIRMWARE_DIR;

    sprintf(fw_path, "%s", fw_patchfile_path);
    if (fw_patchfile_path[strlen(fw_patchfile_path)- 1] != '/') {
        strcat(fw_path, "/");
    }
    strcat(fw_path, bin);

    if ((fw_fd = open(fw_path, O_RDONLY)) == -1) {
        BT_ERROR("vendor lib preload failed to open [%s]", fw_path);
        return (-1);
    }
    read(fw_fd, &loader_stack_base, 4);
    if (loader_stack_base == RK960_D1_STACK_BASE)
        set_loader_type(RK960_D1);
    else if (loader_stack_base == RK960_C1_STACK_BASE)
        set_loader_type(RK960_C1);
    else if (loader_stack_base == RK960_E1_STACK_BASE)
        set_loader_type(RK960_E1);
    else
        set_loader_type(RK960_E1);

    close(fw_fd);

    return 0;
}

int32_t hw_get_loader_load_addr(void)
{
    int loader_type = get_loader_type();
    if (loader_type == RK960_C1)
        return RK960_C1_LOADER_ROM_BASE;
    else if (loader_type == RK960_D1)
        return RK960_D1_LOADER_ROM_BASE;
    else if (loader_type == RK960_E1)
        return RK960_E1_LOADER_ROM_BASE;
    else
        return RK960_E1_LOADER_ROM_BASE;
}


static int rk_bt_boot(int fd, char* bin)
{
    struct _download_request
    {
        uint32_t tag;//'REQ '
        uint16_t cmd;//0x0471 or 0x0472
        uint32_t data_length;//byte unit
        uint16_t package_size;
        uint16_t timeout;     //ms
        uint32_t load_addr;//secotr unit
        uint32_t data_hash;
        uint8_t  reserved[6];
        uint32_t req_hash;
    } __attribute__((packed));

    struct _download_response
    {
        uint32_t tag;//'ACK '
        uint8_t reserved[6];
        uint8_t cause;
        uint8_t status;//0:ok,1:fail
    } __attribute__((packed));

    uint32_t read_buf[128];
    char fw_path[128] = {0};

    int i, j;
    uint32_t size, package_size;
    struct _download_request req;
    struct _download_response resp;
    uint8_t *pBuf;
    uint8_t *pFirmware;

    char fw_patchfile_path[256] = FIRMWARE_DIR;
    const char* READY_TAG = "RKUART";
    const uint8_t REQ_TAG[4] = {'R', 'E', 'Q', ' '};
    const uint8_t RESP_TAG[4] = {'A', 'C', 'K', ' '};

    struct termios ti;

    uart_set_hw_fctrl(fd, 0);
    usleep(20 * 1000);

    tcflush(fd, TCIFLUSH);
    tcflush(fd, TCOFLUSH);
    tcflush(fd, TCIOFLUSH);

    sprintf(fw_path, "%s", fw_patchfile_path);
    if (fw_patchfile_path[strlen((const char *)fw_patchfile_path)- 1] != '/')
    {
        strcat((char *)fw_path, "/");
    }
    strcat((char *)fw_path, bin);

    BT_DBG("Updated Loader Target name = [%s]", (const char *)fw_path);

    package_size = 1024;
    pBuf = (uint8_t *)read_buf;
    size = strlen(READY_TAG)*2-1;

    /*wait device ready*/
    int count = 0, len;
    do
    {
        len = UartReadn(fd, pBuf, size);

        for (i=0; i<len; i++)
        {
            BT_DBG("recv=[%c],hex= [0x%02x], total: %d, cnt:%d\n", pBuf[i], pBuf[i], len, count);
        }

        if(strstr((const char*)pBuf, READY_TAG)!= NULL)
            break;
    }while(++count);

    int fw_fd;
    pBuf = (uint8_t *) malloc(package_size);
    if(!pBuf)
        return -1;

    if ((fw_fd = open(fw_path, O_RDONLY)) == -1)
    {
        BT_ERROR("vendor lib preload failed to open [%s], reason:%s", fw_path, strerror(errno));
        free(pBuf);
        pBuf = NULL;
        return (-1);
    }

    uint32_t fileSize = lseek(fw_fd, 0, SEEK_END);
    lseek(fw_fd, 0, SEEK_SET);
    /*download firmware*/
    memset(&req, 0, sizeof(req));
    req.tag = *((uint32_t *)REQ_TAG);
    req.cmd = 0x0471;
    req.data_length = fileSize;//sizeof(DownloadBin);
    req.package_size = package_size;
    req.timeout = 5000; // 5S
    req.load_addr = hw_get_loader_load_addr();
    //req.data_hash = do_js_hash(0, (unsigned char *)DownloadBin, sizeof(DownloadBin));
    //req.req_hash = do_js_hash(0, (unsigned char *)&req, sizeof(req)-4);
    req.data_hash = 0;

    while((len = read(fw_fd, pBuf, package_size)) > 0){
        req.data_hash = do_js_hash(req.data_hash, (unsigned char *)pBuf, len);
    }
    req.req_hash = do_js_hash(0, (unsigned char *)&req, sizeof(req)-4);

    tcflush(fd, TCIFLUSH);
    tcflush(fd, TCOFLUSH);
    tcflush(fd, TCIOFLUSH);
    len = UartWrite(fd, &req, sizeof(req));

    BT_DBG("%s, to write REQ_TAG:%u, writed:%d, fileSize:%d!\n", __func__, (unsigned int)sizeof(req), len, fileSize);

    do{
        /*read response*/
        len = UartReadn(fd, (uint8_t *)&resp, sizeof(struct _download_response));
        for (i=0; i<len; i++)
        {
            BT_DBG("RESP_TAG=[%c],hex= [0x%02x], total: %d, cnt:%d\n", ((uint8_t*)&resp)[i], ((uint8_t*)&resp)[i], len, count);
        }

        BT_DBG("%s, to Read RESP_TAG:%u, readed:%d!\n", __func__, (unsigned int)sizeof(struct _download_response), len);

        if (resp.tag != *((uint32_t *)RESP_TAG)||(resp.status !=0))
        {
            BT_DBG("RESP0 Error:0x%x!\n", resp.tag);
            //userial_vendor_close();
            while(1);
        }
    }while(0);

    lseek(fw_fd, 0, SEEK_SET);
    size = fileSize;
    BT_DBG("fw download size :%u, start...\n", fileSize);
    while (size)
    {
        len = read(fw_fd, pBuf, package_size);

        i = UartWrite(fd, pBuf, len);
        BT_DBG("fw uart towrite:%u, done:%u\n", len, i);
        /*read response*/
        i = UartReadn(fd, (uint8_t *)&resp, sizeof(struct _download_response));
        BT_DBG("fw read RESP:%u, done:%u\n", (unsigned int)sizeof(struct _download_response), i);
        if (resp.tag != *((uint32_t *)RESP_TAG)||(resp.status !=0))
        {
            BT_DBG("!!!!!!!!RESP1 Error:0x%x!, fileSize:%d, left size:%d, status:%d\n", resp.tag, fileSize, size, resp.status);
            free(pBuf);
            pBuf = NULL;
            close(fw_fd);
            return -1;
        }

        size -= len;
    }

    if (pBuf != NULL){
        pBuf = NULL;
        free(pBuf);
    }

    close(fw_fd);

    //usleep(20 * 1000);
    BT_DBG("UARTBoot download OK!\n");
    return 0;
}

static void util_hexdump(const uint8_t *buf, size_t len)
{
    static const char hexdigits[] = "0123456789abcdef";
    char str[16 * 3];
    size_t i;

    if (!buf || !len)
        return;

    for (i = 0; i < len; i++) {
        str[((i % 16) * 3)] = hexdigits[buf[i] >> 4];
        str[((i % 16) * 3) + 1] = hexdigits[buf[i] & 0xf];
        str[((i % 16) * 3) + 2] = ' ';
        if ((i + 1) % 16 == 0) {
            str[16 * 3 - 1] = '\0';
            BT_INFO("%s", (char*)str);
        }
    }

    if (i % 16 > 0) {
        str[(i % 16) * 3 - 1] = '\0';
        BT_INFO("%s", (char*)str);
    }
}
static int start_xfer_wait(int fd, uint8_t *cmd, uint16_t len, uint32_t msec,
               int retry, uint8_t *resp, uint16_t *resp_len)
{
    uint8_t buf[64];
    int result;
    int state = 1;
    int count = 0;
    int params_len;
    struct pollfd p[2];
    uint16_t opcode;

    if (fd == -1 || !cmd || len < 4) {
        BT_ERROR("%s: invalid parameter", __func__);
        return -1;
    }

    opcode = ((uint16_t)cmd[2] << 8) + cmd[1];

start_xfer:
    result = write(fd, cmd, len);
    if (result != len) {
        BT_ERROR("%s: Write cmd %04x error, %s", __func__, opcode,
               strerror(errno));
        return -1;
    }

start_recv:
    memset(buf, 0, sizeof(buf));
    memset(p, 0, sizeof(p));
    state = 1;
    count = 0;
    p[0].fd = fd;
    p[0].events = POLLERR | POLLHUP | POLLIN;
    for (;;) {
        p[0].revents = 0;
        result = poll(p, 1, msec);
        if (result < 0) {
            BT_ERROR("Poll call error, %s", strerror(errno));
            result = -1;
            break;
        }

        if (result == 0) {
            BT_WARN("%s: Timeout", __func__);
            if (retry <= 0) {
                BT_ERROR("%s: Transfer exhausted", __func__);
                tcflush(fd, TCIOFLUSH);
                exit(EXIT_FAILURE);
                return -1;
            }
            retry--;
            goto start_xfer;
        }

        if (p[0].revents & (POLLERR | POLLHUP)) {
            BT_ERROR("POLLERR or POLLUP happens, %s",
                   strerror(errno));
            result = -1;
            break;
        }

        if (state == 1) {
            result = read(p[0].fd, buf, 1);
            if (result == -1 || result != 1) {
                BT_ERROR("%s: Read pkt type error, %s", __func__,
                       strerror(errno));
                result = -1;
                break;
            }
            if (result == 1 && buf[0] == 0x04) {
                count = 1;
                state = 2;
            }
        } else if (state == 2) {
            result = read(p[0].fd, buf + count, 2);
            if (result == -1 || result != 2) {
                BT_ERROR("%s: Read pkt header error, %s",
                       __func__, strerror(errno));
                break;
            }
            count += result;
            state = 3;
            params_len = buf[2];
            if (params_len + 3 > sizeof(buf)) {
                result = -1;
                BT_ERROR("%s: hci event too long", __func__);
                break;
            }
        } else if (state == 3) {
            result = read(p[0].fd, buf + count, params_len);
            if (result == -1) {
                BT_ERROR("%s: Read pkt payload error, %s",
                       __func__, strerror(errno));
                break;
            }
            count += result;
            params_len -= result;
            if (!params_len)
                break;
        }
    }

    if (result >= 0) {
        if (buf[1] == 0x0e) {
            uint16_t tmp_opcode;

            tmp_opcode = (uint16_t)buf[4] | buf[5] << 8;
            if (tmp_opcode == opcode) {
                BT_INFO("Cmd complete event for cmd %04x",
                    opcode);
                /* Status is not zero indicating command not
                 * succeeded */
                if (buf[6])
                    return -1;
                if (!resp)
                    return 0;
                if (*resp_len > count)
                    *resp_len = count;
                memcpy(resp, buf, *resp_len);
                return 0;
            } else {
                BT_WARN("Unexpected cmd complete event, %04x",
                    tmp_opcode);
                return -1;
            }
        } else {
            BT_INFO("%s: Unexpected hci event packet", __func__);
            util_hexdump(buf, count);
            /* Continue receiving */
        }
        goto start_recv;
    }

    return result;
}

//vendor cmd to change uart speed
//rk960 works in 115200 buad in default.
static int rk_bt_hci_change_stage_baudrate(int fd,int stage,int speed)
{
    uint8_t tx_buf[10];
    uint8_t resp[7];//min size.
    uint16_t rlen = sizeof(resp);
    uint8_t *p = tx_buf;
    int result;

    UINT8_TO_STREAM(p, 0x01);
    if (stage == STAGE_UPDATE_LOADER_BAUDRATE) {
        UINT16_TO_STREAM(p, 0xFC18);
    } else if (stage == STAGE_UPDATE_FW_WORK_BAUDRATE) {
        UINT16_TO_STREAM(p, 0xFC84);
    } else {
        UINT16_TO_STREAM(p, 0xFC18);
    }
    *p++ = 6;
    *p++ = 0;
    *p++ = 0;
    UINT32_TO_STREAM(p,speed);

    result = start_xfer_wait(fd, tx_buf, sizeof(tx_buf), 1000, 3, resp, &rlen);

    if (result < 0) {
    BT_ERROR("rk_bt_hci_change_stage_baudrate failed:%d", result);
        return -1;
    }

    if (rlen != 7) {
        BT_ERROR("%s: rk_bt_hci_change_stage_baudrate length %u", __func__, rlen);
        return -1;
    }

    return 0;
}

static int rk_bt_change_stage_baudrate(int fd, int stage, int speed, struct termios *ti)
{
    if (get_loader_type() >= RK960_D1) {
        if (set_speed(fd, ti, 115200) < 0) {
            BT_ERROR("Can't set baud rate: %d, %d", fd, speed);
            return -1;
        }
        uart_set_hw_fctrl(fd,0);
        rk_bt_hci_change_stage_baudrate(fd, stage, speed);
    }

    if (set_speed(fd, ti, speed) < 0) {
        BT_ERROR("Can't set baud rate: %d, %d", fd, speed);
        return -1;
    }
    uart_set_hw_fctrl(fd,0);

    return 0;
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))
static int rk_bt_read_local_name(int fd, char *name, int len)
{
    unsigned char cmd[] = { 0x01, 0x14, 0x0C, 0x00 };
    //local name max len is 248
    unsigned char resp[248+7];
    uint16_t rlen = sizeof(resp);
    unsigned int name_len;
        int result;

        result = start_xfer_wait(fd, cmd, sizeof(cmd), 1000, 0, resp, &rlen);

        if (result < 0) {
            BT_ERROR("rk bt read local name failed:%d", result);
            goto fail;
        }

    //Num_HCI_Command_Packts(1B) + Command_Opcode(2B) + Status(1B)
    name_len = resp[2] - 4;

    memcpy(name, (char *) &resp[7], MIN(name_len, len));
    name[MIN(name_len, len) - 1] = 0;

        BT_WARN("chip name:%s", name);
    return 0;

fail:
    return -1;
}

static int hw_strncmp (const char *p_str1, const char *p_str2, const int len)
{
    int i;

    if (!p_str1 || !p_str2)
        return (1);

    for (i = 0; i < len; i++)
    {
        if (toupper(p_str1[i]) != toupper(p_str2[i]))
            return (i+1);
    }

    return 0;
}

#define FW_PATCHFILE_EXTENSION      ".bin"
#define FW_PATCHFILE_EXTENSION_LEN  4
#define FW_PATCHFILE_PATH_MAXLEN    248 /* Local_Name length of return of
                                           HCI_Read_Local_Name */

#define FW_TABLE_VERSION "v0.1 20210726"

/* RK FW auto detection table */
typedef struct {
    char *chip_id;
    char *updated_chip_id;
} fw_auto_detection_entry_t;

static const fw_auto_detection_entry_t fw_auto_detection_table[] = {
    {"rk960","rk960"},    //rk960
    { NULL, NULL}
}; 

#include <ctype.h>
static char *_strupr(char *str){
    char *orign=str;
    for (; *str!='\0'; str++)
        *str = toupper(*str);
    return orign;
}
static char *_strlow(char *str){
    char *orign=str;
    for (; *str!='\0'; str++)
        *str = tolower(*str);
    return orign;
}

static int rk_bt_fetch_fw(const char *dir_name,
        const char *chip_name, char *location)
{
    DIR *dirp;
    struct dirent *dp;
    int filenamelen;
    uint8_t retval = FALSE;
    fw_auto_detection_entry_t *p_entry;
    char chip_id_fw[32] = {0};

    BT_DBG("Chip name = [%s]", chip_name);
    sprintf(chip_id_fw, "%s", chip_name);
    _strlow(chip_id_fw);


    BT_DBG("###ROCKCHIP FW Auto detection patch version = [%s]###", FW_TABLE_VERSION);
    //p_entry = (fw_auto_detection_entry_t *)fw_auto_detection_table;
    //while (p_entry->chip_id != NULL)
    //{
    //    if (strstr(chip_id_fw, p_entry->chip_id)!=NULL)
    //    {
    //        strcpy(chip_id_fw,p_entry->updated_chip_id);
    //        break;
    //    }
    //    p_entry++;
    //}
    // strcat(chip_id_fw, "_bt");
     strcat(chip_id_fw, FW_PATCHFILE_EXTENSION);

    BT_DBG("Updated Target name = [%s], dir_name=%s", chip_id_fw, dir_name);

    if ((dirp = opendir(dir_name)) != NULL)
    {
        /* Fetch next filename in patchfile directory */
        while ((dp = readdir(dirp)) != NULL)
        {
            /* Check if filename starts with chip-id name */
            if ((hw_strncmp(dp->d_name, chip_id_fw, strlen(chip_id_fw)) \
                ) == 0)
            {
                /* Check if it has .bin extenstion */
                filenamelen = strlen(dp->d_name);
                if ((filenamelen >= FW_PATCHFILE_EXTENSION_LEN) &&
                    ((hw_strncmp(
                          &dp->d_name[filenamelen-FW_PATCHFILE_EXTENSION_LEN], \
                          FW_PATCHFILE_EXTENSION, \
                          FW_PATCHFILE_EXTENSION_LEN) \
                     ) == 0))
                {
                    BT_ERROR("Found patchfile: [%s][%s]", \
                        dir_name, dp->d_name);

                    /* Make sure length does not exceed maximum */
                    if ((filenamelen + strlen(dir_name)) > \
                         FW_PATCHFILE_PATH_MAXLEN)
                    {
                        BT_ERROR("Invalid patchfile name (too long)");
                    }
                    else
                    {
                        memset(location, 0, FW_PATCHFILE_PATH_MAXLEN);
                        /* Found patchfile. Store location and name */
                        strcpy(location, dir_name);
                        if (dir_name[strlen(dir_name)- 1] != '/')
                        {
                            strcat(location, "/");
                        }
                        strcat(location, dp->d_name);
                        retval = TRUE;
                    }
                    break;
                }
            }
        }

        closedir(dirp);
    }
    else
    {
        BT_ERROR("Could not open %s", dir_name);
    }

    return (retval);
}

#define PACK_SIZE 255
#define MAX_SEGS 2
static int rk_bt_fw_probe(int fd, int fw_fd, int fwsize, int *position)
{
    uint32_t hash = 0;
    uint16_t length;
    uint32_t count =0;
    uint32_t n_segs = 0;
    uint8_t tx_buf[1024];
    uint8_t resp[7];//min size.
    uint16_t rlen = sizeof(resp);
    uint8_t *p = tx_buf;
    int result;

    struct seg_t {
        uint32_t vmaddr;
        uint32_t length;
        } segs[MAX_SEGS];

    length = read(fw_fd, &n_segs, 4);

    for(int i = 0; i < n_segs && i < MAX_SEGS; i++){
        read(fw_fd, &segs[i].vmaddr, 4);
        read(fw_fd, &segs[i].length, 4);
    }

    if (segs[0].length + segs[1].length + 20 != fwsize) {
        n_segs = 2;
        segs[0].vmaddr = RK960_FW_ROM_BASE;
        segs[0].length = fwsize/2;
        segs[1].vmaddr = RK960_FW_ROM_BASE + segs[0].length;
        segs[1].length = fwsize - segs[0].length;

        *position = lseek(fw_fd, 0, SEEK_SET);
    }

    *position = lseek(fw_fd, 0, SEEK_CUR);

    while((length = read(fw_fd, p, PACK_SIZE)) > 0){
        hash = do_js_hash(hash, (unsigned char *)p, length);
        //BT_DBG("do_js_hash , count=%04d", count);
    }

    UINT8_TO_STREAM(p, 0x01);
    UINT16_TO_STREAM(p, 0xFC2D);
    *p++ = (4 + (n_segs)*(4+4) + 4); /* parameter length: segs + (segs) * (addr size) + hash*/
    UINT32_TO_STREAM(p, n_segs);

    for(uint32_t i = 0; i < n_segs && i < MAX_SEGS; i++){
        UINT32_TO_STREAM(p, segs[i].vmaddr);
        UINT32_TO_STREAM(p, segs[i].length);
    }

    UINT32_TO_STREAM(p, hash);//hash

    tx_buf[3] = (4 + (n_segs)*(4+4) + 4); /* parameter length: segs + (segs) * (addr size) + hash*/

    result = start_xfer_wait(fd, tx_buf, 3+1+(4 + (n_segs)*(4+4) + 4), 1000, 0, resp, &rlen);

    if (result < 0) {
        BT_ERROR("probe patch failed");
        return -1;
    }

    if (rlen != 7) {
        BT_ERROR("%s: Unexpected length %u", __func__, rlen);
        return -1;
    }

    return 0;
}

static int rk_bt_fw_transfer(int fd, int fw_fd)
{
    uint8_t tx_buf[1024] = {0x01, 0x2e, 0xfc, 0x00};
    uint8_t resp[7];//min size.
    uint16_t rlen = sizeof(resp);
    uint8_t *p = tx_buf;
    uint8_t fragment;
    uint16_t index =0;
    int result;

    while((fragment = read(fw_fd, p+4, PACK_SIZE))>0)
    {
        //UINT8_TO_STREAM(p, 0x01);
        //UINT16_TO_STREAM(p, 0xFC2E);
        //UINT8_TO_STREAM(p, fragment);
        tx_buf[0] = 0x01;
        tx_buf[1] = 0x2e;
        tx_buf[2] = 0xfc;
        tx_buf[3] = fragment;

        BT_ERROR("fragment = %d", fragment);

        result = start_xfer_wait(fd, tx_buf, 4 + fragment, 1000, 0, resp, &rlen);
        if (result < 0) {
            BT_ERROR("Transfer fragment failed, index %d", index);
            return -1;
        }

        if (rlen != 7) {
            BT_ERROR("%s: Unexpected length %u", __func__, rlen);
            return -1;
        }
        index++;
    }

    BT_DBG("%s: fw download OK", __func__);
    return 0;
}

static int rk_bt_fw_download(int fd, const char *fw_whole)
{
    struct stat st;
    uint32_t fwSize;
    int position;
    int fwfd;

    if (stat(fw_whole, &st) < 0){
        BT_ERROR("Can't access firmware:%s, errno:%s", fw_whole, strerror(errno));
    }

    fwSize = st.st_size;
    if ((fwfd = open(fw_whole, O_RDONLY)) == -1){
        BT_ERROR("vendor lib preload failed to open [%s]", fw_whole);
    }
    else{
        rk_bt_fw_probe(fd, fwfd, fwSize, &position);

        BT_ERROR("lseek position:%d", position);

        lseek(fwfd, position, SEEK_SET);
        rk_bt_fw_transfer(fd, fwfd);
        close(fwfd);
        fwfd = -1;
    }

    return 0;
}

static bool str_to_bda(const char *string, uint8_t *addr) {
    assert(string != NULL);
    assert(addr != NULL);

    bool ret = sscanf(string, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
        &addr[5], &addr[4], &addr[3], &addr[2], &addr[1], &addr[0]);

    BT_DBG("bdaddr = 0x%02x:%02x:%02x:%02x:%02x:%02x", addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
    return ret;
}

static int rk_bt_set_launch_addr(int fd, uint32_t lunchAddr)
{
    uint8_t tx_buf[] = {0x01, 0x0a, 0xfc, 0x04, 0x00, 0x00, 0x00, 0x00};
    uint8_t resp[7];//min size.
    uint16_t rlen = sizeof(resp);
    uint8_t *p = tx_buf;
    int result;

    UINT8_TO_STREAM(p, 0x01);
    UINT16_TO_STREAM(p, 0xFC0A);
    *p++ = SET_FW_RUN_ADDR_SIZE;
    UINT32_TO_STREAM(p, lunchAddr);

    result = start_xfer_wait(fd, tx_buf, sizeof(tx_buf), 1000, 0, resp, &rlen);

    if (result < 0) {
        BT_ERROR("set lunchaddr failed:%d", result);
        return -1;
    }

    if (rlen != 7) {
        BT_ERROR("%s: write bdaddr length %u", __func__, rlen);
        return -1;
    }

    return 0;
}


static int rk_bt_write_nvds_addr(int fd, const char *bdaddr)
{
    uint8_t tx_buf[4+sizeof(hci_op_par_cmd_t)] = { 0x01, 0x0a, 0xfc, 130, 0x01, 0x06,
                0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
    uint8_t resp[7];//min size.
    uint16_t rlen = sizeof(resp);
    uint8_t *p = tx_buf;
    int result;

    hci_op_par_cmd_t wr_nvds_paras;
    UINT8_TO_STREAM(p, 0x01);
    UINT16_TO_STREAM(p, 0xFC0A);
    *p++ = sizeof(hci_op_par_cmd_t);

    memset((uint8_t*)&wr_nvds_paras, 0x00, sizeof(hci_op_par_cmd_t));
    wr_nvds_paras.param_tag = PARAM_ID_BD_ADDRESS;
    wr_nvds_paras.buf.length = PARAM_LEN_BD_ADDRESS;
    if(bdaddr != NULL)
    str_to_bda(bdaddr, (uint8_t *)(wr_nvds_paras.buf.data));//&tx_buf[4]);

    memcpy(p, (uint8_t*)&wr_nvds_paras, sizeof(hci_op_par_cmd_t));

    result = start_xfer_wait(fd, tx_buf, sizeof(tx_buf), 1000, 0, resp, &rlen);

    if (result < 0) {
    BT_ERROR("write bdaddr failed:%d", result);
        return -1;
    }

    if (rlen != 7) {
        BT_ERROR("%s: write bdaddr length %u", __func__, rlen);
        return -1;
    }

    return 0;
}

static int rk_bt_write_addr(int fd, const char *bdaddr)
{
    uint8_t tx_buf[] = { 0x01, 0x01, 0xfc, 0x06, 0x01, 0x02,
                0x03, 0x04, 0x05, 0x06 };
    uint8_t resp[7];//min size.
    uint16_t rlen = sizeof(resp);
    uint8_t *p = tx_buf;
    int result;

    if(bdaddr != NULL)
    str_to_bda(bdaddr, &tx_buf[4]);

    result = start_xfer_wait(fd, tx_buf, sizeof(tx_buf), 1000, 0, resp, &rlen);

    if (result < 0) {
    BT_ERROR("write bdaddr failed:%d", result);
        return -1;
    }

    if (rlen != 7) {
        BT_ERROR("%s: write bdaddr length %u", __func__, rlen);
        return -1;
    }

    return 0;
}

static int rk_bt_hci_reset(int fd)
{
    int result;
    uint8_t cmd[4] = { 0x01, 0x03, 0x0c, 0x00};

    BT_ERROR("%s: send hci reset cmd", __func__);

    result = start_xfer_wait(fd, cmd, sizeof(cmd), 1000, 0, NULL, 0);
    if (result < 0) {
        BT_ERROR("%s: Failed to send reset cmd", __func__);
        return -1;
    }

    return 0;
}


int rk_bt_init(int fd, int speed, struct termios *ti,
        const char *bdaddr)
{
    char chip_name[248] = {0};
    char fw_path[PATH_MAX];

    //reset_bluetooth();
    check_loader_type(RK960_LOADER_NAME);
    rk_bt_boot(fd, RK960_LOADER_NAME);
    usleep(200*1000);

    if(get_loader_type() > RK960_D1) {
        rk_bt_change_stage_baudrate(fd, STAGE_UPDATE_LOADER_BAUDRATE, speed, ti);
        rk_bt_write_nvds_addr(fd, bdaddr);
        rk_bt_set_launch_addr(fd, RK960_FW_ROM_BASE);
        rk_bt_read_local_name(fd, chip_name, sizeof(chip_name));

        BT_ERROR("fw fetch failed , path [%s]", fw_path);
        if(rk_bt_fetch_fw(FIRMWARE_DIR, RK960_FW1, fw_path) == -1){
            BT_ERROR("fw fetch failed , path [%s]", fw_path);
        }
        else {
            rk_bt_fw_download(fd, fw_path);
        }
        usleep(RK960_FW1_EXECUTION_TIME*1000);
    }

    rk_bt_change_stage_baudrate(fd, STAGE_UPDATE_LOADER_BAUDRATE, speed, ti);
    if(get_loader_type() > RK960_D1) {
        rk_bt_set_launch_addr(fd, RK960_FW_ROM_BASE);
    }
    rk_bt_read_local_name(fd, chip_name, sizeof(chip_name));

    if(rk_bt_fetch_fw(FIRMWARE_DIR, RK960_FW2, fw_path) == -1){
        BT_ERROR("fw fetch failed , path [%s]", fw_path);
    }
    else{
        rk_bt_fw_download(fd, fw_path);
    }

    usleep(200*1000);
    rk_bt_change_stage_baudrate(fd, STAGE_UPDATE_FW_WORK_BAUDRATE, speed, ti);
    uart_set_hw_fctrl(fd, 1);
    usleep(200*1000);
    if(get_loader_type() <= RK960_D1)
        rk_bt_write_addr(fd, bdaddr);
    rk_bt_hci_reset(fd);

    return 0;
}
