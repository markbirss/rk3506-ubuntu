/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-05-20     mark.huang  the first version
 */

#include "btAP6212Control.h"


#ifdef BT_CHIP_AP6212
#include "btAP6212_init_script.h"

#include <rtthread.h>
#include <rthw.h>
#include <rtdevice.h>

#include "drv_uart.h"
#include "drv_gpio.h"
#include "drv_clock.h"
#include "hal_base.h"
#include "hal_pinctrl.h"
#include "hal_cru.h"

#ifdef RT_USING_FINSH
#include <shell.h>
#endif

#include "uart_api.h"

#include "board.h"

#if defined(RT_USING_BOARD_RK2108B_EVB) || defined(RT_USING_BOARD_RK2108_EVB)
#define BT_GPIO_PORT                (GPIO_BANK1)
#define BT_POWER_GPIO_PIN           GPIO_PIN_A5
#define BT_HOST_RX_PIN              GPIO_PIN_A0
#define BT_HOST_TX_PIN              GPIO_PIN_A1
#define BT_HOST_CTS_PIN             GPIO_PIN_A2
#define BT_HOST_RTS_PIN             GPIO_PIN_A3
#define BT_32K_CLK_OUT              GPIO_PIN_A7

#define GPIO_PORT_BASE              GPIO1
#define GPIO_BANK                   GPIO_BANK1
#define BT6212_UART_NAME       "/dev/uart2"
#endif

#define DEBUG_LOG    "AP6212"


#define Default_baud_rate   115200
#define Default_parity      'n'

/*-----------------------------------------------------------------------------------*/
extern int open_comm(const char *name);
extern void close_comm(int fd);
extern void config_comm(int fd, int speed_baud_rate, char parity, int data_bits, int stop_bits);
extern int recv_comm(int fd, unsigned char *buffer, rt_size_t size, struct timeval *timeout);
extern int send_comm(int fd, const unsigned char *buffer, rt_size_t size);
extern int flush_comm(int fd);
extern HAL_Status HAL_CRU_ClkFracSetFreq(eCLOCK_Name clockName, uint32_t rate);

int AP6212HwReset(void);
int AP6212HwInit(void);
int AP6212HwDownInitScript(void);
int AP6212HwSetDevMac(struct bd_addr *bdaddr);
static uint32 AP6212SetMacAddr(void);
static uint32 AP6212SetDataUart(void);
int AP6212Change_BaudRate(void);


_ap6212Pcb_ ap6212pcb;
static HDC uart_handle;
unsigned char buffer[260];
uint8_t rev_buf[260];
#if 0
static void dump(unsigned char *out, int len)
{
    int i;

    for (i = 0; i < len; i++)
    {
        if (i && !(i % 16))
        {
            rt_kprintf("\n");
        }

        rt_kprintf("%02x ", out[i]);
    }

    rt_kprintf("\n");
}
#endif

int ap6212_init_complete(uint8 result)
{
    osi_sem_give(&ap6212pcb.cmd_complete_sem);
    if (result == 0x00) //HCI_SUCCESS
    {
        return 0;
    }

    return 1;
}

int wait_ap6212_cmd_complete(uint timeoutms)
{
    int tickTime = OSI_MS_TO_TICK;
    if (osi_sem_take(&ap6212pcb.cmd_complete_sem, timeoutms / tickTime) ==  RK_OK)
    {
        return 0;
    }
    return ERR_TIMEOUT;
}

uint32 ap6212_power_up(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK, BT_POWER_GPIO_PIN, PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO_PORT_BASE, BT_POWER_GPIO_PIN, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO_PORT_BASE, BT_POWER_GPIO_PIN, GPIO_LOW);
    rt_thread_delay(20);
    HAL_GPIO_SetPinLevel(GPIO_PORT_BASE, BT_POWER_GPIO_PIN, GPIO_HIGH);
    /*32K CLK OUT CONFIG */
