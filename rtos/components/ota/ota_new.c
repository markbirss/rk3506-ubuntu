/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <rtdevice.h>
#include <rtthread.h>
#include <dfs_file.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <drivers/mtd_nor.h>
#include "hal_base.h"
#include "hal_bsp.h"

#ifdef RT_USING_NEW_OTA
#include "../fwmgr/fw_analysis.h"
#include "../fwmgr/rkpart.h"
#include "drv_flash_partition.h"
#include "dma.h"
#include "ota_opt.h"
#include "ota.h"
#include "rkimage.h"

#define RK_OTA_RW_SIZE      4096
#define RK_OTA_INFO_SIZE    4096
#define RK_OTA_MAX_PART_COUNT   32

//#define RK_OTA_PRESS_TEST
//#define SIMU_ABNORMAL_POWER_OFF
//#define RK_OTA_MEM_TEST
#define OTA_FW_USE_UPDATE_IMG

#if defined(OTA_FW_USE_UPDATE_IMG) || !defined(RT_USING_SNOR)
#define OTA_FW_FILENAME "update.img"
#else
#define OTA_FW_FILENAME "Firmware.img"
#endif

#define OTA_RECOVERY_FLAG 0x5242c303
#ifdef RT_USING_SPINAND
#define OTA_MISC_RECOVERY_INFO_OFFSET (256 * 1024)
#endif

struct misc_recovery_info
{
    rt_uint32_t recovery_flag; // 0: normal; OTA_RECOVERY_FLAG: recovery
    rt_uint8_t release_data[7];
    rt_uint8_t reserv[1];
    rt_uint32_t fw_version;
};

RT_WEAK const char *const ota_fw_path[] =
{
    "/",
    "/udisk",
    "/sdcard"
};

extern STRUCT_RKIMAGE_HEAD rkimage_head;

static rt_thread_t ota_thread = RT_NULL;
static ota_protocol ota_prot_type;
static ota_flash_type ota_fla_type;
static int ota_file_fd;
static struct rt_mtd_nor_device *snor_dev = RT_NULL;
static struct rt_flash_partition *flash_part_info;
static int flash_part_num;
static struct rt_flash_partition *fw_rk_part_info;
static int fw_part_num;
static struct rt_flash_partition *dfs_part_info = RT_NULL;
static struct rt_flash_partition *dfs_misc_part = RT_NULL;
static int dfs_part_num = 0;
static int firmware_offset;
static int firmware_size;
static rt_uint8_t *fw_read_buf;
static rt_uint8_t *fw_read_chk_buf;
static char ota_url[512];
static char *ota_mem_buf;
static rt_uint32_t cur_running_slot;
static PRKIMAGE_HDR rkimage_hdr;
static struct rt_mtd_nand_device *snand_dev = RT_NULL;
static int udisk_inserted;
static char ota_test_str[32];
static struct rk_parttion_date_time dt_release_data_time;
static rt_uint32_t ui_fw_ver;

#if defined(RKMCU_RK2118) && defined(RT_USING_SDIO1)
extern int32_t get_rk_partition_emmc(struct rt_flash_partition **part);
#endif

static void rk_ota_reboot(void)
{
    RK_OTA_INF("reboot ...\n");
    rt_thread_delay(300);

    rt_hw_cpu_reset();
}

static rt_bool_t rk_ota_in_recovery_fw(void)
{
#ifdef RT_BUILD_RECOVERY_FW
    RK_OTA_INF("ota in recovery fw\n");
    return RT_TRUE;
#else
    RK_OTA_INF("ota not in recovery fw\n");
    return RT_FALSE;
#endif
}

static void rk_ota_run_recovery_fw(void)
{
    RK_OTA_INF("================ run recovery fw\n");

#ifdef SIMU_ABNORMAL_POWER_OFF
    rt_thread_delay(5 * RT_TICK_PER_SECOND);
#endif

#ifdef RT_USING_NEW_OTA_MODE_A_RECOVERY
    rk_ota_set_misc_recovery_flag(OTA_RECOVERY_FLAG);
#else
    GRF_PMU->OS_REG0 = OTA_RECOVERY_FLAG;
#endif
    rk_ota_reboot();
}

static void rk_ota_run_normal_fw(void)
{
    RK_OTA_INF("================ run normal fw\n");

#ifdef RT_USING_NEW_OTA_MODE_AB_RECOVERY
    GRF_PMU->OS_REG0 = 0;
#endif

    rk_ota_reboot();
}

/*
static void recovery_mode(int argc, char **argv)
{
    int slot;

    if (argc < 2)
    {
        rt_kprintf("Usage: recovery_mode in\n");
        rt_kprintf("       recovery_mode out\n");
        return;
    }

    if (!strcmp(argv[1], "in"))
    {
        rk_ota_run_recovery_fw();
    }
    else if (!strcmp(argv[1], "out"))
    {
        rk_ota_run_normal_fw();
    }
#ifdef SIMU_ABNORMAL_POWER_OFF
    else if (!strcmp(argv[1], "test"))
    {
        strcpy(ota_test_str, argv[2]);
        rt_kprintf("ota_test_str = %s\n", ota_test_str);
    }
#endif
    else
    {
        rt_kprintf("Usage: recovery_mode in\n");
        rt_kprintf("       recovery_mode out\n");
        return;
    }
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(recovery_mode, recovery mode);
#endif
*/

void rk_ota_set_boot_success(void)
{
    uint32_t slot = -1;
    fw_ab_data *fw_ab;

    RK_OTA_INF("%s\n", __func__);

    if (fw_slot_get_current_running(&slot) != RT_EOK)
        return;

    RK_OTA_INF("running slot %s Fw.\n", slot == 0 ? "A" : "B");

    fw_ab = fw_ab_data_get();
    if (fw_ab->slots[slot].successful_boot == 0)
        fw_slot_set_active(slot);
}

#ifdef RT_USING_SNOR
int get_snor_block_size(void)
{
    return 64 * 1024;
}
int rk_ota_get_flash_info(void *dev, DWORD *block_size, DWORD *page_size)
{
    *block_size = get_snor_block_size();
    *page_size = fw_flash_get_page_size();
    return 0;
}
#elif defined(RT_USING_SPINAND)
int rk_ota_get_flash_info(void *dev, DWORD *block_size, DWORD *page_size)
{
    *block_size = fw_flash_get_block_size();
    *page_size = fw_flash_get_page_size();
    return 0;
}
#else
int rk_ota_get_flash_info(void *dev, rt_uint32_t *block_size, rt_uint32_t *page_size)
{
    *block_size = 512;
    *page_size = 512;
    return 0;
}
#endif

int rk_ota_source_init(char *url)
{
    if (ota_prot_type == OTA_PROTOCOL_FILE)
    {
        RK_OTA_INF("OTA_PROTOCOL_FILE\n");

        ota_file_fd = open((const char *)url, O_RDONLY, 0);
        if (ota_file_fd < 0)
        {
            RK_OTA_ERR("%s: open %s failed errno %d\n", __func__, url, errno);
            return OTA_STATUS_ERR;
        }

        RK_OTA_INF("open %s success\n", url);
        return OTA_STATUS_OK;
    }
    else if (ota_prot_type == OTA_PROTOCOL_MEM)
    {
        RK_OTA_INF("OTA_PROTOCOL_MEM\n");

        if (ota_mem_buf == RT_NULL)
            return OTA_STATUS_ERR;

        firmware_offset = 0;
        return OTA_STATUS_OK;
    }

    return OTA_STATUS_ERR;
}

