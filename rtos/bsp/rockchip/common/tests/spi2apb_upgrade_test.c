/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <rtdevice.h>
#include <rtthread.h>

#ifdef RT_USING_COMMON_TEST_SPI2APB_UPGRADE
#include <dfs_file.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <drivers/spi.h>

#include "spi_device.h"

#define USE_UPDATE_IMG
#define UPDATE_IMG_HEADER_LEN 512

#define SPIDEV_RKSPI2APB_OP_BYTES           512

#define SPIDEV_RKSPI2APB_STATE_UNKNOW           0x0
#define SPIDEV_RKSPI2APB_STATE_MASKROM          0x0FF0AA55
#define SPIDEV_RKSPI2APB_STATE_PROGRAMMER       0x50524F47

#define SPI2APB_ERR_LOG(fmt, args...)     \
    do                                  \
    {                                   \
        rt_kprintf(fmt, ## args);       \
    }                                   \
    while(0)
#define SPI2APB_INF_LOG(fmt, args...)     \
    do                                  \
    {                                   \
        rt_kprintf(fmt, ## args);       \
    }                                   \
    while(0)
#if 0
#define SPI2APB_DBG_LOG(fmt, args...)     \
    do                                  \
    {                                   \
        rt_kprintf(fmt, ## args);       \
    }                                   \
    while(0)
#else
#define SPI2APB_DBG_LOG(fmt, args...)  do { } while (0)
#endif

#define BOOT_FLAG_CRC               (1<<0)
#define BOOT_FLAG_RUN               (1<<1)
#define BOOT_FLAG_ACK               (1<<2)

#define CMD_ST_OK                   0
#define CMD_ST_BUSY                 (1<<0)//BIT(0)
#define CMD_ST_ERROR                (1<<3)//BIT(3)
#define CMD_ST_PARAM_ERR            (1<<4)//BIT(4)
#define CMD_ST_CRC_ERR              (1<<5)//BIT(5)
#define CMD_ST_SECURE_ERR           (1<<6)//BIT(6)

#define SPI_LBA_READ_10         0x14
#define SPI_LBA_WRITE_10        0x15
#define SPI_ERASE_DATA          0x25
#define SPI_READ_FLASH_INFO     0x1A
#define SPI_FW_RESET            0xFF

#define IMEM_BASE_ADDR          0x00200000  /* 1MB */
#define SRAM_XFER_ADDR          (IMEM_BASE_ADDR + 256*1024)

#define SPI_REQ_ADDR            (IMEM_BASE_ADDR + 1024*1024 - 64)
#define SPI_0471_ADDR           (IMEM_BASE_ADDR + 4*1024)
#define SPI_STATUS1_ADDR        (SPI_REQ_ADDR + 16)
#define SPI_STATUS2_ADDR        (SPI_REQ_ADDR + 28)
#define SPI_XFER_ADDR1          (IMEM_BASE_ADDR + 64*1024)

#define TO_NS_ADDR(a)           (0x30000000 + a)
#define TO_SS_ADDR(a)           (0x0FFFFFFF & a)

#define APB_CMD_WRITE           0x00000011
#define APB_CMD_WRITE_REG0      0X00010011
#define APB_CMD_WRITE_REG1      0X00020011
#define APB_CMD_READ            0x00000077
#define APB_CMD_READ_BEGIN      0x000000AA
#define APB_CMD_QUERY           0x000000FF
#define APB_CMD_QUERY_REG2      0x000001FF

#define APB_OP_STATE_ID_MASK            0xffff0000
#define APB_OP_STATE_ID             0X16080000

#define APB_OP_STATE_MASK           0x0000ffff
#define APB_OP_STATE_WRITE_ERROR        (0x01 << 0)
#define APB_OP_STATE_WRITE_OVERFLOW     (0x01 << 1)
#define APB_OP_STATE_WRITE_UNFINISHED       (0x01 << 2)
#define APB_OP_STATE_READ_ERROR         (0x01 << 8)
#define APB_OP_STATE_READ_UNDERFLOW     (0x01 << 9)
#define APB_OP_STATE_PRE_READ_ERROR     (0x01 << 10)

#define APB_SAFE_OPERATION_TRY_MAX      3
#define APB_SAFE_OPERATION_TRY_DELAY_US     100

#define SPI_BUFSIZ  max(32, SMP_CACHE_BYTES)
#define WAIT_READY_TIMEOUT_MAX_COUNT 1000

typedef struct
{
    rt_uint32_t flag;
    rt_uint32_t load_addr;
    rt_uint32_t boot_len;         //Byte  Unit
    rt_uint32_t hash;
    rt_uint16_t status;
    rt_uint16_t cmd;
} boot_request_t;

typedef struct
{
    rt_uint16_t tag;
    rt_uint8_t version;
    rt_uint8_t head_size;
    rt_uint32_t head_hash;      /*hash for head,check by bootrom.*/
    rt_uint16_t flag;
    rt_uint8_t reseved0;
    rt_uint8_t lun;
    rt_uint32_t lba;
    rt_uint16_t nsec;
    rt_uint16_t reseved1;
    rt_uint32_t buf_addr;
    rt_uint32_t parity;
    rt_uint16_t status;
    rt_uint16_t cmd;
} spi_request_t;

typedef struct
{
    rt_uint8_t name[32];
    rt_uint32_t offset;
    rt_uint32_t size;
    rt_uint32_t reserve[2];
} spi2apb_fw_info;

typedef struct
{
    rt_uint8_t magic[4];
    rt_uint32_t jshash;
    rt_uint32_t jshash_enable;
    rt_uint32_t reserve[1];
    spi2apb_fw_info fw_info[2];
} spi2apb_img_header;

typedef enum
{
    SPI2APB_UPG_INIT = 0,
    SPI2APB_UPG_DOWN_BIN,
    SPI2APB_UPG_DOWN_BIN_SEND_REQ,
    SPI2APB_UPG_DOWN_BIN_WAIT_READY,
    SPI2APB_UPG_DOWN_IMG,
    SPI2APB_UPG_DOWN_IMG_SEND_REQ,
    SPI2APB_UPG_DOWN_IMG_WAIT_READY,
    SPI2APB_UPG_REBOOT,
} spi2apb_upgrade_state;

rt_int32_t spi_write_boot(rt_uint32_t addr, rt_uint32_t nsec, rt_uint8_t *data_buf, rt_uint32_t flag);
rt_int32_t spi_read_boot(rt_uint32_t addr, rt_uint32_t nsec, rt_uint8_t *data_buf, rt_uint32_t flag);

static struct rt_spi_device *spidev;
static rt_uint8_t *read_buf;
static rt_uint8_t *write_buf;
static char bin_name[512];
static char img_name[512];

#ifdef USE_UPDATE_IMG
static rt_int32_t firmware_offset = 0;
static spi2apb_img_header img_header;
spi2apb_upgrade_state upgrade_state;
static rt_int32_t flash_src_addr;
static rt_int32_t total_writed;
static rt_int32_t data_residue;
static rt_uint32_t calc_jshash;
static rt_int32_t wait_timeout;
#endif

#ifdef USE_UPDATE_IMG
rt_uint32_t JS_Hash(rt_uint8_t *buf, rt_int32_t len)
{
    rt_int32_t i;

    for (i = 0; i < len; ++i)
    {
        calc_jshash ^= ((calc_jshash << 5) + buf[i] + (calc_jshash >> 2));
    }
    return calc_jshash;
}
#endif

rt_uint32_t spi_read_status2(void)
{
    rt_uint32_t status = 0;
    struct rt_spi_message *ret;
    struct rt_spi_message cmd_msg;
    struct rt_spi_message state_msg;
    rt_uint32_t query_cmd = APB_CMD_QUERY_REG2;

    cmd_msg.send_buf   = &query_cmd;
    cmd_msg.recv_buf   = RT_NULL;
    cmd_msg.length     = 4;
    cmd_msg.cs_take    = 1;
    cmd_msg.cs_release = 0;
    cmd_msg.next       = &state_msg;

    state_msg.send_buf   = RT_NULL;
    state_msg.recv_buf   = &status;
    state_msg.length     = 4;
    state_msg.cs_take    = 0;
    state_msg.cs_release = 1;
    state_msg.next       = RT_NULL;

    ret = rt_spi_transfer_message(spidev, &cmd_msg);
    if (ret != RT_NULL)
    {
        SPI2APB_ERR_LOG("%s: failed\n", __func__);
        return 1;
    }

    return status;
}

static rt_int32_t _spi2apb_write(rt_int32_t addr, const char *data, rt_int32_t data_len)
{
    struct rt_spi_message *ret;
    struct rt_spi_message cmd_msg;
    struct rt_spi_message addr_msg;
    struct rt_spi_message data_msg;
    rt_int32_t write_cmd = APB_CMD_WRITE;

    cmd_msg.send_buf   = &write_cmd;
    cmd_msg.recv_buf   = RT_NULL;
    cmd_msg.length     = 4;
    cmd_msg.cs_take    = 1;
    cmd_msg.cs_release = 0;
    cmd_msg.next       = &addr_msg;

    addr_msg.send_buf   = &addr;
    addr_msg.recv_buf   = RT_NULL;
    addr_msg.length     = 4;
    addr_msg.cs_take    = 0;
    addr_msg.cs_release = 0;
    addr_msg.next       = &data_msg;

    data_msg.send_buf   = data;
    data_msg.recv_buf   = RT_NULL;
    data_msg.length     = data_len;
    data_msg.cs_take    = 0;
    data_msg.cs_release = 1;
    data_msg.next       = RT_NULL;

    ret = rt_spi_transfer_message(spidev, &cmd_msg);
    if (ret != RT_NULL)
    {
        SPI2APB_ERR_LOG("%s: failed\n", __func__);
        return -1;
    }

    //SPI2APB_DBG_LOG("%s: success\n", __func__);
    return 0;
}

/*static rt_int32_t _spi2apb_read(rt_int32_t addr, char *data, rt_int32_t data_len)
{
    struct rt_spi_message *ret;
    struct rt_spi_message cmd_msg;
    struct rt_spi_message addr_msg;
    struct rt_spi_message dummy_msg;
    struct rt_spi_message begin_msg;
    struct rt_spi_message data_msg;
    rt_int32_t real_len = data_len;//data_len < SPIDEV_RKSPI2APB_OP_BYTES ? data_len : SPIDEV_RKSPI2APB_OP_BYTES;
    rt_int32_t read_cmd = APB_CMD_READ | (real_len << 14 & 0xffff0000);
    rt_int32_t read_begin_cmd = APB_CMD_READ_BEGIN;
    rt_int32_t dummy = 0;

    cmd_msg.send_buf   = &read_cmd;
    cmd_msg.recv_buf   = RT_NULL;
    cmd_msg.length     = 4;
    cmd_msg.cs_take    = 1;
    cmd_msg.cs_release = 0;
    cmd_msg.next       = &addr_msg;

    addr_msg.send_buf   = &addr;
    addr_msg.recv_buf   = RT_NULL;
    addr_msg.length     = 4;
    addr_msg.cs_take    = 0;
    addr_msg.cs_release = 0;
    addr_msg.next       = &dummy_msg;

    dummy_msg.send_buf   = &dummy;
    dummy_msg.recv_buf   = RT_NULL;
    dummy_msg.length     = 4;
    dummy_msg.cs_take    = 0;
    dummy_msg.cs_release = 0;
    dummy_msg.next       = &begin_msg;

    begin_msg.send_buf   = &read_begin_cmd;
    begin_msg.recv_buf   = RT_NULL;
    begin_msg.length     = 4;
    begin_msg.cs_take    = 0;
    begin_msg.cs_release = 0;
    begin_msg.next       = &data_msg;

    data_msg.send_buf   = RT_NULL;
    data_msg.recv_buf   = data;
    data_msg.length     = data_len;
    data_msg.cs_take    = 0;
    data_msg.cs_release = 1;
    data_msg.next       = RT_NULL;

    ret = rt_spi_transfer_message(spidev, &cmd_msg);
    if (ret != RT_NULL)
    {
        SPI2APB_ERR_LOG("%s: failed\n", __func__);
        return -1;
    }

    return 0;
}*/

/*static rt_int32_t _spi2apb_operation_query(rt_int32_t *state)
{
    struct rt_spi_message *ret;
    struct rt_spi_message cmd_msg;
    struct rt_spi_message state_msg;
    rt_uint32_t query_cmd = APB_CMD_QUERY;

    cmd_msg.send_buf   = &query_cmd;
    cmd_msg.recv_buf   = RT_NULL;
    cmd_msg.length     = 4;
    cmd_msg.cs_take    = 1;
    cmd_msg.cs_release = 0;
    cmd_msg.next       = &state_msg;

    state_msg.send_buf   = RT_NULL;
    state_msg.recv_buf   = state;
    state_msg.length     = 4;
    state_msg.cs_take    = 0;
    state_msg.cs_release = 1;
    state_msg.next       = RT_NULL;

    ret = rt_spi_transfer_message(spidev, &cmd_msg);
    if (ret != RT_NULL)
    {
        SPI2APB_ERR_LOG("%s: failed\n", __func__);
        return -1;
    }

    //SPI2APB_DBG_LOG("%s: success state %x\n", __func__, *state);
    return ((*state & APB_OP_STATE_ID_MASK) == APB_OP_STATE_ID) ? 0 : -1;
}*/

/*static rt_int32_t _spi2apb_safe_write(rt_int32_t addr, const char *data, rt_int32_t data_len)
{
    rt_int32_t state = 0;
    rt_int32_t try = 0;

    do
    {
        rt_int32_t ret = 0;

        ret = _spi2apb_write(addr, data, data_len);
        if (ret == 0)
            ret = _spi2apb_operation_query(&state);
        if (ret != 0)
            return ret;
        else if ((state & APB_OP_STATE_MASK) == 0)
            break;

        if (try++ == APB_SAFE_OPERATION_TRY_MAX)
                break;
        //udelay(APB_SAFE_OPERATION_TRY_DELAY_US);
        rt_thread_delay(RT_TICK_PER_SECOND / 10000);
    }
    while (1);

    return (state & APB_OP_STATE_MASK);
}*/

/*static rt_int32_t _spi2apb_safe_read(rt_int32_t addr, char *data, rt_int32_t data_len)
{
    rt_int32_t state = 0;
    rt_int32_t try = 0;

    do
    {
        rt_int32_t ret = 0;

        ret = _spi2apb_read(addr, data, data_len);
        if (ret == 0)
            ret = _spi2apb_operation_query(&state);
        if (ret != 0)
            return ret;
        else if ((state & APB_OP_STATE_MASK) == 0)
            break;

        if (try++ == APB_SAFE_OPERATION_TRY_MAX)
                break;
        //udelay(APB_SAFE_OPERATION_TRY_DELAY_US);
        rt_thread_delay(RT_TICK_PER_SECOND / 10000);
    }
    while (1);

    return (state & APB_OP_STATE_MASK);
}*/

/*rt_int32_t spi_read_io(rt_uint32_t addr, void *data, rt_uint32_t size, rt_uint32_t flag)
{
    rt_int32_t ret;

    if (size > SPIDEV_RKSPI2APB_OP_BYTES)
        return -1;

    if (size > 4)
        SPI2APB_DBG_LOG("%s: addr 0x%08x size %d\n", __func__, addr, size);

    addr = TO_NS_ADDR(addr);
    ret = _spi2apb_safe_read(addr, data, size);
    if (ret != 0)
    {
        SPI2APB_ERR_LOG("%s: failed\n", __func__);
    }

    return ret;
}*/
rt_int32_t spi_write_io(rt_uint32_t addr, void *data, rt_uint32_t size)
{
    rt_int32_t ret;

    if (size > SPIDEV_RKSPI2APB_OP_BYTES)
        return -1;

    SPI2APB_DBG_LOG("%s: addr 0x%08x size %d\n", __func__, addr, size);

    addr = TO_NS_ADDR(addr);
    ret = _spi2apb_write(addr, data, size);
    if (ret != 0)
    {
        SPI2APB_ERR_LOG("%s: failed\n", __func__);
    }

    return ret;
}

/*rt_int32_t wait_request_done(rt_uint32_t status_addr)
{
    rt_int32_t timeout;
    rt_int32_t ret = -1;
    rt_uint32_t status = CMD_ST_BUSY;

    SPI2APB_DBG_LOG("%s: status_addr %x\n", __func__, status_addr);

    timeout = 50000;    // 5s
    do
    {
        ret = spi_read_io(status_addr, (void *)&status, 4, 1);
        if (ret < 0)
        {
            SPI2APB_ERR_LOG("spi_read_io fail! addr:0x%x\n", status_addr);
            break;
        }

        if (!(CMD_ST_BUSY & status))
        {
            ret = status & 0xFFFF;
            break;
        }

        //usleep(100);
        rt_thread_delay(RT_TICK_PER_SECOND / 10000);
        if (timeout-- <= 0)
        {
            ret = -2;
            break;
        }
    }
    while (1);

    if (ret)
        SPI2APB_ERR_LOG("wait req fail:%d\n", ret);

    return ret;
}*/

rt_int32_t spi_send_request(rt_uint16_t cmd, rt_uint32_t load_addr, rt_uint32_t boot_len, void *data, rt_uint32_t flag)
{
    rt_int32_t ret = -1;
    boot_request_t req;

    req.flag = flag;//BOOT_FLAG_CRC | BOOT_FLAG_RUN;
    req.load_addr = load_addr;
    req.boot_len = boot_len;
    req.hash = 0;//js_hash((uint8*)data, boot_len);
    req.status = CMD_ST_BUSY;
    req.cmd = cmd;

    SPI2APB_DBG_LOG("%s: cmd 0x%x addr:0x%x len %d flag %d\n", __func__, cmd, load_addr, boot_len, flag);

    ret = spi_write_io(SPI_REQ_ADDR, (void *)&req, sizeof(boot_request_t));
    if (ret < 0)
    {
        SPI2APB_ERR_LOG("spi_write_io fail addr:0x%x\n", SPI_REQ_ADDR);
        goto end;
    }

    //ret = wait_request_done(SPI_STATUS1_ADDR);
end:
    return ret;
}

/*rt_int32_t spi_chk_write_data(rt_uint32_t load_addr, rt_uint8_t *pData, rt_uint32_t boot_len)
{
    rt_int32_t ret = 0;
    rt_uint32_t i, len;

    SPI2APB_DBG_LOG("%s: addr:0x%x, len:%d\n", __func__, load_addr, boot_len);

    while (boot_len)
    {

        len = boot_len < SPIDEV_RKSPI2APB_OP_BYTES ? boot_len : SPIDEV_RKSPI2APB_OP_BYTES;
        ret = spi_read_io(load_addr, (void *)read_buf, len, 1);
        if (ret < 0)
            return ret;

        for (i = 0; i < len; i++)
        {
            if (pData[i] != read_buf[i])
            {
                SPI2APB_ERR_LOG("io error:0x%x, 0x%x, %d\n", pData[i], read_buf[i], i);
                return -2;
            }
        }
        boot_len -= len;
        load_addr += len;
        pData += len;
    }

    return 0;
}*/

/*
 * dwonload rk2118_upgrade.bin
 */
rt_int32_t spi_down_bin(rt_uint16_t cmd, rt_uint32_t load_addr, rt_int32_t fd)
{
    rt_int32_t ret = 0;
    rt_int32_t readed = 0;
    rt_uint32_t src_addr = load_addr;
    rt_uint32_t total = 0;

    SPI2APB_INF_LOG("%s: cmd:0x%x, addr:0x%x\n", __func__, cmd, load_addr);

    while (1)
    {
        readed = read(fd, write_buf, SPIDEV_RKSPI2APB_OP_BYTES);
        if (readed == 0)
        {
            SPI2APB_INF_LOG("%s: read file complete\n", __func__);
            break;
        }

        SPI2APB_DBG_LOG("%s: readed %d\n", __func__, readed);

        ret = spi_write_io(src_addr, (void *)write_buf, readed);
        if (ret != 0)
        {
            SPI2APB_ERR_LOG("spi_write_io fail!, addr:0x%x\n", src_addr);
            goto end;
        }

        //memset(read_buf, 0, SPIDEV_RKSPI2APB_OP_BYTES);
        /*ret = spi_read_io(src_addr, (void *)read_buf, readed, 1);
        if (ret != 0)
        {
            SPI2APB_ERR_LOG("spi_read_io fail!, addr:0x%x\n", src_addr);
            goto end;
        }

        if (memcmp(write_buf, read_buf, readed) != 0)
        {
            SPI2APB_ERR_LOG("write %02x %02x %02x %02x\n", write_buf[0], write_buf[1], write_buf[2], write_buf[3]);
            SPI2APB_ERR_LOG("read  %02x %02x %02x %02x\n", read_buf[0], read_buf[1], read_buf[2], read_buf[3]);
            SPI2APB_ERR_LOG("memcmp fail!, addr:0x%x\n", src_addr);
            ret = -1;
            goto end;
        }*/

        src_addr += readed;
        total += readed;
        readed = 0;

        SPI2APB_INF_LOG("downloading %s %d\n", bin_name, total);
    }

    ret = spi_send_request(cmd, load_addr, total, (void *)NULL, BOOT_FLAG_CRC | BOOT_FLAG_RUN);
    if (ret < 0)
    {
        SPI2APB_ERR_LOG("send request fail!:0x%x\n", ret);
        goto end;
    }

end:
    return ret;
}

/*
 * dwonload Firmware.img
 */
rt_int32_t spi_down_img(rt_uint16_t cmd, rt_uint32_t load_addr, rt_int32_t fd)
{
    rt_int32_t ret = 0;
    rt_uint32_t sec_addr = load_addr;
    rt_uint32_t nsec = SPIDEV_RKSPI2APB_OP_BYTES << 9;
    rt_int32_t readed = 0;
    rt_uint32_t total = 0;

    while (1)
    {
        readed = read(fd, write_buf, SPIDEV_RKSPI2APB_OP_BYTES);
        if (readed == 0)
        {
            SPI2APB_INF_LOG("%s: read file complete\n", __func__);
            ret = 0;
            break;
        }

        SPI2APB_DBG_LOG("%s: readed %d\n", __func__, readed);

        if ((readed % 512) != 0)
        {
            SPI2APB_ERR_LOG("%s readed not 512 align %d\n", __func__, readed);
            ret = -1;
            goto end;
        }

        nsec = readed >> 9;
        ret = spi_write_boot(sec_addr, nsec, write_buf, 1);
        if (0 != ret)
        {
            SPI2APB_ERR_LOG("spi_write_boot fail!:0x%x\n", ret);
            goto end;
        }

        /*ret = spi_read_boot(sec_addr, nsec, read_buf, 0);
        if (0 != ret)
        {
            SPI2APB_ERR_LOG("spi_read_boot fail!:0x%x\n", ret);
            goto end;
        }

        if (0 != memcmp(write_buf, read_buf, nsec << 9))
        {
            SPI2APB_ERR_LOG("memcmp2 fail!\n");
            ret = -1;
            goto end;
        }*/

        sec_addr += nsec;
        total += readed;
        readed = 0;

        SPI2APB_INF_LOG("downloading %s %d\n", img_name, total);
    }

end:
    return ret;
}

rt_int32_t wait_boot_ready(rt_uint32_t flag)
{
    rt_int32_t timeout;
    rt_uint32_t status = 0;

    SPI2APB_DBG_LOG("%s: 0x%x\n", __func__, flag);

    timeout = 10000;   //10s
    do
    {
        status = spi_read_status2();
        if ((status & 0xFFFF) == flag)
        {
            SPI2APB_INF_LOG("boot ready!:0x%x\n", status);
            return 0;
        }

        //SPI2APB_INF_LOG("status:0x%x\n",status);
        //usleep(100*1000);
        rt_thread_delay(RT_TICK_PER_SECOND / 1000);

    }
    while (timeout--);  //while (status != 0x0FF0AA55);

    SPI2APB_ERR_LOG("wait rk2118 boot timeout!:0x%x\n", status);
    return -1;
}

rt_uint32_t spi_calc_hash(rt_uint32_t *data_buf, rt_uint32_t size)
{
    rt_uint32_t i;
    rt_uint32_t parity = 0x000055aa;

    size = size >> 2;
    for (i = 0; i < size; i++)
        parity += data_buf[i];

    return parity;
}

/*rt_int32_t spi_read_boot(rt_uint32_t addr, rt_uint32_t nsec, rt_uint8_t *data_buf, rt_uint32_t flag)
{
    int ret = -1;
    spi_request_t req;

    SPI2APB_DBG_LOG("%s: addr:0x%x, nsec:%d\n", __func__, addr, nsec);

    memset((void *)&req, 0, sizeof(spi_request_t));
    req.tag = 0x4B52;
    req.version = 0;
    req.head_size = sizeof(spi_request_t);

    req.buf_addr = SPI_XFER_ADDR1;
    req.lba = addr;
    req.nsec = nsec;
    req.status = CMD_ST_BUSY;
    req.cmd = SPI_LBA_READ_10;

    ret = spi_write_io(SPI_REQ_ADDR, (void *)&req, sizeof(spi_request_t));
    if (ret < 0)
    {
        SPI2APB_ERR_LOG("spi_write_io fail addr:0x%x\n", SPI_REQ_ADDR);
        goto end;
    }

    ret = wait_request_done(SPI_STATUS2_ADDR);
    if (ret != 0)
    {
        SPI2APB_ERR_LOG("send req fail!:0x%x\n", ret);
        goto end;
    }

    ret = spi_read_io(SPI_XFER_ADDR1, (void *)data_buf, nsec << 9, 1);
    if (ret < 0)
    {
        SPI2APB_ERR_LOG("spi_read_io fail! addr:0x%x\n", SPI_XFER_ADDR1);
        goto end;
    }

end:
    return ret;
}*/

rt_int32_t spi_write_boot(rt_uint32_t addr, rt_uint32_t nsec, rt_uint8_t *data_buf, rt_uint32_t flag)
{
    rt_int32_t ret = -1;
    spi_request_t req;

    SPI2APB_DBG_LOG("%s: addr:0x%x, nsec:%d\n", __func__, addr, nsec);

    ret = spi_write_io(SPI_XFER_ADDR1, (void *)data_buf, nsec << 9);
    if (ret < 0)
    {
        SPI2APB_ERR_LOG("spi_write_io fail addr:0x%x\n", SPI_XFER_ADDR1);
        goto end;
    }

    memset((void *)&req, 0, sizeof(spi_request_t));
    req.tag = 0x4B52;
    req.version = 0;
    req.head_size = sizeof(spi_request_t);

    req.buf_addr = SPI_XFER_ADDR1;
    req.lba = addr;
    req.nsec = nsec;
    req.status = CMD_ST_BUSY;
    req.cmd = SPI_LBA_WRITE_10;

    if (flag & 0x01)
    {
        req.flag |= BOOT_FLAG_CRC;
        req.parity = spi_calc_hash((rt_uint32_t *)data_buf, nsec << 9);
    }

    ret = spi_write_io(SPI_REQ_ADDR, (void *)&req, sizeof(spi_request_t));
    if (ret < 0)
    {
        SPI2APB_ERR_LOG("spi_write_io fail addr:0x%x\n", SPI_REQ_ADDR);
        goto end;
    }

#if 0
    ret = wait_request_done(SPI_STATUS2_ADDR);
#else
    ret = wait_boot_ready(0x0000);
#endif
end:
    return ret;
}

/*
rt_int32_t spi_erase_boot(rt_uint32_t sec_addr, rt_uint32_t nsec)
{
    rt_int32_t ret = -1;
    spi_request_t req;

    SPI2APB_DBG_LOG("EraseBoot addr:0x%x, nsec:%d\n", sec_addr, nsec);

    memset((void *)&req, 0, sizeof(spi_request_t));
    req.tag = 0x4B52;
    req.version = 0;
    req.head_size = sizeof(spi_request_t);

    req.lba = sec_addr;
    req.nsec = nsec;
    req.status = CMD_ST_BUSY;
    req.cmd = SPI_ERASE_DATA;

    ret = spi_write_io(SPI_REQ_ADDR, (void *)&req, sizeof(spi_request_t));
    if (ret < 0)
    {
        SPI2APB_ERR_LOG("spi_write_io fail addr:0x%x\n", SPI_REQ_ADDR);
        goto end;
    }

    ret = wait_request_done(SPI_STATUS2_ADDR);
end:
    return ret;
}

rt_int32_t spi_get_flash_size(rt_uint32_t *capability)
{
    rt_int32_t ret = -1;
    spi_request_t req;
    rt_uint32_t flash_size = 0;

    if (!capability)
        return -1;

    memset((void *)&req, 0, sizeof(spi_request_t));
    req.tag = 0x4B52;
    req.version = 0;
    req.head_size = sizeof(spi_request_t);

    req.buf_addr = SPI_XFER_ADDR1;
    req.status = CMD_ST_BUSY;
    req.cmd = SPI_READ_FLASH_INFO;

    ret = spi_write_io(SPI_REQ_ADDR, (void *)&req, sizeof(spi_request_t));
    if (ret < 0)
    {
        SPI2APB_ERR_LOG("spi_write_io fail addr:0x%x\n", SPI_REQ_ADDR);
        goto end;
    }

    ret = wait_request_done(SPI_STATUS2_ADDR);
    if (ret < 0)
    {
        SPI2APB_ERR_LOG("waitReqest fail addr:0x%x\n", SPI_STATUS2_ADDR);
        goto end;
    }

    ret = spi_read_io(SPI_XFER_ADDR1, (void *)&flash_size, 4, 1);
    if (ret < 0)
    {
        SPI2APB_ERR_LOG("spi_read_io fail addr:0x%x\n", SPI_XFER_ADDR1);
        goto end;
    }

    *capability = flash_size;
end:
    return ret;
}*/

rt_int32_t spi_reset_boot(void)
{
    rt_int32_t ret = -1;
    spi_request_t req;

    SPI2APB_INF_LOG("spi_reset_boot...\n");

    memset((void *)&req, 0, sizeof(spi_request_t));
    req.tag = 0x4B52;
    req.version = 0;
    req.head_size = sizeof(spi_request_t);

    req.status = CMD_ST_BUSY;
    req.cmd = SPI_FW_RESET;

    ret = spi_write_io(SPI_REQ_ADDR, (void *)&req, sizeof(spi_request_t));
    if (ret < 0)
    {
        SPI2APB_ERR_LOG("spi_write_io fail addr:0x%x\n", SPI_REQ_ADDR);
        goto end;
    }
end:
    return ret;
}

rt_int32_t spi_upgrade_init(void)
{
    struct rt_spi_configuration cfg;
    spidev = (struct rt_spi_device *)rt_device_find("spi1_0");

    read_buf = RT_NULL;
    write_buf = RT_NULL;

    if (spidev ==  RT_NULL)
    {
        SPI2APB_ERR_LOG("Failed to find spi device spi1_0\n");
        return -1;
    }

    cfg.data_width = 8;
    cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_0 | RT_SPI_LSB;
    cfg.max_hz = 1000000;
    rt_spi_configure(spidev, &cfg);

    SPI2APB_INF_LOG("spi master, mode0, lsb, %dHz speed, data_width=8\n", cfg.max_hz);

#ifdef USE_UPDATE_IMG
    upgrade_state = SPI2APB_UPG_INIT;
#endif

    read_buf = rt_malloc(SPIDEV_RKSPI2APB_OP_BYTES);
    if (read_buf == NULL)
    {
        SPI2APB_ERR_LOG("malloc read_buf len %d fail\n", SPIDEV_RKSPI2APB_OP_BYTES);
        return -1;
    }

    write_buf = rt_malloc(SPIDEV_RKSPI2APB_OP_BYTES);
    if (write_buf == NULL)
    {
        SPI2APB_ERR_LOG("malloc write_buf len %d fail\n", SPIDEV_RKSPI2APB_OP_BYTES);
        return -1;
    }

    SPI2APB_INF_LOG("SPIDEV_RKSPI2APB_OP_BYTES %d\n", SPIDEV_RKSPI2APB_OP_BYTES);
    return 0;
}

#ifdef USE_UPDATE_IMG
rt_int32_t spi_upgrade_process(rt_uint8_t *data, rt_int32_t len)
{
    rt_int32_t ret;
    rt_uint32_t status;

    if (upgrade_state == SPI2APB_UPG_INIT)
    {
        if (len < UPDATE_IMG_HEADER_LEN)
        {
            SPI2APB_ERR_LOG("len(%d) < UPDATE_IMG_HEADER_LEN\n", len);
            return -1;
        }

        memcpy(&img_header, data, sizeof(spi2apb_img_header));
        SPI2APB_INF_LOG("img magic: %02x %02x %02x %02x\n", img_header.magic[0], img_header.magic[1], img_header.magic[2], img_header.magic[3]);
        if (memcmp(img_header.magic, "RKFP", 4) != 0)
        {
            SPI2APB_ERR_LOG("error rk2118_spi2apb_update.img file\n");
            return -1;
        }

        if (img_header.jshash_enable != 0)
        {
            calc_jshash = 0x47C6A7E6;
            SPI2APB_INF_LOG("js hash is enable\n");
        }

        SPI2APB_INF_LOG("%s: offset %x size %x/%d\n", img_header.fw_info[0].name, img_header.fw_info[0].offset,
                        img_header.fw_info[0].size, img_header.fw_info[0].size);
        SPI2APB_INF_LOG("%s: offset %x size %x/%d\n", img_header.fw_info[0].name, img_header.fw_info[1].offset,
                        img_header.fw_info[1].size, img_header.fw_info[1].size);

        SPI2APB_INF_LOG("start download %s\n", img_header.fw_info[0].name);

        upgrade_state = SPI2APB_UPG_DOWN_BIN;
        flash_src_addr = SPI_0471_ADDR;
        total_writed = 0;
        data_residue = firmware_offset - UPDATE_IMG_HEADER_LEN;
        SPI2APB_INF_LOG("state %d residue %d\n", upgrade_state, data_residue);
        if (data_residue > 0)
        {
            memcpy(data, data + UPDATE_IMG_HEADER_LEN, data_residue);
            return 1;
        }
        return 0;
    }
    else if (upgrade_state == SPI2APB_UPG_DOWN_BIN)
    {
        if (data_residue > 0)
        {
            len = data_residue;
            data_residue = 0;
        }

        if (img_header.jshash_enable != 0)
        {
            JS_Hash(data, len);
        }

        ret = spi_write_io(flash_src_addr, (void *)data, len);
        if (ret != 0)
        {
            SPI2APB_ERR_LOG("spi_write_io fail!, addr:0x%x\n", flash_src_addr);
            return -1;
        }

        flash_src_addr += len;
        total_writed += len;

        SPI2APB_INF_LOG("downloading bin %d/%d\n", total_writed, img_header.fw_info[0].size);
        if (total_writed >= img_header.fw_info[0].size)
        {
            SPI2APB_INF_LOG("download bin complete\n");
            upgrade_state = SPI2APB_UPG_DOWN_BIN_SEND_REQ;
            return 1;
        }
        return 0;
    }
    else if (upgrade_state == SPI2APB_UPG_DOWN_BIN_SEND_REQ)
    {
        SPI2APB_INF_LOG("send bin run request\n");
        ret = spi_send_request(0x0471, SPI_0471_ADDR, total_writed, (void *)NULL, BOOT_FLAG_CRC | BOOT_FLAG_RUN);
        if (ret < 0)
        {
            SPI2APB_ERR_LOG("send bin run request fail!:0x%x\n", ret);
            return -1;
        }
        upgrade_state = SPI2APB_UPG_DOWN_BIN_WAIT_READY;
        wait_timeout = 0;
        return 1;
    }
    else if (upgrade_state == SPI2APB_UPG_DOWN_BIN_WAIT_READY)
    {
        status = spi_read_status2();
        if ((status & 0xFFFF) != 0x0471)
        {
            if (wait_timeout++ >= WAIT_READY_TIMEOUT_MAX_COUNT)
            {
                SPI2APB_ERR_LOG("wait bin run failed status %x\n", status);
                return -1;
            }
            else
            {
                SPI2APB_ERR_LOG("wait bin run status %x\n", status);
                rt_thread_delay(RT_TICK_PER_SECOND / 1000);
                return 1;
            }
        }

        SPI2APB_INF_LOG("wait bin run success\n");

        upgrade_state = SPI2APB_UPG_DOWN_IMG;
        flash_src_addr = 0;
        total_writed = 0;
        data_residue = firmware_offset - img_header.fw_info[0].offset - img_header.fw_info[0].size;
        SPI2APB_INF_LOG("state %d residue %d\n", upgrade_state, data_residue);
        if (data_residue > 0)
        {
            memcpy(data, data + (len - data_residue), data_residue);
            return 1;
        }
    }
    else if (upgrade_state == SPI2APB_UPG_DOWN_IMG)
    {
        if (data_residue > 0)
        {
            len = data_residue;
        }

        if (data_residue == 0 && img_header.jshash_enable != 0)
        {
            JS_Hash(data, len);
        }

        if ((len % 512) != 0)
        {
            SPI2APB_ERR_LOG("%s len not 512 align %d\n", __func__, len);
            return -1;
        }

        ret = spi_write_io(SPI_XFER_ADDR1, (void *)data, len);
        if (ret < 0)
        {
            SPI2APB_ERR_LOG("spi_write_io fail addr:0x%x\n", SPI_XFER_ADDR1);
            return -1;
        }

        upgrade_state = SPI2APB_UPG_DOWN_IMG_SEND_REQ;
        return 1;
    }
    else if (upgrade_state == SPI2APB_UPG_DOWN_IMG_SEND_REQ)
    {
        spi_request_t req;

        if (data_residue > 0)
        {
            len = data_residue;
        }

        memset((void *)&req, 0, sizeof(spi_request_t));
        req.tag = 0x4B52;
        req.version = 0;
        req.head_size = sizeof(spi_request_t);

        req.buf_addr = SPI_XFER_ADDR1;
        req.lba = flash_src_addr;
        req.nsec = len >> 9;
        req.status = CMD_ST_BUSY;
        req.cmd = SPI_LBA_WRITE_10;

#if 1
        req.flag |= BOOT_FLAG_CRC;
        req.parity = spi_calc_hash((rt_uint32_t *)data, len);
        //SPI2APB_ERR_LOG("req.parity = %x\n", req.parity);
#endif

        ret = spi_write_io(SPI_REQ_ADDR, (void *)&req, sizeof(spi_request_t));
        if (ret < 0)
        {
            SPI2APB_ERR_LOG("spi_write_io fail addr:0x%x\n", SPI_REQ_ADDR);
            return -1;
        }

        rt_thread_delay(RT_TICK_PER_SECOND / 10000); // must do, else hash check fail

        upgrade_state = SPI2APB_UPG_DOWN_IMG_WAIT_READY;
        wait_timeout = 0;
        return 1;
    }
    else if (upgrade_state == SPI2APB_UPG_DOWN_IMG_WAIT_READY)
    {
        status = spi_read_status2();
        if ((status & 0xFFFF) != 0x0000)
        {
            if (wait_timeout++ >= WAIT_READY_TIMEOUT_MAX_COUNT)
            {
                SPI2APB_ERR_LOG("wait img write to flash failed status %x\n", status);
                return -1;
            }
            else
            {
                //SPI2APB_ERR_LOG("wait img write to flash status %x\n", status);
                rt_thread_delay(RT_TICK_PER_SECOND / 1000);
                return 1;
            }
        }

        if (data_residue > 0)
        {
            len = data_residue;
            data_residue = 0;
        }

        flash_src_addr += len >> 9;
        total_writed += len;

        SPI2APB_INF_LOG("downloading img %d/%d\n", total_writed, img_header.fw_info[1].size);

        if (total_writed >= img_header.fw_info[1].size)
        {
            SPI2APB_INF_LOG("download img complete\n");
            if (firmware_offset != img_header.fw_info[1].offset + img_header.fw_info[1].size)
            {
                SPI2APB_ERR_LOG("%s file end error %x/%x\n", __func__, firmware_offset,
                                img_header.fw_info[1].offset + img_header.fw_info[1].size);
                return -1;
            }

            if (img_header.jshash_enable != 0)
            {
                SPI2APB_INF_LOG("calc_jshash %08x img_header.jshash %08x\n", calc_jshash, img_header.jshash);
                if (calc_jshash != img_header.jshash)
                {
                    SPI2APB_ERR_LOG("js hash compare failed\n");
                    return -1;
                }
            }

            upgrade_state = SPI2APB_UPG_REBOOT;
            return 1;
        }

        upgrade_state = SPI2APB_UPG_DOWN_IMG;
        return 0;
    }
    else if (upgrade_state == SPI2APB_UPG_REBOOT)
    {
        SPI2APB_INF_LOG("downloaded total fw success, reboot rk2118\n");
        spi_reset_boot();
        return 0;
    }
    else
    {
        SPI2APB_ERR_LOG("%s: error state %d\n", __func__, upgrade_state);
        return -1;
    }

    return 0;
}
#endif

rt_int32_t spi_upgrade(char *bin_name, char *img_name)
{
    rt_int32_t ret = -1;
    rt_int32_t file_bin = -1, file_img = -1;

    file_bin = open((const char *)bin_name, O_RDONLY, 0);
    if (file_bin < 0)
    {
        SPI2APB_ERR_LOG("error opening %s file\n", bin_name);
        goto out;
    }

    SPI2APB_INF_LOG("success open %s\n", bin_name);

    file_img = open((const char *)img_name, O_RDONLY, 0);
    if (file_img  < 0)
    {
        SPI2APB_ERR_LOG("error opening %s\n", img_name);
        goto out;
    }

    SPI2APB_INF_LOG("success open %s\n", img_name);

    ret = wait_boot_ready(0xAA55);
    if (ret != 0)
        goto out;

    SPI2APB_INF_LOG("start download %s\n", bin_name);

    ret = spi_down_bin(0x0471, SPI_0471_ADDR, file_bin);
    if (ret != 0)
    {
        SPI2APB_ERR_LOG("download %s failed\n", bin_name);
        goto out;
    }

    SPI2APB_INF_LOG("downloaded %s success\n", bin_name);

    ret = wait_boot_ready(0x0471);
    if (ret != 0)
        goto out;

    SPI2APB_INF_LOG("wait %s boot success\n", bin_name);

    SPI2APB_INF_LOG("start download %s\n", img_name);

    ret = spi_down_img(0, 0, file_img);
    if (ret != 0)
    {
        SPI2APB_ERR_LOG("download %s failed\n", img_name);
        goto out;
    }

    SPI2APB_INF_LOG("downloaded %s success\n", img_name);

    spi_reset_boot();

    SPI2APB_INF_LOG("%s boot success\n", img_name);

out:
    if (file_bin >= 0)
        close(file_bin);
    if (file_img >= 0)
        close(file_img);
    return ret;
}

#ifdef USE_UPDATE_IMG
static void spi2apb_upgrade_thr(void *parameter)
{
    rt_int32_t file_img = -1;
    rt_int32_t readed;
    rt_int32_t ret;
    rt_uint32_t status = 0;

    if (spi_upgrade_init() != 0)
    {
        goto out;
    }

    // Must let slave rk2118 enter maskroom before upgrade fw

    file_img = open((const char *)img_name, O_RDONLY, 0);
    if (file_img < 0)
    {
        SPI2APB_ERR_LOG("error opening %s file\n", img_name);
        goto out;
    }

    SPI2APB_INF_LOG("success open %s\n", bin_name);

    wait_timeout = 0;
    while ((status & 0xFFFF) != 0xAA55)
    {
        status = spi_read_status2();
        if (wait_timeout++ >= WAIT_READY_TIMEOUT_MAX_COUNT)
        {
            SPI2APB_ERR_LOG("wait maskroom boot failed status %x\n", status);
            goto out;
        }
        else
        {
            rt_thread_delay(RT_TICK_PER_SECOND / 1000);
        }
    }
    SPI2APB_ERR_LOG("wait maskroom boot success\n");

    firmware_offset = 0;
    while (1)
    {
        readed = read(file_img, write_buf, SPIDEV_RKSPI2APB_OP_BYTES);
        firmware_offset += readed;
        if (readed == 0)
        {
            SPI2APB_INF_LOG("end of %s\n", img_name);
            goto out;
        }

re_process:
        ret = spi_upgrade_process(write_buf, readed);
        if (ret == -1)
        {
            SPI2APB_ERR_LOG("spi_upgrade_process error\n");
            goto out;
        }
        else if (ret == 1)
        {
            goto re_process;
        }
    }

out:
    if (file_img >= 0)
        close(file_img);
    if (read_buf != NULL)
        rt_free(read_buf);
    if (write_buf != NULL)
        rt_free(write_buf);
    return;
}
#else
static void spi2apb_upgrade_thr(void *parameter)
{
    if (spi_upgrade_init() != 0)
    {
        goto out;
    }

    // Must let slave rk2118 enter maskroom before upgrade fw

    spi_upgrade(bin_name, img_name);

out:
    if (read_buf != NULL)
        rt_free(read_buf);
    if (write_buf != NULL)
        rt_free(write_buf);
    return;
}
#endif

static void spi2apb_upgrade(int argc, char **argv)
{
    rt_thread_t thread;

#ifdef USE_UPDATE_IMG
    if (argc != 2)
    {
        rt_kprintf("Usage: spi2apb_upgrade <img file>\n");
        return;
    }

    strcpy(img_name, argv[1]);
#else
    if (argc != 3)
    {
        rt_kprintf("Usage: spi2apb_upgrade <bin file> <img file>\n");
        return;
    }

    strcpy(bin_name, argv[1]);
    strcpy(img_name, argv[2]);
#endif

    thread = rt_thread_create("spi2apb_upgrade",
                              spi2apb_upgrade_thr,
                              RT_NULL,
                              2048,
                              21, 20);
    RT_ASSERT(thread != RT_NULL);
    rt_thread_startup(thread);
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(spi2apb_upgrade, spi2apb upgrade test);
#endif

#endif