#if defined(RT_USING_BOARD_RK2108B_EVB) || defined(RT_USING_BOARD_RK2108_EVB)
    HAL_PINCTRL_SetIOMUX(GPIO_BANK, BT_32K_CLK_OUT, PIN_CONFIG_MUX_FUNC3);
    HAL_CRU_ClkEnable(CLK_BT32K_GATE);
    HAL_CRU_ClkFracSetFreq(CLK_32K, 32768);
#endif

    RK_LOGD(DEBUG_LOG, "ap6212 POWER On \n");
    return RK_OK;
}

uint32  ap6212_power_down(void)
{
    HAL_GPIO_SetPinLevel(GPIO_PORT_BASE, BT_POWER_GPIO_PIN, GPIO_LOW);
    rt_thread_delay(100);
    HAL_GPIO_SetPinDirection(GPIO_PORT_BASE, BT_POWER_GPIO_PIN, GPIO_IN);
#if defined(RT_USING_BOARD_RK2108B_EVB) || defined(RT_USING_BOARD_RK2108_EVB)
    HAL_PINCTRL_SetIOMUX(GPIO_BANK, BT_32K_CLK_OUT, PIN_CONFIG_MUX_FUNC0);
    HAL_CRU_ClkDisable(CLK_BT32K_GATE);
#endif
    return RK_FAIL;
}

static void bt_hci_uart_hw_init(void)
{
#if defined(RT_USING_BOARD_RK2108B_EVB) || defined(RT_USING_BOARD_RK2108_EVB)
    HAL_PINCTRL_SetIOMUX(GPIO_BANK,
                         BT_HOST_RX_PIN |
                         BT_HOST_TX_PIN |
                         BT_HOST_CTS_PIN |
                         BT_HOST_RTS_PIN,
                         PIN_CONFIG_MUX_FUNC4);
#endif
}

// for temp read hci event when firmware init
uint16_t read_event(int fd, uint8_t *buffer)
{
    int i = 1;
    int len = 1;
    int count;
start:
    count = recv_comm(fd, buffer, len, RT_NULL);
    //RK_LOGD(DEBUG_LOG,"read count = %d \n" ,count);

    switch (buffer[0])
    {
    case 0x02:
        len = 4; //HCI ACL Data Packet 8bits + Handle 12bits + FB Flag 1bit + BC Flag 1 bit + Data Total length 16 bit
        count = recv_comm(fd, &buffer[i], len, RT_NULL);
        len = buffer[4] << 8 | buffer[3];
        break;
    case 0x03:
        len = 3; //HCI Synchronous Data Packet 8bits + Connection_Handle 12bits + Package status Flag 1bit+  RFU 1bit  + Data Total length 8bits
        count = recv_comm(fd, &buffer[i], len, RT_NULL);
        len = buffer[3];
        break;
    case 0x04:
        len = 2; //HCI Event Packet 8bits + Event Code 8 bits + Parameter Total Event 8 bits
        count = recv_comm(fd, &buffer[i], len, RT_NULL);
        len = buffer[2];
        break;
    default:
        RK_LOGD(DEBUG_LOG, "[h4] Unknown HCI message type drop this byte 0x%x", buffer[0]);
        goto start;
    }
    i += count;
    count = i;
    if (len != 0)
    {
        count = recv_comm(fd, &buffer[i], len, RT_NULL);
        count += i;
    }

    //RK_LOGD(DEBUG_LOG,"Recived = %d \n" ,count);
    //dump(buffer, count);
    return count;

}

void ap6212_send_go_to_sleep_ack(void)
{

}

void ap6212_send_wake_up_ind(void)
{

}

void ap6212_send_wake_up_ack(void)
{

}