int rk_ota_source_read(rt_uint8_t *buf, rt_uint32_t length)
{
    int readed = 0;

    if (ota_prot_type == OTA_PROTOCOL_FILE)
    {
        if (ota_file_fd < 0)
            return 0;

        readed = read(ota_file_fd, buf, length);

        //RK_OTA_DBG("read %d readed %d\n", length, readed);
    }
    else if (ota_prot_type == OTA_PROTOCOL_MEM)
    {
        if (ota_mem_buf == RT_NULL)
            return OTA_STATUS_ERR;

        memcpy(buf, &ota_mem_buf[firmware_offset], length);
        readed = length;
    }

    firmware_offset += readed;
    //RK_OTA_DBG("firmware_offset 0x%x\n", firmware_offset);

    return readed;
}

int rk_ota_source_seek(rt_uint32_t offset, int whence)
{
    int readed = 0;

    if (ota_prot_type == OTA_PROTOCOL_FILE)
    {
        if (ota_file_fd < 0)
            return -1;

        firmware_offset = lseek(ota_file_fd, offset, whence);
        if (firmware_offset < 0)
        {
            RK_OTA_ERR("%s: offset %d whence %d failed\n", __func__, offset, whence);
            return -1;
        }
    }
    else if (ota_prot_type == OTA_PROTOCOL_MEM)
    {
        if (ota_mem_buf == RT_NULL)
            return -1;

        if (SEEK_SET == whence)
            firmware_offset = offset;
        else if (SEEK_CUR == whence)
            firmware_offset += offset;
        else if (SEEK_END == whence)
            firmware_offset = firmware_size + offset;
    }
    else
    {
        return -1;
    }

    //RK_OTA_DBG("firmware_offset 0x%x\n", firmware_offset);

    return firmware_offset;
}

int rk_ota_source_deinit(void)
{
    //RK_OTA_INF("%s\n", __func__);

    if (ota_prot_type == OTA_PROTOCOL_FILE)
    {
        if (ota_file_fd >= 0)
            close(ota_file_fd);
        ota_file_fd = -1;
    }
    else if (ota_prot_type == OTA_PROTOCOL_MEM)
    {
        //
    }

    return OTA_STATUS_OK;
}

static rt_bool_t check_misc_part(struct rt_flash_partition *misc)
{
#if defined(RT_USING_SNOR) || defined(RT_USING_SPINAND)
    if (misc->size < 2 * fw_flash_get_block_size())
    {
        RK_OTA_ERR("%s: misc size %d too small\n", __func__, misc->size);
        return RT_FALSE;
    }
    return RT_TRUE;
#endif
    return RT_TRUE;
}

int rk_ota_get_dfs_part_info(void)
{
    int i;
    int ret = OTA_STATUS_OK;

    if (dfs_part_num != 0 && dfs_part_info != RT_NULL)
        return OTA_STATUS_OK;

    RK_OTA_INF("================ ota_get_dfs_part_info\n");

#ifdef RT_USING_SNOR
    RK_OTA_INF("get dfs part info in snor flash\n");
#elif defined(RT_USING_SPINAND)
    RK_OTA_INF("get dfs part info in snand flash\n");

    RK_OTA_INF("snand page_size %d\n", fw_flash_get_page_size());
    RK_OTA_INF("snand block_size %d\n", fw_flash_get_block_size());
    if (fw_flash_get_page_size() != 2048 && fw_flash_get_page_size() != 4096)
    {
        RK_OTA_ERR("%s: snand error page_size %d\n", __func__, fw_flash_get_page_size());
        ret = OTA_STATUS_ERR;
        goto get_dfs_part_out;
    }

    if (fw_flash_get_block_size() > 256 * 1024)
    {
        RK_OTA_ERR("%s: snand block size > 256K\n", __func__);
        ret = OTA_STATUS_ERR;
        goto get_dfs_part_out;
    }
#endif

#if defined(RKMCU_RK2118) && defined(RT_USING_SDIO1)
    RK_OTA_INF("get dfs part info in emmc\n");
    dfs_part_num = get_rk_partition_emmc(&dfs_part_info);
#else
    dfs_part_num = get_rk_partition(&dfs_part_info);
#endif
    if (dfs_part_num == 0 || dfs_part_info == RT_NULL)
    {
        RK_OTA_ERR("%s: get_gpt_partition failed\n", __func__);
        ret = OTA_STATUS_ERR;
        goto get_dfs_part_out;
    }

    dfs_misc_part = RT_NULL;
    for (i = 0; i < dfs_part_num; i++)
    {
        if (!strcmp(dfs_part_info[i].name, "misc"))
        {
            dfs_misc_part = &dfs_part_info[i];
            if (check_misc_part(dfs_misc_part) != RT_TRUE)
            {
                ret = OTA_STATUS_ERR;
                goto get_dfs_part_out;
            }
        }
    }

    if (dfs_misc_part == RT_NULL)
    {
        RK_OTA_ERR("%s: no misc part\n", __func__);
        ret = OTA_STATUS_ERR;
        goto get_dfs_part_out;
    }

    return OTA_STATUS_OK;

get_dfs_part_out:
    dfs_part_info = RT_NULL;
    dfs_part_num = 0;
    dfs_misc_part = RT_NULL;

    return ret;
}

int rk_ota_dfs_part_info_chk(void)
{
    int i;
    int ret = OTA_STATUS_OK;
#ifndef RT_USING_NEW_OTA_MODE_AB
    struct rt_flash_partition *dfs_recovery_part = RT_NULL;
#endif

    for (i = 0; i < dfs_part_num; i++)
    {
#if defined(RT_USING_SNOR) || defined(RT_USING_SPINAND)
        if (strcmp(dfs_part_info[i].name, "userdata") && dfs_part_info[i].size % fw_flash_get_block_size())
        {
            RK_OTA_ERR("%s: part %s size 0x%x not 0x%x align\n", __func__,
                       dfs_part_info[i].name, dfs_part_info[i].size, fw_flash_get_block_size());
            ret = OTA_STATUS_ERR;
            goto chk_dfs_part_out;
        }
#endif

#ifndef RT_USING_NEW_OTA_MODE_AB
        if (!strcmp(dfs_part_info[i].name, "recovery"))
        {
            dfs_recovery_part = &dfs_part_info[i];
        }
#endif
    }

#ifndef RT_USING_SNOR
    if (dfs_misc_part->size < 512 * 1024)
    {
        RK_OTA_ERR("%s: misc part size %d < 512K\n", __func__, dfs_misc_part->size);
        ret = OTA_STATUS_ERR;
        goto chk_dfs_part_out;
    }
#endif

#ifndef RT_USING_NEW_OTA_MODE_AB
    if (dfs_recovery_part == RT_NULL)
    {
        RK_OTA_ERR("%s: no recovery part\n", __func__);
        ret = OTA_STATUS_ERR;
        goto chk_dfs_part_out;
    }
#endif

    return OTA_STATUS_OK;

chk_dfs_part_out:
    return ret;
}

int rk_ota_get_misc_part_offset(void)
{
    if (dfs_part_info == RT_NULL)
        rk_ota_get_dfs_part_info();

    if (dfs_misc_part != RT_NULL)
        return dfs_misc_part->offset;
    else
        return 0;
}