int AP6212HwPowerOn(void *config)
{
    RK_LOGD(DEBUG_LOG, "AP6212HwPowerOn start");
    int ret = 0;
    int retry = 3;

    POWER_ON_CONFIG *power_on_config = config;


    if ((uart_handle = open_comm(BT6212_UART_NAME)) == -1)
    {
        RK_LOGD(DEBUG_LOG, "Check the device name...\n");
        return -1;
    }

    config_comm(uart_handle, 115200, Default_parity, 8, 1);

    flush_comm(uart_handle);

    /*串口硬件初始化*/
    bt_hci_uart_hw_init();

    if (ap6212_power_up() == RK_FAIL)
    {
        return RK_FAIL;
    }
    rt_thread_delay(300);

    RK_LOGD(DEBUG_LOG, "AP6212HwPowerOn end");
power_up:
    ret = AP6212HwDownInitScript();
    if (ret != 0)
    {
        while (1); //for test
        if (retry--)
        {
            ap6212_power_down();
            rt_thread_delay(10);
            ap6212_power_up();
            goto power_up;
        }
        else
        {
            return ret;
        }
    }

    AP6212Change_BaudRate();
    read_event(uart_handle, rev_buf);
    rt_thread_mdelay(300);

    config_comm(uart_handle, BAUD_RATE_1500000, Default_parity, 8, 1);
    RK_LOGD(DEBUG_LOG, "change baudrate ok --->1500000\n");


    AP6212HwSetDevMac(&power_on_config->bdaddr);

    read_event(uart_handle, rev_buf);
    rt_thread_mdelay(300);
    RK_LOGD(DEBUG_LOG, "set mac OK");

    AP6212SetDataUart();
    read_event(uart_handle, rev_buf);
    rt_thread_mdelay(300);

    RK_LOGD(DEBUG_LOG, "set data uart OK");

    ap6212pcb.PowerStatus = POWER_STATUS_ON;
    return ret;

}

int AP6212HwPowerOff(void)
{
    ap6212_power_down();
    ap6212pcb.PowerStatus = POWER_STATUS_OFF;

    close_comm(uart_handle);
    uart_handle = -1;
    return 0;
}


int AP6212HwReset(void)
{
    /*    hci_reset();*/
    uint8_t hci_reset[] = { 0x01, 0x03, 0x0c, 0x00 };
    send_comm(uart_handle, hci_reset, sizeof(hci_reset));
    return 0;
}


int AP6212HwInit(void)
{
    memset(&ap6212pcb, 0, sizeof(_ap6212Pcb_));
    return 0;

}

int AP6212Change_BaudRate()
{
    BT_HDR *p;
    uint8_t len = 0;
    uint8 patchcode[6] = {0x00, 0x00, 0x60, 0xe3, 0x16, 0x00};

    if ((p = (BT_HDR *)osi_malloc(BT_HDR_SIZE + 10)) == NULL)
    {
        RK_LOGD(DEBUG_LOG, "hci_download_patchcode: Could not allocate memory for BT_HDR\n");
        return ERR_MEM;
    }

    /* Assembling command packet */
    /// Set data struct.
    uint8 packetcode[4] = {0x01, 0x18, 0xfc, 0x06/*length*/};
    memcpy((uint8 *)p->data + 0, packetcode, 4);
    memcpy((uint8 *)p->data + 4, patchcode, 6);
    len = sizeof(packetcode) + sizeof(patchcode) ;
    /* Assembling cmd prameters */
    send_comm(uart_handle, p->data, len);

    osi_free(p);

    return 1;
}

void AP6212Download_patchcode(uint8 *patchcode, int patch_len)
{
    BT_HDR *p;

    if ((p = (BT_HDR *)osi_malloc(BT_HDR_SIZE + patch_len)) == NULL)
    {
        return;
    }

    if (NULL != patchcode)
    {
        memcpy(p->data, patchcode, patch_len);
    }

    send_comm(uart_handle, p->data, patch_len);

    osi_free(p);
}

void AP6212Download_patchcode_cmd()
{
    BT_HDR *p;

    if ((p = (BT_HDR *)osi_malloc(BT_HDR_SIZE + 4)) == NULL)
    {
        RK_LOGD(DEBUG_LOG, "hci_download_patchcode: Could not allocate memory for BT_HDR\n");
        return;
    }

    /* Assembling command packet */
    uint8 packetcode[4] = {0x01, 0x2e, 0xfc, 0/*length*/};
    memcpy((uint8 *)p->data + 0, packetcode, 4);
    send_comm(uart_handle, p->data, sizeof(packetcode));

    osi_free(p);
}