static int get_misc_rec_info_off(void)
{
//#ifdef RT_USING_SNOR
    return dfs_misc_part->size / 2;
//#else
//    return OTA_MISC_RECOVERY_INFO_OFFSET;
//#endif
}

int rk_ota_read_misc_recovery_info(struct misc_recovery_info *info)
{
    int ret = OTA_STATUS_OK;
    rt_uint8_t *read_buf = RT_NULL;
    rt_uint32_t misc_offset;

    read_buf = rt_dma_malloc(RK_OTA_INFO_SIZE);
    if (read_buf == RT_NULL)
    {
        RK_OTA_ERR("%s: malloc read_buf failed\n", __func__);
        return OTA_STATUS_ERR;
    }

    misc_offset = rk_ota_get_misc_part_offset();
    if (misc_offset == 0)
    {
        RK_OTA_ERR("%s: rk_ota_get_misc_part_offset failed\n", __func__);
        ret = OTA_STATUS_ERR;
        goto read_misc_recovery_info_out;
    }

    misc_offset += get_misc_rec_info_off();
    if (fw_flash_read(misc_offset, read_buf, RK_OTA_INFO_SIZE) != RK_OTA_INFO_SIZE)
    {
        RK_OTA_ERR("%s: fw_flash_read failed\n", __func__);
        ret = OTA_STATUS_ERR;
        goto read_misc_recovery_info_out;
    }

    memcpy(info, read_buf, sizeof(struct misc_recovery_info));
    if (info->recovery_flag != 0 && info->recovery_flag != OTA_RECOVERY_FLAG)
    {
#if 0
        RK_OTA_ERR("%s: error recovery_flag %x\n", __func__, info->recovery_flag);
        ret = OTA_STATUS_ERR;
        goto read_misc_recovery_info_out;
#else
        info->recovery_flag = 0;
#endif
    }

read_misc_recovery_info_out:
    if (read_buf != RT_NULL)
        rt_free_align(read_buf);

    return ret;
}

int rk_ota_write_misc_recovery_info(struct misc_recovery_info *info)
{
    int ret = OTA_STATUS_OK;
    rt_uint8_t *read_buf = RT_NULL;
    rt_uint32_t misc_offset;
    int block_size = 4096;
    int write_size = 4096;

    if (info->recovery_flag != 0 && info->recovery_flag != OTA_RECOVERY_FLAG)
    {
        RK_OTA_ERR("%s: error recovery_flag %x\n", __func__, info->recovery_flag);
        return OTA_STATUS_ERR;
    }

    read_buf = rt_dma_malloc(RK_OTA_INFO_SIZE);
    if (read_buf == RT_NULL)
    {
        RK_OTA_ERR("%s: malloc read_buf failed\n", __func__);
        return OTA_STATUS_ERR;
    }

    memset(read_buf, 0, RK_OTA_INFO_SIZE);
    memcpy(read_buf, info, sizeof(struct misc_recovery_info));

    misc_offset = rk_ota_get_misc_part_offset();
    if (misc_offset == 0)
    {
        RK_OTA_ERR("%s: rk_ota_get_misc_part_offset failed\n", __func__);
        ret = OTA_STATUS_ERR;
        goto write_misc_recovery_info_out;
    }

#ifdef RT_USING_SPNOR
    block_size = fw_flash_get_block_size();
    write_size = block_size;
#elif defined(RT_USING_SPINAND)
    write_size = RK_OTA_INFO_SIZE;
    block_size = fw_flash_get_block_size();
    if (block_size > 256 * 1024)
    {
        RK_OTA_ERR("%s: snand block size > 256K\n", __func__);
        ret = OTA_STATUS_ERR;
        goto write_misc_recovery_info_out;
    }
#endif

    misc_offset += get_misc_rec_info_off();
    if (fw_flash_erase(misc_offset, block_size) != RT_EOK)
    {
        RK_OTA_ERR("%s: flash erase offset 0x%x fail\n", __func__, misc_offset);
        ret = OTA_STATUS_ERR;
        goto write_misc_recovery_info_out;
    }

    if (fw_flash_write(misc_offset, read_buf, write_size) != write_size)
    {
        RK_OTA_ERR("%s: fw_flash_write fail\n", __func__);
        ret = OTA_STATUS_ERR;
        goto write_misc_recovery_info_out;
    }

write_misc_recovery_info_out:
    if (read_buf != RT_NULL)
        rt_free_align(read_buf);

    return ret;
}

int rk_ota_get_misc_recovery_flag(rt_uint32_t *recovery_flag)
{
    struct misc_recovery_info info;

    if (rk_ota_read_misc_recovery_info(&info) != OTA_STATUS_OK)
        return OTA_STATUS_ERR;

    *recovery_flag = info.recovery_flag;
    RK_OTA_INF("get recovery_flag %x\n", *recovery_flag);

    return OTA_STATUS_OK;
}

int rk_ota_set_misc_recovery_flag(rt_uint32_t recovery_flag)
{
    struct misc_recovery_info info;

    if (rk_ota_read_misc_recovery_info(&info) != OTA_STATUS_OK)
        return OTA_STATUS_ERR;

    info.recovery_flag = recovery_flag;
    if (rk_ota_write_misc_recovery_info(&info) != OTA_STATUS_OK)
        return OTA_STATUS_ERR;

    RK_OTA_INF("set recovery_flag %x\n", recovery_flag);
    return OTA_STATUS_OK;
}

int rk_ota_get_misc_release_data(rt_uint8_t *release_data)
{
    struct misc_recovery_info info;

    if (rk_ota_read_misc_recovery_info(&info) != OTA_STATUS_OK)
        return OTA_STATUS_ERR;

    memcpy(release_data, &info.release_data, 7);
    RK_OTA_INF("get misc release date: %d:%02d:%02d:%02d:%02d:%02d\n", ((rt_uint16_t)release_data[1] << 8) | release_data[0],
               release_data[2], release_data[3], release_data[4], release_data[5], release_data[6]);

    return OTA_STATUS_OK;
}

int rk_ota_set_misc_release_data(rt_uint8_t *release_data)
{
    struct misc_recovery_info info;

    if (rk_ota_read_misc_recovery_info(&info) != OTA_STATUS_OK)
        return OTA_STATUS_ERR;

    memcpy(&info.release_data, release_data, 7);
    if (rk_ota_write_misc_recovery_info(&info) != OTA_STATUS_OK)
        return OTA_STATUS_ERR;

    RK_OTA_INF("set misc release date: %d:%02d:%02d:%02d:%02d:%02d\n", ((rt_uint16_t)release_data[1] << 8) | release_data[0],
               release_data[2], release_data[3], release_data[4], release_data[5], release_data[6]);
    return OTA_STATUS_OK;
}

int rk_ota_get_misc_fw_versioin(rt_uint32_t *version)
{
    struct misc_recovery_info info;

    if (rk_ota_read_misc_recovery_info(&info) != OTA_STATUS_OK)
        return OTA_STATUS_ERR;

    *version = info.fw_version;
    RK_OTA_INF("get misc fw version: 0x%x\n", info.fw_version);

    return OTA_STATUS_OK;
}

int rk_ota_set_misc_fw_versioin(rt_uint32_t version)
{
    struct misc_recovery_info info;

    if (rk_ota_read_misc_recovery_info(&info) != OTA_STATUS_OK)
        return OTA_STATUS_ERR;

    info.fw_version = version;
    if (rk_ota_write_misc_recovery_info(&info) != OTA_STATUS_OK)
        return OTA_STATUS_ERR;

    RK_OTA_INF("set misc fw version: 0x%x\n", info.fw_version);
    return OTA_STATUS_OK;
}

rt_bool_t rk_ota_fw_version_chk(void)
{
    struct misc_recovery_info info;
    //rt_uint8_t *release_data_fw;

    if (rk_ota_read_misc_recovery_info(&info) != OTA_STATUS_OK)
        return RT_FALSE;

    if (info.fw_version == 0)
        info.fw_version = firmware_version;
#if defined(OTA_FW_USE_UPDATE_IMG) || !defined(RT_USING_SNOR)
    RK_OTA_INF("current fw_version 0x%x new fw_version 0x%x\n", info.fw_version, rkimage_head.dwVersion);
    if (info.fw_version == rkimage_head.dwVersion)
    {
        RK_OTA_INF("%s: fw version is same\n", __func__);
        return RT_FALSE;
    }

    /*release_data_fw = (rt_uint8_t *)&rkimage_head.stReleaseTime;
    RK_OTA_INF("fw release data: %d:%02d:%02d:%02d:%02d:%02d\n", ((rt_uint16_t)release_data_fw[1] << 8) | release_data_fw[0],
               release_data_fw[2], release_data_fw[3], release_data_fw[4], release_data_fw[5], release_data_fw[6]);

    if (!memcmp(&rkimage_head.stReleaseTime, info.release_data, 7))
    {
        RK_OTA_INF("%s: fw release data is same\n", __func__);
        return RT_FALSE;
    }*/
#else
    RK_OTA_INF("current fw_version 0x%x new fw_version 0x%x\n", info.fw_version, ui_fw_ver);
    if (info.fw_version == ui_fw_ver)
    {
        RK_OTA_INF("%s: fw version is same\n", __func__);
        return RT_FALSE;
    }

    /*release_data_fw = (rt_uint8_t *)&dt_release_data_time;
    RK_OTA_INF("fw release data: %d:%02d:%02d:%02d:%02d:%02d\n", ((rt_uint16_t)release_data_fw[1] << 8) | release_data_fw[0],
               release_data_fw[2], release_data_fw[3], release_data_fw[4], release_data_fw[5], release_data_fw[6]);

    if (!memcmp(&dt_release_data_time, info.release_data, 7))
    {
        RK_OTA_INF("%s: fw release data is same\n", __func__);
        return RT_FALSE;
    }*/
#endif

    return RT_TRUE;
}

#if defined(OTA_FW_USE_UPDATE_IMG) || !defined(RT_USING_SNOR)

int rk_ota_part_info_chk(char *url)
{
    int i, j;
    int ret = OTA_STATUS_OK;

    RK_OTA_INF("================ ota_part_info_check\n");
    RK_OTA_INF("get part info in %s\n", url);

    if (rkimage_hdr == RT_NULL)
        return OTA_STATUS_ERR;

    ret = analyticImage(url, rkimage_hdr);
    if (ret != 0)
    {
        RK_OTA_ERR("%s: analyticImage failed %d\n", __func__, ret);
        ret = OTA_STATUS_ERR;
        goto part_info_chk_out;
    }

    for (i = 0; i < rkimage_hdr->item_count; i++)
    {
        for (j = 0; j < dfs_part_num; j++)
        {
            if (!strcmp(dfs_part_info[j].name, rkimage_hdr->item[i].name))
            {
                //RK_OTA_INF("match %s part info\n", dfs_part_info[j].name);
                if (dfs_part_info[j].offset != rkimage_hdr->item[i].flash_offset * 512)
                {
                    RK_OTA_ERR("%s: offset not match %x/%x\n", dfs_part_info[j].name, dfs_part_info[j].offset,
                               rkimage_hdr->item[i].flash_offset * 512);
                    ret = OTA_STATUS_ERR;
                    goto part_info_chk_out;
                }
                if (rkimage_hdr->item[i].size > dfs_part_info[j].size)
                {
                    RK_OTA_ERR("%s: size exceed %x/%x\n", dfs_part_info[j].name, dfs_part_info[j].size,
                               rkimage_hdr->item[i].size);
                    ret = OTA_STATUS_ERR;
                    goto part_info_chk_out;
                }
            }
        }
    }

part_info_chk_out:
    return ret;
}
#else
int rk_ota_part_info_chk(char *url)
{
    struct rk_partition_info *part_temp;
    int part_num;
    int i;
    int ret = OTA_STATUS_OK;

    RK_OTA_INF("================ ota_part_info_check\n");
    RK_OTA_INF("get part info in %s\n", url);

    if (rk_ota_source_seek(0, SEEK_SET) < 0)
    {
        RK_OTA_ERR("%s: rk_ota_source_seek failed\n", __func__);
        ret = OTA_STATUS_ERR;
        goto part_info_chk_out;
    }

    part_temp = (struct rk_partition_info *)fw_read_buf;
    if (rk_ota_source_read((rt_uint8_t *)part_temp, RK_OTA_RW_SIZE) != RK_OTA_RW_SIZE)
    {
        RK_OTA_ERR("%s: rk_ota_source_read failed\n", __func__);
        ret = OTA_STATUS_ERR;
        goto part_info_chk_out;
    }

    if (part_temp->hdr.ui_fw_tag != RK_PARTITION_TAG)
    {
        RK_OTA_ERR("%s: partition tag mismatch\n", __func__);
        ret = OTA_STATUS_ERR;
        goto part_info_chk_out;
    }

    memcpy(&dt_release_data_time, &part_temp->hdr.dt_release_data_time, 7);
    ui_fw_ver = part_temp->hdr.ui_fw_ver;

    fw_part_num = part_temp->hdr.ui_part_entry_count;
    fw_rk_part_info = rt_malloc(sizeof(struct rt_flash_partition) * fw_part_num);
    if (fw_rk_part_info == RT_NULL)
    {
        RK_OTA_ERR("%s: malloc fw_rk_part_info failed\n", __func__);
        ret = OTA_STATUS_ERR;
        goto part_info_chk_out;
    }
    rt_memset(fw_rk_part_info, 0, sizeof(struct rt_flash_partition) * fw_part_num);

    for (i = 0; i < fw_part_num; i++)
    {
        rt_strncpy(fw_rk_part_info[i].name, (const char *)part_temp->part[i].sz_name,
                   RK_PARTITION_NAME_SIZE);
        fw_rk_part_info[i].offset = (rt_uint32_t)part_temp->part[i].ui_pt_off << 9;
        if (part_temp->part[i].ui_pt_sz == 0xFFFFFFFF ||
                (part_temp->part[i].ui_part_property & RK_PARTITION_NO_PARTITION_SIZE))
        {
            fw_rk_part_info[i].size = fw_flash_get_size() - fw_rk_part_info[i].offset;
        }
        else
        {
            fw_rk_part_info[i].size = (rt_uint32_t)part_temp->part[i].ui_pt_sz << 9;
            if ((fw_rk_part_info[i].size % RK_OTA_RW_SIZE) != 0)
            {
                RK_OTA_ERR("%s: part[%d]: name %s, size %d not %d align\n",
                           __func__, i, fw_rk_part_info[i].name, fw_rk_part_info[i].size, RK_OTA_RW_SIZE);
                ret = OTA_STATUS_ERR;
                goto part_info_chk_out;
            }
        }
        fw_rk_part_info[i].type = (rt_uint32_t)part_temp->part[i].em_part_type;

        RK_OTA_INF("part[%d]: name %s, offset 0x%x/0x%x size 0x%x/0x%x type 0x%x\n",
                   i, fw_rk_part_info[i].name, fw_rk_part_info[i].offset, fw_rk_part_info[i].offset / 512,
                   fw_rk_part_info[i].size, fw_rk_part_info[i].size / 512, fw_rk_part_info[i].type);
    }

    if (fw_part_num != dfs_part_num)
    {
        RK_OTA_ERR("%s: partition num not match %d/%d\n", __func__, fw_part_num, dfs_part_num);
        ret = OTA_STATUS_ERR;
        goto part_info_chk_out;
    }

    part_num = fw_part_num < dfs_part_num ? fw_part_num : dfs_part_num;
    for (i = 0; i < part_num; i++)
    {
        if (strcmp(fw_rk_part_info[i].name, dfs_part_info[i].name) != 0)
        {
            RK_OTA_ERR("%s: part[%d] name not match %s/%s\n", __func__, i,
                       fw_rk_part_info[i].name, dfs_part_info[i].name);
            ret = OTA_STATUS_ERR;
            goto part_info_chk_out;
        }

        if (strstr(fw_rk_part_info[i].name, "userdata"))
            continue;

        if (fw_rk_part_info[i].offset != dfs_part_info[i].offset)
        {
            RK_OTA_ERR("%s: part[%d] offset not match %d/%d\n", __func__, i,
                       fw_rk_part_info[i].offset, dfs_part_info[i].offset);
            ret = OTA_STATUS_ERR;
            goto part_info_chk_out;
        }

        if (dfs_part_info[i].size == 0xFFFFFFFF)
            dfs_part_info[i].size = fw_rk_part_info[i].size;

        if (fw_rk_part_info[i].size != dfs_part_info[i].size)
        {
            RK_OTA_ERR("%s: part[%d] size not match %d/%d\n", __func__, i,
                       fw_rk_part_info[i].size, dfs_part_info[i].size);
            ret = OTA_STATUS_ERR;
            goto part_info_chk_out;
        }

        /*if (fw_rk_part_info[i].type != dfs_part_info[i].type) {
            RK_OTA_ERR("%s: part[%d] type not match %d/%d\n", __func__, i,
                fw_rk_part_info[i].type, dfs_part_info[i].type);
            ret = OTA_STATUS_ERR;
            goto part_info_chk_out;
        }*/
    }

part_info_chk_out:
    return ret;
}
#endif