#if 0
static void Set_BDAdress(uint8 *des, uint8 *bd_add, uint8 *cur_len)
{
    int i;
    int len, pos;
    uint16 data_len = 0;

    if (des == NULL || bd_add == NULL)
        return;

    pos = -1;
    len = *cur_len;

    for (i = 0; i < len; i++)
    {
        //config file's signature 0x55AB2387
        if (des[i] == 0x55
                && des[i + 1] == 0xAB
                && des[i + 2] == 0x23
                && des[i + 3] == 0x87
           )
        {
            pos = i;        //config file is in last loadbuf data.
            break;
        }
    }

    if (pos == -1)  //means not found config file start signature in this buffer .
    {
        RK_LOGD(DEBUG_LOG, "Not found config file signature,need to get more data.");
    }
    else    //we get the config file's position in last load buffer data.
    {
        data_len = (des[pos + 4] | des[pos + 5] << 8);

        data_len += 9;
        des[pos + 4] = data_len &  0x00ff;
        des[pos + 5] = data_len >> 8 & 0xff;

        if (len + 9 < 252) //not exceed the patch code maximum data size
        {
            des[len]     = 0x3C;    //offset of BT Efuse.
            des[len + 1] = 0x00;
            des[len + 2] = 0x06;    //length of BDAdrr.
            memcpy(&des[len + 3], bd_add, sizeof(bd_add));

            *cur_len = len + 9;     //add 9 bytes bdaddr data not
        }
    }

}
#endif

int AP6212HwDownInitScript(void)
{
    int offset = 0;

    uint32 loadAddr;
    uint8 iCurLen = 0;
    uint32 patchcodeLen;

    loadAddr = (uint32)bt_init_firmware;
    patchcodeLen = sizeof(bt_init_firmware);
    RK_LOGD(DEBUG_LOG, "loadAddr = 0x%04x, patchcodeLen = %d", loadAddr, patchcodeLen);

    AP6212HwReset();
    read_event(uart_handle, rev_buf);
    rt_thread_mdelay(300);
    RK_LOGD(DEBUG_LOG, "hci reset ok \n");

    AP6212Change_BaudRate();
    read_event(uart_handle, rev_buf);
    rt_thread_mdelay(300);

    config_comm(uart_handle, BAUD_RATE_1500000, Default_parity, 8, 1);
    RK_LOGD(DEBUG_LOG, "change baudrate ok --->1500000\n");

    rt_thread_delay(300);
    RK_LOGD(DEBUG_LOG, "change baud ok\n");


    AP6212Download_patchcode_cmd();
    read_event(uart_handle, rev_buf);
    rt_thread_delay(300);

    RK_LOGD(DEBUG_LOG, "download patch code command ok\n");

    while ((offset + 3 < patchcodeLen))
    {
        memcpy(&buffer[1], (uint8 *)(loadAddr + offset), 3);
        offset += 3;
        buffer[0] = 0x01;
        iCurLen = buffer[3];
        memcpy(&buffer[4], (uint8 *)(loadAddr + offset), iCurLen);
        AP6212Download_patchcode(buffer, iCurLen + 4);
        read_event(uart_handle, rev_buf);
        rt_thread_delay(10);

        offset += iCurLen;
    }

    RK_LOGD(DEBUG_LOG, "down script complete\n");
    rt_thread_delay(200);

    config_comm(uart_handle, BAUD_RATE_115200, Default_parity, 8, 1);
    rt_thread_mdelay(300);
    RK_LOGD(DEBUG_LOG, "change baudrate ok  --->115200 \n");

    AP6212HwReset();
    read_event(uart_handle, rev_buf);
    rt_thread_mdelay(300);

    return 0;
}

int AP6212HwWakeup(void *config)
{

    ap6212_send_wake_up_ind();
    return 0;
}

int AP6212HwGetPowerStatus(void)
{
    return ap6212pcb.PowerStatus;
}


uint32 AP6212HwSetChipBaudrate(uint baudrate)
{
    return 0;
}