#if defined(RT_USING_NEW_OTA_MODE_AB) || defined(RT_BUILD_RECOVERY_FW)
/* upgrade signal partition */
int rk_ota_upgrade_part(int i, rt_uint32_t offset, rt_uint32_t size)
{
    int want_read;
    int readed, writed;
    int tot_writed = 0;
    int tot_size = size;
    int offset1 = offset;
    int size1 = size;

#if defined(RKMCU_RK2118) && defined(RT_USING_SDIO1)
    size1 = 0;
#endif

    RK_OTA_INF("upgrade_part %s: part offset 0x%x/0x%x file size 0x%x part size 0x%x\n",
               dfs_part_info[i].name, offset, offset / 512, size, dfs_part_info[i].size);

    while (size1)
    {
#ifdef RT_USING_SNOR
        int erase_size = get_snor_block_size();
        readed = size1 > erase_size ? erase_size : fw_flash_get_block_size();
#elif defined(RT_USING_SPINAND)
        readed = fw_flash_get_block_size();
#endif
        if (fw_flash_erase(offset1, readed) != RT_EOK)
        {
            RK_OTA_ERR("%s: flash erase offset 0x%x fail\n", __func__, offset1);
            return OTA_STATUS_ERR;
        }

        if (size1 >= readed)
            size1 -= readed;
        else
            size1 = 0;

        RK_OTA_INF("upgrade_part: erase 0x%x/0x%x\n", offset1, readed);
        offset1 += readed;
    }

#ifdef SIMU_ABNORMAL_POWER_OFF
    if (!strcmp(ota_test_str, dfs_part_info[i].name))
    {
        RK_OTA_INF("simulate abnormal power off during upgrade\n");
        while (1);
    }
#endif

    while (size)
    {
        want_read = size > RK_OTA_RW_SIZE ? RK_OTA_RW_SIZE : size;
        memset(fw_read_buf, 0, RK_OTA_RW_SIZE);

        readed = rk_ota_source_read(fw_read_buf, want_read);
        if (readed != want_read)
        {
            if (readed == 0)
                return OTA_STATUS_OK;
            RK_OTA_ERR("%s: ota_source_read fail %d / %d\n", __func__, want_read, readed);
            return OTA_STATUS_ERR;
        }

#ifdef RT_USING_SNOR
        writed = fw_flash_write(offset, fw_read_buf, readed);
        if (writed != readed)
#else
        writed = fw_flash_write(offset, fw_read_buf, RK_OTA_RW_SIZE);
        if (writed != RK_OTA_RW_SIZE)
#endif
        {
            RK_OTA_ERR("%s: flash_write offset 0x%x fail %d / %d\n", __func__, offset, readed, writed);
            return OTA_STATUS_ERR;
        }

#ifdef RT_USING_SNOR
        writed = fw_flash_read(offset, fw_read_chk_buf, readed);
        if (writed != readed)
#else
        writed = fw_flash_read(offset, fw_read_chk_buf, RK_OTA_RW_SIZE);
        if (writed != RK_OTA_RW_SIZE)
#endif
        {
            RK_OTA_ERR("%s: flash_read offset 0x%x fail %d / %d\n", __func__, offset, readed, writed);
            return OTA_STATUS_ERR;
        }

        if (memcmp(fw_read_buf, fw_read_chk_buf, readed) != 0)
        {
            RK_OTA_ERR("%s: flash readback offset 0x%x cmp failed\n", __func__, offset);
            return OTA_STATUS_ERR;
        }

        size -= readed;
        offset += readed;
        tot_writed += readed;
        RK_OTA_INF("upgrade_part: writed %d/%d\n", tot_writed, tot_size);
    }

    return OTA_STATUS_OK;
}

int rk_ota_upgrade_part_check(rt_uint32_t offset, rt_uint32_t size)
{
    int want_read;
    int readed, writed;
    int tot_writed = 0;
    int tot_size = size;

    RK_OTA_INF("upgrade_part_check: offset 0x%x size 0x%x\n", offset, size);

    while (size)
    {
        want_read = size > RK_OTA_RW_SIZE ? RK_OTA_RW_SIZE : size;
        readed = rk_ota_source_read(fw_read_buf, want_read);
        if (readed != want_read)
        {
            RK_OTA_ERR("%s: ota_source_read fail %d / %d\n", __func__, want_read, readed);
            return OTA_STATUS_ERR;
        }

#ifdef RT_USING_SNOR
        writed = fw_flash_read(offset, fw_read_chk_buf, readed);
        if (writed != readed)
        {
#else
        writed = fw_flash_read(offset, fw_read_chk_buf, RK_OTA_RW_SIZE);
        if (writed != RK_OTA_RW_SIZE)
        {
#endif
            RK_OTA_ERR("%s: flash_read offset 0x%x fail %d / %d\n", __func__, offset, readed, writed);
            return OTA_STATUS_ERR;
        }

        if (memcmp(fw_read_buf, fw_read_chk_buf, readed) != 0)
        {
            RK_OTA_ERR("%s: flash readback offset 0x%x cmp failed\n", __func__, offset);
            return OTA_STATUS_ERR;
        }

        size -= readed;
        offset += readed;
        tot_writed += readed;
        RK_OTA_INF("upgrade_part_check: check %d/%d\n", tot_writed, tot_size);
    }

    return OTA_STATUS_OK;
}

static int rk_ota_get_part_info_for_upgrade(int i, rt_uint32_t *file_offset, rt_uint32_t *flash_offset, rt_uint32_t *size)
{
#if defined(OTA_FW_USE_UPDATE_IMG) || !defined(RT_USING_SNOR)
    int j;

    if (!strcmp(dfs_part_info[i].name, "loader") ||
            !strcmp(dfs_part_info[i].name, "NIDB_a") ||
            !strcmp(dfs_part_info[i].name, "NIDB_b"))
    {
        return OTA_STATUS_ERR;
    }

    for (j = 0; j < rkimage_hdr->item_count; j++)
    {
        if (!strcmp(dfs_part_info[i].name, rkimage_hdr->item[j].name))
        {
            *file_offset = rkimage_hdr->item[j].offset;
            *flash_offset = dfs_part_info[i].offset;
            *size = rkimage_hdr->item[j].size;
            return OTA_STATUS_OK;
        }
    }

    return OTA_STATUS_ERR;
#else
    *file_offset = dfs_part_info[i].offset;
    *flash_offset = dfs_part_info[i].offset;
    *size = dfs_part_info[i].size;
    return OTA_STATUS_OK;
#endif
}

#if defined(OTA_FW_USE_UPDATE_IMG) || !defined(RT_USING_SNOR)
int rk_ota_upgrade_loader_idb(char *name, rt_uint8_t *idb, rt_uint32_t offset, rt_uint32_t size)
{
    int want_read;
    int readed, writed;
    int tot_writed = 0;
    int tot_size = size;
    int offset1 = offset;
    int size1 = size;

#if defined(RKMCU_RK2118) && defined(RT_USING_SDIO1)
    size1 = 0;
#endif

    RK_OTA_INF("upgrade_loader_idb %s: offset 0x%x/0x%x size 0x%x\n", name, offset, offset / 512, size);

    while (size1)
    {
#ifdef RT_USING_SNOR
        readed = get_snor_block_size();
#elif defined(RT_USING_SPINAND)
        readed = fw_flash_get_block_size();
#endif
        if (fw_flash_erase(offset1, readed) != RT_EOK)
        {
            RK_OTA_ERR("%s: flash erase offset 0x%x fail\n", __func__, offset1);
            return OTA_STATUS_ERR;
        }

        if (size1 >= readed)
            size1 -= readed;
        else
            size1 = 0;

        RK_OTA_INF("upgrade_loader_idb: erase 0x%x/0x%x\n", offset1, readed);
        offset1 += readed;
    }

#ifdef SIMU_ABNORMAL_POWER_OFF
    if (!strcmp(ota_test_str, name))
    {
        RK_OTA_INF("simulate abnormal power off during upgrade\n");
        while (1);
    }
#endif

    while (size)
    {
        want_read = RK_OTA_RW_SIZE;
        readed = size > want_read ? want_read : size;
        memset(fw_read_buf, 0, want_read);
        memcpy(fw_read_buf, idb, readed);

#ifdef RT_USING_SNOR
        want_read = readed;
#endif
        writed = fw_flash_write(offset, fw_read_buf, want_read);
        if (writed != want_read)
        {
            RK_OTA_ERR("%s: flash_write offset 0x%x fail %d / %d\n", __func__, offset, readed, writed);
            return OTA_STATUS_ERR;
        }

        writed = fw_flash_read(offset, fw_read_chk_buf, want_read);
        if (writed != want_read)
        {
            RK_OTA_ERR("%s: flash_read offset 0x%x fail %d / %d\n", __func__, offset, readed, writed);
            return OTA_STATUS_ERR;
        }

        if (memcmp(fw_read_buf, fw_read_chk_buf, readed) != 0)
        {
            RK_OTA_ERR("%s: flash readback offset 0x%x cmp failed\n", __func__, offset);
            return OTA_STATUS_ERR;
        }

        size -= readed;
        offset += readed;
        tot_writed += readed;
        idb += readed;
        RK_OTA_INF("upgrade_loader_idb: writed %d/%d\n", tot_writed, tot_size);
    }

    return OTA_STATUS_OK;
}

int rk_ota_upgrade_loader(void)
{
    rt_uint8_t *loader_buf = RT_NULL;
    int loader_len = rkimage_head.dwBootSize;
    int ret = OTA_STATUS_OK;

    RK_OTA_INF("================ start upgrade_loader ...\n");

    loader_buf = rt_malloc(loader_len);
    if (loader_buf == RT_NULL)
    {
        RK_OTA_ERR("%s: malloc %d failed\n", __func__, loader_len);
        ret = OTA_STATUS_ERR;
        goto upgrade_loader_out;
    }

    if (rk_ota_source_seek(rkimage_head.dwBootOffset, SEEK_SET) < 0)
    {
        RK_OTA_ERR("%s: seek %d failed\n", __func__, rkimage_head.dwBootOffset);
        ret = OTA_STATUS_ERR;
        goto upgrade_loader_out;
    }

    if (rk_ota_source_read(loader_buf, loader_len) != loader_len)
    {
        RK_OTA_ERR("%s: read %d failed\n", __func__, loader_len);
        ret = OTA_STATUS_ERR;
        goto upgrade_loader_out;
    }

    if (rk_ota_download_loader(loader_buf, loader_len, NULL) != RT_TRUE)
    {
        RK_OTA_ERR("%s: downlaod loader failed\n", __func__);
        ret = OTA_STATUS_ERR;
        goto upgrade_loader_out;
    }

    RK_OTA_INF("upgrade_loader: success\n");

upgrade_loader_out:
    if (loader_buf != RT_NULL)
        rt_free(loader_buf);

    return ret;
}
#endif

/* upgrade whole Firmware.img */
int rk_ota_upgrade_fw(void)
{
    int i;
    rt_uint32_t file_offset;
    rt_uint32_t flash_offset;
    rt_uint32_t size;
    char ab[4];

    if (cur_running_slot == 0)
        strcpy(ab, "_a");
    else
        strcpy(ab, "_b");

    RK_OTA_INF("================ start upgrade fw ...\n");

    for (i = 0; i < dfs_part_num; i++)
    {
        RK_OTA_INF("upgrade_fw: part[%d]: name %s, offset 0x%x size 0x%x type 0x%x\n",
                   i, dfs_part_info[i].name, dfs_part_info[i].offset,
                   dfs_part_info[i].size, dfs_part_info[i].type);

        if (rk_ota_get_part_info_for_upgrade(i, &file_offset, &flash_offset, &size) != OTA_STATUS_OK)
        {
            RK_OTA_INF("upgrade_fw: skip %s upgrade\n", dfs_part_info[i].name);
            continue;
        }

        if (!strcmp(dfs_part_info[i].name, "vendor"))
        {
            RK_OTA_INF("upgrade_fw: skip %s upgrade\n", dfs_part_info[i].name);
            continue;
        }
        if (strstr(dfs_part_info[i].name, "userdata") ||
            !strcmp(dfs_part_info[i].name, "misc") ||
            !strcmp(dfs_part_info[i].name, "recovery") ||
            strstr(dfs_part_info[i].name, ab))
        {
            RK_OTA_INF("upgrade_fw: skip %s upgrade\n", dfs_part_info[i].name);
            continue;
        }

        if (rk_ota_source_seek(file_offset, SEEK_SET) < 0)
        {
            return OTA_STATUS_ERR;
        }

        if (firmware_offset != file_offset)
        {
            RK_OTA_ERR("%s: src offset %d != part offset %d\n", __func__, firmware_offset, dfs_part_info[i].offset);
            return OTA_STATUS_ERR;
        }

        if (!strcmp(dfs_part_info[i].name, "NIDB"))
        {
            size = size / 2;
            RK_OTA_INF("upgrade_fw: start upgrade part[%d] %s file_offset %d flash_offset 0x%x size %d ...\n",
                    i, "NIDB_a", file_offset, flash_offset, size);
            if (rk_ota_upgrade_part(i, flash_offset, size) != OTA_STATUS_OK)
            {
                RK_OTA_ERR("%s: ota_upgrade_part failed\n", __func__);
                return OTA_STATUS_ERR;
            }

            flash_offset += size;
            RK_OTA_INF("upgrade_fw: start upgrade part[%d] %s file_offset %d flash_offset 0x%x size %d ...\n",
                    i, "NIDB_b", file_offset, flash_offset, size);
            if (rk_ota_upgrade_part(i, flash_offset, size) != OTA_STATUS_OK)
            {
                RK_OTA_ERR("%s: ota_upgrade_part failed\n", __func__);
                return OTA_STATUS_ERR;
            }
        }
        else
        {
            RK_OTA_INF("upgrade_fw: start upgrade part[%d] %s file_offset %d flash_offset 0x%x size %d ...\n",
                    i, dfs_part_info[i].name, file_offset, flash_offset, size);
            if (rk_ota_upgrade_part(i, flash_offset, size) != OTA_STATUS_OK)
            {
                RK_OTA_ERR("%s: ota_upgrade_part failed\n", __func__);
                return OTA_STATUS_ERR;
            }
        }
    }

    RK_OTA_INF("upgrade_fw: success\n");
    return OTA_STATUS_OK;
}
#endif

int rk_ota_process(ota_protocol prot_type, char *url)
{
    int ret = OTA_STATUS_OK;
    rt_uint32_t recovery_flag;
    rt_uint8_t release_data[7];

    ota_prot_type = prot_type;
    firmware_offset = 0;

    fw_read_buf = RT_NULL;
    fw_read_chk_buf = RT_NULL;
    fw_rk_part_info = RT_NULL;
    rkimage_hdr = RT_NULL;

    if (prot_type == OTA_PROTOCOL_FILE && url != RT_NULL)
        RK_OTA_INF("upgrade frimware url: %s\n", url);

    /* RK_OTA_RW_SIZE >= max snand_dev->page_size */
    if (RK_OTA_RW_SIZE < 4096 || RK_OTA_INFO_SIZE < 4096)
    {
        RK_OTA_ERR("%s: RK_OTA_RW_SIZE < 4096 error\n", __func__);
        ret = OTA_STATUS_ERR;
        goto ota_process_out;
    }

    fw_read_buf = rt_dma_malloc(RK_OTA_RW_SIZE);
    if (fw_read_buf == RT_NULL)
    {
        RK_OTA_ERR("%s: malloc fw_read_buf failed\n", __func__);
        ret = OTA_STATUS_ERR;
        goto ota_process_out;
    }

    fw_read_chk_buf = rt_dma_malloc(RK_OTA_RW_SIZE);
    if (fw_read_chk_buf == RT_NULL)
    {
        RK_OTA_ERR("%s: malloc fw_read_chk_buf failed\n", __func__);
        ret = OTA_STATUS_ERR;
        goto ota_process_out;
    }

    rkimage_hdr = rt_malloc(sizeof(RKIMAGE_HDR));
    if (rkimage_hdr == RT_NULL)
    {
        RK_OTA_ERR("%s: malloc rkimage_hdr failed\n", __func__);
        ret = OTA_STATUS_ERR;
        goto ota_process_out;
    }

    if (rk_ota_get_dfs_part_info() != OTA_STATUS_OK)
    {
        RK_OTA_ERR("%s: rk_ota_get_dfs_part_info failed\n", __func__);
        ret = OTA_STATUS_ERR;
        goto ota_process_out;
    }

    ret = rk_ota_source_init(url);
    if (ret != OTA_STATUS_OK)
        goto ota_process_out;

    ret = rk_ota_part_info_chk(url);
    if (ret != OTA_STATUS_OK)
    {
        RK_OTA_ERR("%s: rk_ota_part_info_chk failed\n", __func__);
        goto ota_process_out;
    }

    ret = rk_ota_dfs_part_info_chk();
    if (ret != OTA_STATUS_OK)
    {
        RK_OTA_ERR("%s: rk_ota_dfs_part_info_chk failed\n", __func__);
        goto ota_process_out;
    }

    RK_OTA_INF("================ FW version check\n");
    if (rk_ota_fw_version_chk() == RT_FALSE)
    {
        RK_OTA_INF("%s: no need update\n", __func__);
        goto ota_process_out;
    }

#if defined(OTA_FW_USE_UPDATE_IMG) || !defined(RT_USING_SNOR)
    RK_OTA_INF("================ FW md5sum check\n");
    if (rk_ota_check_fw_md5sum(url) == RT_FALSE)
    {
        RK_OTA_INF("%s: md5sum check failed\n", __func__);
        goto ota_process_out;
    }
#endif

    RK_OTA_INF("================ FW img check pass, start upgrade ...\n");

#ifdef RT_USING_NEW_OTA_MODE_AB_RECOVERY
    RK_OTA_INF("RT_USING_NEW_OTA_MODE_AB_RECOVERY\n");
    if (rk_ota_in_recovery_fw() != RT_TRUE)
    {
        rk_ota_run_recovery_fw();
        return OTA_STATUS_OK;
    }
#elif defined(RT_USING_NEW_OTA_MODE_A_RECOVERY)
    RK_OTA_INF("RT_USING_NEW_OTA_MODE_A_RECOVERY\n");
    if (rk_ota_in_recovery_fw() != RT_TRUE)
    {
        rk_ota_run_recovery_fw();
        return OTA_STATUS_OK;
    }
#elif defined(RT_USING_NEW_OTA_MODE_AB)
    RK_OTA_INF("RT_USING_NEW_OTA_MODE_AB\n");
#endif

#if defined(RT_USING_NEW_OTA_MODE_AB) || defined(RT_BUILD_RECOVERY_FW)
#ifdef RT_USING_NEW_OTA_MODE_A_RECOVERY
    cur_running_slot = 1;
#else
    /* dump fw slot info */
    //fw_slot_info_dump();

    /* get current running fw slot index */
    ret = fw_slot_get_current_running(&cur_running_slot);
    if (ret != RT_EOK)
    {
        RK_OTA_ERR("%s: fw_slot_get_current_running failed %d\n", __func__, ret);
        ret = OTA_STATUS_ERR;
        goto ota_process_out;
    }

    RK_OTA_INF("current running fw slot %s\n", cur_running_slot == 0 ? "A" : "B");

    /* reset other fw slot flag */
    ret = fw_slot_reset_flag(cur_running_slot);
    if (ret != OTA_STATUS_OK)
    {
        RK_OTA_ERR("%s: fw_slot_reset_flag failed %d\n", __func__, ret);
        ret = OTA_STATUS_ERR;
        goto ota_process_out;
    }

    RK_OTA_INF("reset fw slot %s flag\n", cur_running_slot == 0 ? "B" : "A");
#endif

    RK_OTA_INF("start to upgrade FW %s\n", cur_running_slot == 0 ? "B" : "A");

#ifdef RT_USING_NEW_OTA_MODE_A_RECOVERY
    /* upgrade start */
    rk_ota_set_misc_recovery_flag(OTA_RECOVERY_FLAG);
#endif

#if defined(OTA_FW_USE_UPDATE_IMG) || !defined(RT_USING_SNOR)
    ret = rk_ota_upgrade_loader();
    if (ret != OTA_STATUS_OK)
    {
        RK_OTA_ERR("%s: rk_ota_upgrade_loader failed\n", __func__);
        goto ota_process_out;
    }
#endif

    ret = rk_ota_upgrade_fw();
    if (ret != OTA_STATUS_OK)
    {
        RK_OTA_ERR("%s: rk_ota_upgrade_fw failed\n", __func__);
        goto ota_process_out;
    }

#ifdef RT_USING_NEW_OTA_MODE_A_RECOVERY
    fw_slot_reset_flag(cur_running_slot);
    RK_OTA_INF("reset current running fw slot A\n");
#else
    /* set current running fw slot pending */
    fw_slot_set_pending(cur_running_slot);
    RK_OTA_INF("set current running fw slot %s pending\n", cur_running_slot == 0 ? "A" : "B");
#endif

#ifdef RT_USING_NEW_OTA_MODE_A_RECOVERY
    /* upgrade success end */
    rk_ota_set_misc_recovery_flag(0);
#endif

#if defined(OTA_FW_USE_UPDATE_IMG) || !defined(RT_USING_SNOR)
    ret = rk_ota_set_misc_fw_versioin(rkimage_head.dwVersion);
#else
    ret = rk_ota_set_misc_fw_versioin(ui_fw_ver);
#endif
    if (ret != OTA_STATUS_OK)
    {
        RK_OTA_ERR("%s: rk_ota_set_misc_fw_versioin failed\n", __func__);
    }

    RK_OTA_INF("================ upgrade complete\n");

    rk_ota_run_normal_fw();
    return ret;
#endif

ota_process_out:

    rk_ota_source_deinit();

    if (fw_read_buf != RT_NULL)
    {
        rt_free_align(fw_read_buf);
        fw_read_buf = RT_NULL;
    }

    if (fw_read_chk_buf != RT_NULL)
    {
        rt_free_align(fw_read_chk_buf);
        fw_read_chk_buf = RT_NULL;
    }

    if (fw_rk_part_info != RT_NULL)
    {
        rt_free(fw_rk_part_info);
        fw_rk_part_info = RT_NULL;
    }

    if (rkimage_hdr != RT_NULL)
    {
        rt_free(rkimage_hdr);
        rkimage_hdr = RT_NULL;
    }

    if (rk_ota_in_recovery_fw() == RT_TRUE)
    {
        rk_ota_run_normal_fw();
    }

    return ret;
}

int ota_check_firmware_exists(void)
{
    int i;
    struct stat file_stat;

    for (i = 0; i < sizeof(ota_fw_path) / sizeof(ota_fw_path[0]); i++)
    {
        char file_path[128];
#ifdef RK_OTA_PRESS_TEST
        if (fw_slot_get_current_running(&cur_running_slot) != RT_EOK)
        {
            RK_OTA_ERR("%s: fw_slot_get_current_running failed\n", __func__);
            return OTA_STATUS_ERR;
        }
        if (cur_running_slot == 0)
            snprintf(file_path, sizeof(file_path), "%s/%s", ota_fw_path[i], "update_b.img");
        else
            snprintf(file_path, sizeof(file_path), "%s/%s", ota_fw_path[i], "update_a.img");
#else
        snprintf(file_path, sizeof(file_path), "%s/%s", ota_fw_path[i], OTA_FW_FILENAME);
#endif
        if (stat(file_path, &file_stat) == 0)
        {
            if (S_ISREG(file_stat.st_mode))
            {
                RK_OTA_INF("FW found at: %s\n", file_path);
                strcpy(ota_url, file_path);
                return OTA_STATUS_OK;
            }
        }
    }

//    RK_OTA_ERR("%s not found in any specified directory.\n", OTA_FW_FILENAME);
    return OTA_STATUS_ERR;
}

static rt_bool_t udisk_plug_detect(void)
{
    rt_device_t dev_ud0;
    rt_device_t dev_disk;

    dev_ud0 = rt_device_find("ud0-0");
    if (dev_ud0 == RT_NULL)
        dev_disk = rt_device_find("udisk0");
    if (dev_ud0 != RT_NULL || dev_disk != RT_NULL)
    {
        if (udisk_inserted == 0)
        {
            RK_OTA_INF("ota udisk inserted\n");
            udisk_inserted = 1;
            return RT_TRUE;
        }
    }
    else
    {
        if (udisk_inserted == 1)
        {
            RK_OTA_INF("ota udisk removed\n");
            udisk_inserted = 0;
        }
    }

    return RT_FALSE;
}

#ifdef RK_OTA_MEM_TEST
static int ota_mem_test_init(void)
{
    int size;

    ota_file_fd = open((const char *)ota_url, O_RDONLY, 0);
    if (ota_file_fd < 0)
    {
        RK_OTA_ERR("%s: open %s failed errno %d\n", __func__, ota_url, errno);
        return OTA_STATUS_ERR;
    }

    firmware_size = lseek(ota_file_fd, 0L, SEEK_END);

    ota_mem_buf = rt_malloc(firmware_size);
    if (ota_mem_buf == RT_NULL)
    {
        RK_OTA_ERR("%s: malloc %d failed\n", __func__, firmware_size);
        return OTA_STATUS_ERR;
    }

    if (read(ota_file_fd, ota_mem_buf, firmware_size) != firmware_size)
    {
        RK_OTA_ERR("%s: read %d failed\n", __func__, firmware_size);
        rt_free(ota_mem_buf);
        ota_mem_buf = RT_NULL;
        close(ota_file_fd);
        return OTA_STATUS_ERR;
    }

    close(ota_file_fd);
    return OTA_STATUS_OK;
}
#endif

static void ota_upgrade_thr(void *parameter)
{
    while (1)
    {
        rt_thread_delay(RT_TICK_PER_SECOND);

        if (udisk_plug_detect() == RT_TRUE)
        {
            rt_thread_delay(2 * RT_TICK_PER_SECOND);
            if (ota_check_firmware_exists() == OTA_STATUS_OK)
            {
#ifdef RK_OTA_MEM_TEST
                if (ota_mem_test_init() == OTA_STATUS_OK)
                {
                    rk_ota_process(OTA_PROTOCOL_MEM, ota_mem_buf);
                    rt_free(ota_mem_buf);
                    ota_mem_buf = RT_NULL;
                }
#else
                rk_ota_process(OTA_PROTOCOL_FILE, ota_url);
#endif
            }
        }
    }
}

static int ota_upgrade_check(void)
{
    ota_url[0] = 0;

    udisk_inserted = 0;

    ota_thread = rt_thread_create("ota_upgrade",
                                  ota_upgrade_thr, RT_NULL,
                                  2048,
                                  21, 20);
    if (ota_thread != RT_NULL)
        rt_thread_startup(ota_thread);

    return 0;
}

INIT_APP_EXPORT(ota_upgrade_check);

#endif