static uint32 AP6212SetMacAddr(void)
{
    BT_HDR *p;

    if ((p = (BT_HDR *)osi_malloc(BT_HDR_SIZE + 10)) == NULL)
    {
        RK_LOGD(DEBUG_LOG, "hci_download_patchcode: Could not allocate memory for BT_HDR\n");
        return RK_FAIL;
    }

    /* Assembling command packet */
    uint8 packetcode[4] = {0x01, 0x01, 0xfc, 0x06/*length*/};
    memcpy((uint8 *)p->data + 0, packetcode, 4);

    memcpy(((uint8 *)p->data) + 4, ap6212pcb.bdaddr.addr, 6);

    send_comm(uart_handle, p->data, 10);

    osi_free(p);
    return 0;
}

static uint32 AP6212SetDataUart(void)
{
    BT_HDR *p;

    if ((p = (BT_HDR *)osi_malloc(BT_HDR_SIZE + 9)) == NULL)
    {
        RK_LOGD(DEBUG_LOG, "AP6212SetDataUart: Could not allocate memory for BT_HDR\n");
        return RK_FAIL;
    }

    /* Assembling command packet
    p->data[0] = 0x01;
    p->data[1] = 0x1c;
    p->data[2] = 0xfc;
    p->data[3] = 0x05;
    p->data[4] = 0x01;
    p->data[5] = 0x00;
    p->data[6] = 0x00;
    p->data[7] = 0x00;
    p->data[8] = 0x00;
    */
    rt_uint8_t hci_write_sco_pcm_int[] =
    { 0x01, 0x1C, 0xFC, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00 };
    hci_write_sco_pcm_int[4] = 0x01;
    send_comm(uart_handle, hci_write_sco_pcm_int, sizeof(hci_write_sco_pcm_int));


    // uint8 packetcode[4] = {0x01, 0x1c, 0xfc, 0x05/*length*/};
    // memcpy((uint8 *)p->data + 0, packetcode, 4);

    // //memcpy(((uint8 *)p->data) + 4, ap6212pcb.bdaddr.addr, 6);
    // send_comm(uart_handle, p->data, strlen(p->data));

    osi_free(p);
    return 0;
}


int AP6212HwSetDevMac(struct bd_addr *bdaddr)
{

    ap6212pcb.needChangeMac = TRUE;
    memcpy(ap6212pcb.bdaddr.addr, bdaddr->addr, 6);
    AP6212SetMacAddr();
    return 0;
}

uint32 AP6212HwRegisterPowerNotification(int(*func)(POWER_NOTIFICATION_t event))
{
    ap6212pcb.power_event_notify = func;
    return 0;
}


uint32 AP6212HwRegisterHwError(int(*func)(POWER_NOTIFICATION_t event))
{
    return 0;
}


uint32 AP6212HwVendorRecv(uint8 *data, uint len)
{
    uint8 c;

    c = *data;

    switch (c)
    {
    default:
        break;
    }
    return 0;
}


int AP6212HciDataRead(char *data, uint32 len)
{
    /* RX */
    //RK_LOGD(DEBUG_LOG,"host_recv_packet now!\n");
    return recv_comm(uart_handle, (unsigned char *)data, len, RT_NULL);
}

int AP6212HciDataWrite(char *data, uint32 len)
{
    /* TX */
    //RK_LOGD(DEBUG_LOG,"host_send_packet now!\n");
    send_comm(uart_handle, (unsigned char *)data, len);
    return 0;
}

/******************************************************************************
**  HCI H4 Services interface table
******************************************************************************/

const bt_hw_control_t ap6212_hw_control =
{
    AP6212HwInit,
    AP6212HwPowerOn,
    AP6212HwPowerOff,
    NULL,
    NULL,
    NULL,
    NULL,
    AP6212HwGetPowerStatus,
    NULL,
    NULL,
    NULL,
    AP6212HwSetDevMac,
    NULL,
    NULL,
    AP6212HciDataWrite,
    AP6212HciDataRead,
};

#endif    //(BT_CHIP_CONFIG == BT_CHIP_AP6212)


