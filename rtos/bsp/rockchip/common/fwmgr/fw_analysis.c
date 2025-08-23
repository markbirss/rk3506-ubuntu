/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
 */

/*
*---------------------------------------------------------------------------------------------------------------------
*
*                                                   #include define
*
*---------------------------------------------------------------------------------------------------------------------
*/
#include <rtdevice.h>
#include <rtthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <drivers/mtd_nor.h>
#include "hal_base.h"

//#include "FwUpdate.h"
#include "fw_analysis.h"

#include "rkpart.h"
#include "dma.h"
#include "drv_flash_partition.h"

#ifdef RT_USING_DFS
#include "dfs_fs.h"
#endif

#ifdef RT_USING_SPINAND
#ifdef RT_USING_MINI_FTL
#include "mini_ftl.h"
#else
#error "RT_USING_MINI_FTL must be defined when RT_USING_SPINAND is defined"
#endif
#endif

extern int rk_ota_get_misc_part_offset(void);

/*
*---------------------------------------------------------------------------------------------------------------------
*
*                                                   #define / #typedef define
*
*---------------------------------------------------------------------------------------------------------------------
*/
#define D_MODULE_CNT 40

#define RKDEVICE_SN_LEN 60
#define RKDEVICE_UID_LEN 30
#define RKDEVICE_MAC_LEN 6
#define RKDEVICE_WIFI_LEN 6
#define RKDEVICE_BT_LEN 6
#define SEC3_RESERVED_LEN 405

typedef __PACKED_STRUCT tagFLASH_IDB_SEC3
{
    rt_uint16_t  usSNSize;
    rt_uint8_t   sn[RKDEVICE_SN_LEN];
    rt_uint8_t   reserved[SEC3_RESERVED_LEN];
    rt_uint8_t   uidSize;
    rt_uint8_t   uid[RKDEVICE_UID_LEN];
    rt_uint8_t   blueToothSize;
    rt_uint8_t   blueToothAddr[RKDEVICE_BT_LEN];
    rt_uint8_t   macSize;
    rt_uint8_t   macAddr[RKDEVICE_MAC_LEN];
} FLASH_IDB_SEC3, *PFLASH_IDB_SEC3;
/*
*---------------------------------------------------------------------------------------------------------------------
*
*                                                   local variable define
*
*---------------------------------------------------------------------------------------------------------------------
*/
FIRMWARE_INF gstFwInf;
uint16_t  UpgradeTimes;
uint16_t  LoaderVer;
rt_uint32_t  SysProgDiskCapacity;
rt_uint32_t  SysProgRawDiskCapacity;
int32_t  FW1Valid, FW2Valid;
rt_uint32_t FwSysOffset;
rt_uint32_t IdbBlockOffset;
rt_sem_t FwOperSem;
rt_sem_t DbOperSem;

extern uint32_t firmware_addr1;
extern uint32_t firmware_addr2;

static fw_ab_data g_fw_ab_data;
static uint32_t g_cur_slot = -1;
static struct rt_mutex g_slot_lock;
static struct rt_mutex g_flash_lock;

#ifdef RT_USING_SNOR
static struct rt_mtd_nor_device *snor_device = RT_NULL;
#elif defined(RT_USING_SPINAND)
static struct rt_mtd_nand_device *snand_device = RT_NULL;
#elif defined(RKMCU_RK2118) && defined(RT_USING_SDIO1)
static rt_device_t emmc_device = RT_NULL;;
#endif

/*
*---------------------------------------------------------------------------------------------------------------------
*
*                                                   global variable define
*
*---------------------------------------------------------------------------------------------------------------------
*/

/*
 * Do't modify firmware version here
 * it is auto generate by gen_fw_ver.py for parameter.txt: FIRMWARE_VER: 1.0.0
 * major: firmware_version[31:24]
 * minor: firmware_version[23:16]
 * small: firmware_version[15: 0]
 */
uint32_t firmware_version = 0x01000000;

/*
*---------------------------------------------------------------------------------------------------------------------
*
*                                                   local function declare
*
*---------------------------------------------------------------------------------------------------------------------
*/

/*
*---------------------------------------------------------------------------------------------------------------------
*
*                                                   API(common) define
*
*---------------------------------------------------------------------------------------------------------------------
*/

#if defined(RKMCU_RK2118) && defined(RT_USING_SDIO1)
#define EMMC_DEVICE "sd0"
#define PART_TAB_SIZE (4096)
#define EMMC_SECTOR_SIZE (512)
#define EMMC_SECTOR (PART_TAB_SIZE / EMMC_SECTOR_SIZE)

static int fw_part_count = 0;
static struct rt_flash_partition fw_part_info[32];

int32_t get_rk_partition_emmc(void **part)
{
    int i;
    uint8_t *read_buf = RT_NULL;
    struct dfs_partition part_info;
    int count = 0;

    *part = &fw_part_info[0];
    if (fw_part_count > 0)
    {
        return fw_part_count;
    }

    read_buf = rt_malloc_align(RT_ALIGN(PART_TAB_SIZE, CACHE_LINE_SIZE),
                               CACHE_LINE_SIZE);
    if (read_buf == RT_NULL)
    {
        rt_kprintf("malloc read_buf failed\n");
        goto cleanup;
    }

    if (fw_flash_read(0, read_buf, PART_TAB_SIZE) != PART_TAB_SIZE)
    {
        rt_kprintf("emmc_read failed\n");
        goto cleanup;
    }

    for (i = 0; i < 32; i++)
    {
        memset(&part_info, 0, sizeof(struct dfs_partition));
        if (dfs_filesystem_get_partition(&part_info, read_buf, EMMC_SECTOR, i) != RT_EOK)
        {
            break;
        }

        //rt_kprintf("part[%d]: %s offset 0x%x size 0x%x\n", count, part_info.name, part_info.offset, part_info.size);
        strcpy(fw_part_info[count].name, part_info.name);
        fw_part_info[count].offset = part_info.offset << 9;
        fw_part_info[count].size = part_info.size << 9;
        count++;
    }
    fw_part_count = count;

cleanup:
    if (read_buf != RT_NULL)
        rt_free_align(read_buf);

    return count;
}
#endif

static const unsigned int iavb_crc32_tab[] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
    0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
    0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
    0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
    0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
    0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
    0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
    0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
    0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
    0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
    0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
    0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
    0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
    0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
    0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
    0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
    0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
    0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
    0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
    0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
    0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
    0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

/*
 *  A function that calculates the CRC-32 based on the table above is
 *  given below for documentation purposes. An equivalent implementation
 *  of this function that's actually used in the kernel can be found
 *  in sys/libkern.h, where it can be inlined.
 */
unsigned int iavb_crc32(unsigned int crc_in, const unsigned char* buf, int size)
{
    const unsigned char* p = buf;
    unsigned int crc;

    crc = crc_in ^ ~0U;
    while (size--)
        crc = iavb_crc32_tab[(crc ^ *p++) & 0xFF] ^ (crc >> 8);
    return crc ^ ~0U;
}

/* Converts a 32-bit unsigned integer from host to big-endian byte order. */
unsigned int avb_htobe32(unsigned int in)
{
    union {
        unsigned int word;
        unsigned char bytes[4];
    } ret;
    ret.bytes[0] = (in >> 24) & 0xff;
    ret.bytes[1] = (in >> 16) & 0xff;
    ret.bytes[2] = (in >> 8) & 0xff;
    ret.bytes[3] = in & 0xff;
    return ret.word;
}

unsigned int jshash(unsigned int hash, char *str, unsigned int len)
{
    unsigned int i    = 0;

    for (i = 0; i < len; str++, i++)
    {
        hash ^= ((hash << 5) + (*str) + (hash >> 2));
    }
    return hash;
}

unsigned int rt_fw_crc32(unsigned int hash, char *str, unsigned int len)
{
#ifdef RT_USING_AVB_LIBAVB_AB
    return avb_htobe32(iavb_crc32(hash, str, len));
#else
    return jshash(hash, str, len);
#endif
}

void DumpData(char *buf, int bufSize, int radix)
{
    int i = 0;

    rt_kprintf("\n");
    for (i = 0; i < bufSize; i++)
    {
        if ((i > 0) && ((i % 16) == 0))
            rt_kprintf("\n");

        if (radix == 10)
            rt_kprintf(" %d", buf[i]);
        else
            rt_kprintf(" %02X", buf[i]);
    }
    rt_kprintf("\n");
}

/*******************************************************************************
** Name: fw_GetBtMac
** Input:void * pBtMac
** Return: rt_err_t
** Owner:aaron.sun
** Date: 2017.11.3
** Time: 15:34:42
*******************************************************************************/
rt_err_t fw_GetBtMac(void *pBtMac)
{
    rt_err_t ret;
    char *btbuf = pBtMac;
    ret = get_device_sn(DEV_BT_MAC, btbuf, 127);
    if (ret > 0)
        return ret;

    return RT_EOK;
}

/*******************************************************************************
** Name: fw_GetWifiMac
** Input:void * pBtMac
** Return: rt_err_t
** Owner:aaron.sun
** Date: 2017.11.3
** Time: 15:34:42
*******************************************************************************/
rt_err_t fw_GetWifiMac(void *pWifiMac)
{
    rt_err_t ret;
    ret = get_device_sn(DEV_WLAN_MAC, (char *)pWifiMac, 127);
    if (ret > 0)
        return ret;

    return RT_EOK;
}

/*******************************************************************************
** Name: fw_GetLanMac
** Input:void * pLanMac
** Return: rt_err_t
** Owner:aaron.sun
** Date: 2017.11.3
** Time: 15:34:42
*******************************************************************************/
rt_err_t fw_GetLanMac(void *pLanMac)
{
    rt_err_t ret;
    ret = get_device_sn(DEV_LAN_MAC, (char *)pLanMac, 127);
    if (ret > 0)
        return ret;

    return RT_EOK;
}

/*******************************************************************************
** Name: fw_CodePageDeInit
** Input:void
** Return: rt_err_t
** Owner:aaron.sun
** Date: 2016.9.8
** Time: 15:52:00
*******************************************************************************/
rt_err_t fw_CodePageDeInit(void)
{
    return RT_EOK;
}
/*******************************************************************************
** Name: FwResume
** Input:void
** Return: rt_err_t
** Owner:aaron.sun
** Date: 2016.8.16
** Time: 15:45:06
*******************************************************************************/
rt_err_t FwResume(void)
{
    return RT_EOK;
}

/*******************************************************************************
** Name: FwSuspend
** Input:void
** Return: rt_err_t
** Owner:aaron.sun
** Date: 2016.8.16
** Time: 15:44:39
*******************************************************************************/
rt_err_t FwSuspend(void)
{
    return RT_EOK;
}

/*******************************************************************************
** Name: FwResume
** Input:void
** Return: rt_err_t
** Owner:aaron.sun
** Date: 2016.8.16
** Time: 15:45:06
*******************************************************************************/
rt_err_t DBResume(void)
{
    return RT_EOK;
}

/*******************************************************************************
** Name: FwSuspend
** Input:void
** Return: rt_err_t
** Owner:aaron.sun
** Date: 2016.8.16
** Time: 15:44:39
*******************************************************************************/
rt_err_t DBSuspend(void)
{
    return RT_EOK;
}

/*******************************************************************************
** Name: fw_DBInit
** Input:void
** Return: rt_err_t
** Owner:aaron.sun
** Date: 2016.8.4
** Time: 10:01:13
*******************************************************************************/
rt_err_t fw_DBInit(void)
{
    return RT_EOK;
}

/*******************************************************************************
** Name: Fw_GetModle
** Input:rt_uint32_t *version,the version if current fw.
** Return: the length of current modle string.
** Owner:chad.ma
** Date: 2020.1.6
** Time: 11:48:20
*******************************************************************************/
rt_uint32_t fw_GetModle(void *modle, int fw_slot)
{
    rt_uint32_t addr;
    rt_uint32_t modle_len = 0;
    unsigned char magic[8] = {'R', 'E', 'S', 'C', 0, 0, 0, 0};
    PFIRMWARE_HEADER pFWHead1 = NULL;

    rt_uint8_t *dataBuf = NULL;
    struct rt_mtd_nor_device *snor_dev = RT_NULL;

    rt_kprintf("\n ### %s() Enter ### \n", __func__);

    snor_dev = (struct rt_mtd_nor_device *)rt_device_find("snor");
    if (snor_dev ==  RT_NULL)
    {
        rt_kprintf("Did not find device: snor....\n");
        return -RT_ERROR;
    }

    dataBuf = (rt_uint8_t *)rt_malloc(512);
    if (dataBuf == NULL)
    {
        rt_kprintf("No mem\n");
        goto END;
    }

    if (fw_slot == 0)       //FW A
        addr = firmware_addr1;
    else if (fw_slot == 1)  // FW B
        addr = firmware_addr2;
    else
        goto END;

    if (512 != rt_mtd_nor_read(snor_dev, addr, (rt_uint8_t *)dataBuf, 512))
    {
        rt_kprintf("rt_mtd_nor_read error\n");
        goto END;
    }

    pFWHead1 = (PFIRMWARE_HEADER)dataBuf;
    if (0 != rt_memcmp(pFWHead1->magic, magic, 4))
    {
        rt_kprintf("FW magic ERR addr:0x%x\n", firmware_addr1);
        goto END;
    }

    if (modle)
    {
        rt_memcpy(modle, pFWHead1->model, rt_strlen((const char *)pFWHead1->model));
        modle_len = rt_strlen((const char *)pFWHead1->model);
    }

END:
    if (dataBuf)
        rt_free(dataBuf);
    return modle_len;
}

/*******************************************************************************
** Name: fw_GetChipType
** Input:void *chipType,the buff to save current firmware chip type.
** Return: the length of chip type string
** Owner:chad.ma
** Date: 2020.1.6
** Time: 11:48:20
*******************************************************************************/
rt_uint32_t fw_GetChipType(void *chipType, int fw_slot)
{
    rt_uint32_t addr;
    rt_uint32_t chipLen = 0;
    unsigned char magic[8] = {'R', 'E', 'S', 'C', 0, 0, 0, 0};
    PFIRMWARE_HEADER pFWHead1 = NULL;

    rt_uint8_t *dataBuf = NULL;
    struct rt_mtd_nor_device *snor_dev = RT_NULL;

    rt_kprintf("\n ### %s() Enter ### \n", __func__);

    snor_dev = (struct rt_mtd_nor_device *)rt_device_find("snor");
    if (snor_dev ==  RT_NULL)
    {
        rt_kprintf("Did not find device: snor....\n");
        return -RT_ERROR;
    }

    dataBuf = rt_malloc_align(512, 64);
    if (dataBuf == NULL)
    {
        rt_kprintf("No mem\n");
        goto END;
    }

    if (fw_slot == 0)       //FW A
        addr = firmware_addr1;
    else if (fw_slot == 1)  // FW B
        addr = firmware_addr2;
    else
        goto END;
    rt_kprintf("\n ### fw slot %d, addr = 0x%#x  fw_addr = 0x%#x ### \n", fw_slot, addr,
               fw_slot == 0 ? firmware_addr1 : firmware_addr2);

    if (512 != rt_mtd_nor_read(snor_dev, addr, (uint8_t *)dataBuf, 512))
    {
        rt_kprintf("rt_mtd_nor_read error\n");
        goto END;
    }

    pFWHead1 = (PFIRMWARE_HEADER)dataBuf;
    if (0 != rt_memcmp(pFWHead1->magic, magic, 4))
    {
        rt_kprintf("FW magic ERR addr:0x%x\n", firmware_addr1);
        goto END;
    }

    if (chipType)
    {
        rt_memcpy(chipType, pFWHead1->chip, rt_strlen((const char *)pFWHead1->chip));
        chipLen = rt_strlen((const char *)pFWHead1->chip);
    }

END:
    if (dataBuf)
        rt_free_align(dataBuf);
    return chipLen;
}

/*******************************************************************************
** Name: fw_GetVersion
** Input:rt_uint32_t *version,the version if current fw.
** Return: rt_err_t
** Owner:chad.ma
** Date: 2019.7.3
** Time: 11:48:20
*******************************************************************************/
rt_err_t fw_GetVersion(rt_uint32_t *version, int fw_slot)
{
    unsigned char magic[8] = {'R', 'E', 'S', 'C', 0, 0, 0, 0};
    PFIRMWARE_HEADER pFWHead;
    rt_uint8_t *dataBuf = RT_NULL;
    rt_uint32_t addr;
    rt_uint32_t fwVersion = 0;
    struct rt_mtd_nor_device *snor_dev = RT_NULL;

    rt_kprintf("\n ### %s() Enter ### \n", __func__);

    snor_dev = (struct rt_mtd_nor_device *)rt_device_find("snor");
    if (snor_dev ==  RT_NULL)
    {
        rt_kprintf("Did not find device: snor....\n");
        return -RT_ERROR;
    }

    dataBuf = (rt_uint8_t *)rt_malloc(512);
    if (dataBuf == NULL)
    {
        rt_kprintf("No mem\n");
        goto end;
    }

    if (fw_slot == 0)       //FW A
        addr = firmware_addr1;
    else if (fw_slot == 1)  // FW B
        addr = firmware_addr2;
    else
        goto end;

    if (512 != rt_mtd_nor_read(snor_dev, addr, (uint8_t *)dataBuf, 512))
    {
        rt_kprintf("rt_mtd_nor_read error\n");
        goto end;
    }

    pFWHead = (PFIRMWARE_HEADER)dataBuf;
    if (0 != rt_memcmp(pFWHead->magic, magic, 4))
    {
        rt_kprintf("FW magic ERR addr:0x%x\n", firmware_addr1);
        goto end;
    }

    fwVersion = pFWHead->version.major << 24 |
                pFWHead->version.minor << 16 |
                pFWHead->version.small;
end:
    *version = fwVersion;
    if (dataBuf)
        rt_free(dataBuf);
    return 0;
}

/*******************************************************************************
** Name: fw_GetVersionStr
** Input:char *dest,the buff to save version of current fw.
** Return: the firmware version string length.
** Owner:chad.ma
** Date: 2020.1.6
** Time: 11:48:20
*******************************************************************************/
int fw_GetVersionStr(char *dest, int fw_slot)
{
    /*transform version = 0x1000001 to string like "1.0.0001" */
    rt_uint32_t version = 0;
    int tmp[3] = {0};
    char verStr[13] = {0};

    if (fw_slot != 0 && fw_slot != 1)
        return 0;

    fw_GetVersion(&version, fw_slot);
    tmp[0] = version >> 24 & 0xFF;
    tmp[1] = version >> 16 & 0xFF;
    tmp[2] = version       & 0xFFFF;
    rt_sprintf(verStr, "%1d%c%02d%c%04d", tmp[0], '.', tmp[1], '.', tmp[2]);
    rt_kprintf("Cur Fw Version = %s \n", verStr);
    rt_strncpy(dest, verStr, rt_strlen((const char *)verStr));
    return rt_strlen(verStr);
}

/*******************************************************************************
** Name: fw_CheckHash
** Input:fw slot which will to check.
** Return: the result of check.
** Owner:chad.ma
** Date: 2020.2.28
** Time: 11:48:20
*******************************************************************************/
int fw_CheckHash(rt_uint8_t fw_slot)
{
    unsigned char magic[8] = {'R', 'E', 'S', 'C', 0, 0, 0, 0};
    PFIRMWARE_HEADER pFWHead;
    rt_uint32_t fw_data_size, hash_data, js_hash;
    rt_uint32_t addr, fw_addr;
    rt_uint8_t *dataBuf = RT_NULL;
    struct rt_mtd_nor_device *snor_dev = RT_NULL;
    int ret = -RT_ERROR;

    rt_kprintf("\n### %s() Enter ###\n", __func__);

    snor_dev = (struct rt_mtd_nor_device *)rt_device_find("snor");
    if (snor_dev ==  RT_NULL)
    {
        rt_kprintf("Did not find device: snor....\n");
        return ret;
    }
    rt_kprintf("%s: check fw slot %d\n", __func__, fw_slot);

    dataBuf = (rt_uint8_t *)rt_malloc(4 * 1024);
    if (dataBuf == NULL)
    {
        rt_kprintf("No mem\n");
        goto end;
    }

    if (fw_slot == 0)       //FW A
        fw_addr = firmware_addr1;
    else if (fw_slot == 1)  // FW B
        fw_addr = firmware_addr2;
    else
        goto end;

    if (512 != rt_mtd_nor_read(snor_dev, fw_addr, (rt_uint8_t *)dataBuf, 512))
    {
        rt_kprintf("rt_mtd_nor_read error\n");
        goto end;
    }

    pFWHead = (PFIRMWARE_HEADER)dataBuf;
    if (0 != memcmp(pFWHead->magic, magic, 4))
    {
        rt_kprintf("FW magic ERR addr:0x%x\n", fw_addr);
        goto end;
    }

    rt_kprintf("fw_addr = %d (0x%#x)\n", fw_addr, fw_addr >> 9);
    rt_kprintf("data_size = %d\n",  pFWHead->data_size);
    rt_kprintf("data_offset = %d\n", pFWHead->data_offset);


    /*verify fireware hash*/
    hash_data = js_hash = 0;
    addr = fw_addr + pFWHead->data_offset + pFWHead->data_size;    //move to fw hash offset

    if (4 != rt_mtd_nor_read(snor_dev, addr, (rt_uint8_t *)&js_hash, 4))
    {
        rt_kprintf("rt_mtd_nor_read js hash error\n");
        goto end;
    }

    fw_data_size  = pFWHead->data_size + 512;
    addr = fw_addr;
    rt_kprintf("FW Check start:%#x, data size =%#x, js_hash =%#x\n", addr, fw_data_size, js_hash);

    while (fw_data_size)
    {
        rt_uint32_t len;
        len = MIN(fw_data_size, 4 * 1024);

        if (len != rt_mtd_nor_read(snor_dev, addr, (rt_uint8_t *)dataBuf, len))
        {
            rt_kprintf("rt_mtd_nor_read error\n");
            goto end;
        }

        hash_data = rt_fw_crc32(hash_data, (char *)dataBuf, len);
        addr += len;
        fw_data_size -= len;
    }

    rt_kprintf("FW Check End: calc hash = %#x, read js_hash = %#x\n", hash_data, js_hash);
    if (hash_data != js_hash)
    {
        rt_kprintf("FW hash ERR:0x%x, 0x%x\n", hash_data, js_hash);
        goto end;
    }

    ret = RT_EOK;
end:
    if (dataBuf)
    {
        rt_free(dataBuf);
        dataBuf = RT_NULL;
    }
    return ret;
}

/*******************************************************************************
** Name: fw_GetProductSn
** Input:void *pSn, max len must be 128
** Return: rt_err_t
** Owner:aaron.sun
** Date: 2016.4.26
** Time: 11:48:20
*******************************************************************************/
rt_err_t fw_GetProductSn(void *pSn)
{
    rt_err_t ret;
    ret = get_device_sn(DEV_SN, (char *)pSn, 127);
    if (ret > 0)
    {
        return ret;
    }
    else
    {
        return RT_EOK;
    }
}

/*******************************************************************************
** Name: fw_Resource_DeInit
** Input:void
** Return: void
** Owner:aaron.sun
** Date: 2016.3.16
** Time: 17:08:56
*******************************************************************************/
void fw_Resource_DeInit(void)
{
    rt_sem_delete(FwOperSem);
}

/*******************************************************************************
** Name: fw_GetDBInf
** Input:DATABASE_INF * pstDataBaseInf
** Return: void
** Owner:aaron.sun
** Date: 2015.10.28
** Time: 17:35:28
*******************************************************************************/
void fw_GetDBInf(DATABASE_INFO *pstDataBaseInf)
{
}

/*******************************************************************************
** Name: fw_ReadDataBaseBySector
** Input:rt_uint8_t * buf,  rt_uint32_t SecCnt
** Return: rt_err_t
** Owner:aaron.sun
** Date: 2015.10.28
** Time: 16:56:13
*******************************************************************************/
rt_err_t fw_ReadDataBaseBySector(rt_uint32_t LBA, rt_uint8_t *buf,  rt_uint32_t SecCnt)
{
//    return LUNReadDB(LBA, SecCnt, buf);
    return RT_EOK;
}

/*******************************************************************************
** Name: fw_WriteDataBaseBySector
** Input:rt_uint8_t * buf, rt_uint32_t SecCnt
** Return: rt_err_t
** Owner:aaron.sun
** Date: 2015.10.28
** Time: 16:54:24
*******************************************************************************/
rt_err_t fw_WriteDataBaseBySector(rt_uint32_t LBA, rt_uint8_t *buf, rt_uint32_t SecCnt)
{
//    return LUNWriteDB(LBA, SecCnt, buf);
    return RT_EOK;
}

rt_uint8_t   FlashBuf1[1][512];
rt_uint32_t  FlashSec1[1] = {0xffffffff};
/*******************************************************************************
** Name: fw_ReadFirmwaveByByte
** Input:rt_uint32_t Addr, rt_uint8_t *pData, rt_uint32_t length
** Return: rt_err_t
** Owner:aaron.sun
** Date: 2015.10.28
** Time: 15:33:55
*******************************************************************************/
rt_err_t fw_ReadFirmwaveByByte(rt_uint32_t Addr, rt_uint8_t *pData, rt_uint32_t length)
{
    struct rt_mtd_nor_device *snor_dev = RT_NULL;

    rt_kprintf("\n ### %s() Enter ### \n", __func__);
    rt_sem_take(FwOperSem, RT_WAITING_FOREVER);

    snor_dev = (struct rt_mtd_nor_device *)rt_device_find("snor");
    if (snor_dev ==  RT_NULL)
    {
        rt_kprintf("Did not find device: snor....\n");
        return -RT_ERROR;
    }

    if (length == rt_mtd_nor_read(snor_dev, Addr, pData, length))
    {
        rt_sem_release(FwOperSem);
    }
    else
    {
        rt_sem_release(FwOperSem);
        return -RT_ERROR;
    }
    return RT_EOK;
}

/*
*---------------------------------------------------------------------------------------------------------------------
*
*                                                   local function(common) define
*
*---------------------------------------------------------------------------------------------------------------------
*/

/*
*---------------------------------------------------------------------------------------------------------------------
*
*                                                   API(init) define
*
*---------------------------------------------------------------------------------------------------------------------
*/
/*******************************************************************************
** Name: fw_Resource_Init
** Input:void
** Return: void
** Owner:aaron.sun
** Date: 2015.10.28
** Time: 15:45:24
*******************************************************************************/
void fw_Resource_Init(void)
{
    rt_uint8_t  FlashBuf[1024];
    FIRMWARE_HEADER       *pFWHead;
    FIRMWARE_INF *pFW_Resourec_addr;
    struct rt_mtd_nor_device *snor_dev = RT_NULL;

    pFW_Resourec_addr =  &gstFwInf;

    FwOperSem = rt_sem_create("operSem", 1, RT_IPC_FLAG_FIFO);
    rt_kprintf("\n ### %s() Enter ### \n", __func__);

    snor_dev = (struct rt_mtd_nor_device *)rt_device_find("snor");
    if (snor_dev ==  RT_NULL)
    {
        rt_kprintf("Did not find device: snor....\n");
        return;
    }

    if (512 == rt_mtd_nor_read(snor_dev, 0, FlashBuf, 512))
    {
        ////////////////////////////////////////////////////////////////////////////
        //read resource module address.
        pFWHead = (FIRMWARE_HEADER *)FlashBuf;

        pFW_Resourec_addr->CodeLogicAddress  =  0;

        pFW_Resourec_addr->FirmwareYear = pFWHead->release_date.year;
        pFW_Resourec_addr->FirmwareDate = pFWHead->release_date.day;
        pFW_Resourec_addr->MasterVersion = pFWHead->version.major;
        pFW_Resourec_addr->SlaveVersion = pFWHead->version.major;
        pFW_Resourec_addr->SmallVersion = pFWHead->version.small;
    }
}

/*******************************************************************************
** Name: fw_WriteProductSn
** Input:void *pSn, max len must be 128
** Return: rt_err_t
** Owner:chad.ma
** Date: 2016.4.26
** Time: 11:48:20
*******************************************************************************/
rt_err_t fw_WriteProductSn(void *pSn, int len)
{
    rt_err_t ret;
    ret = write_device_sn(DEV_SN, (char *)pSn, len);
    if (ret == RT_EOK)
    {
        return len;
    }
    else
    {
        return 0;
    }
}

/*******************************************************************************
** Name: fw_WriteLanMac
** Input:void *pLanMac, max len must be 128
** Return: rt_err_t
** Owner:chad.ma
** Date: 2016.4.26
** Time: 11:48:20
*******************************************************************************/
rt_err_t fw_WriteLanMac(void *pLanMac, int len)
{
    rt_err_t ret;
    ret = write_device_sn(DEV_LAN_MAC, (char *)pLanMac, len);
    if (ret == RT_EOK)
    {
        return len;
    }
    else
    {
        return 0;
    }
}

/*******************************************************************************
** Name: fw_GetCustom
** Input: int snTypeId, ; void *pCustom, max len must be 128
** Return: rt_err_t
** Owner:chad.ma
** Date: 2019-6-13
** Time: 16:52:51
*******************************************************************************/
rt_err_t fw_GetCustom(int snTypeId, void *pCustom, int len)
{
    rt_err_t ret;
    ret = get_device_sn(snTypeId, (char *)pCustom, len);
    if (ret > 0)
    {
        return ret;
    }
    else
    {
        return 0;
    }
}

/*******************************************************************************
** Name: fw_WriteCustom
** Input:int snTypeId, ; void *pCustom, max len must be 128
** Return: rt_err_t
** Owner:chad.ma
** Date: 2019-6-13
** Time: 16:52:51
*******************************************************************************/
rt_err_t fw_WriteCustom(int snTypeId, void *pCustom, int len)
{
    rt_err_t ret;
    ret = write_device_sn(snTypeId, (char *)pCustom, len);
    if (ret == RT_EOK)
    {
        return len;
    }
    else
    {
        return 0;
    }
}

/*******************************************************************************
** Name: fw_WriteBtMac
** Input:void *pBtMac, max len must be 128
** Return: rt_err_t
** Owner:chad.ma
** Date: 2019-6-13
** Time: 11:48:20
*******************************************************************************/
rt_err_t fw_WriteBtMac(void *pBtMac, int len)
{
    rt_err_t ret;
    ret = write_device_sn(DEV_BT_MAC, (char *)pBtMac, len);
    if (ret == RT_EOK)
    {
        return len;
    }
    else
    {
        return 0;
    }
}

int fw_flash_init_(void)
{
#ifdef RT_USING_SNOR
    if (snor_device == RT_NULL)
    {
        snor_device = (struct rt_mtd_nor_device *)rt_device_find("snor");
        if (snor_device == RT_NULL)
        {
            rt_kprintf("%s: rt_device_find snor failed\n", __func__);
            return RT_ERROR;
        }
    }
#elif defined(RT_USING_SPINAND)
    if (snand_device == RT_NULL)
    {
        snand_device = (struct rt_mtd_nand_device *)rt_device_find("spinand0");
        if (snand_device == RT_NULL)
        {
            rt_kprintf("%s: rt_device_find snand failed\n", __func__);
            return RT_ERROR;
        }
    }
#elif defined(RKMCU_RK2118) && defined(RT_USING_SDIO1)
    int retry_count = 3;
    if (emmc_device == RT_NULL)
    {
retry_open:
        emmc_device = rt_device_find(EMMC_DEVICE);
        if (emmc_device == RT_NULL)
        {
            if (retry_count--)
            {
                rt_thread_delay(1000);
                goto retry_open;
            }
            rt_kprintf("%s: rt_device_find %s failed\n", __func__, EMMC_DEVICE);
            return RT_ERROR;
        }
        if (rt_device_open(emmc_device, RT_DEVICE_FLAG_RDWR) != RT_EOK)
        {
            rt_kprintf("open %s device failed\n", EMMC_DEVICE);
            return RT_ERROR;
        }
    }
#endif

    return RT_EOK;
}

int fw_flash_init(void)
{
    static int first_time = 1;
    int ret;

    if (first_time)
    {
        first_time = 0;
        if (rt_mutex_init(&g_flash_lock, "flash_lock", RT_IPC_FLAG_FIFO) != RT_EOK)
            return -RT_ERROR;
    }

    rt_mutex_take(&g_flash_lock, RT_WAITING_FOREVER);
    ret = fw_flash_init_();
    rt_mutex_release(&g_flash_lock);

    return ret;
}

/**
 * @brief fw flash read.
 * @param offset: offset of flash in byte.
 * @param buf: buf to save read data.
 * @param length: length in byte to read.
 * @retval < 0: read error;
 *         the actual read length;
 */
int fw_flash_read(rt_uint32_t offset, rt_uint8_t *buf, rt_uint32_t length)
{
    int readed = 0;

    if (fw_flash_init() != RT_EOK)
        return -RT_EIO;

#ifdef RT_USING_SNOR
    readed = rt_mtd_nor_read(snor_device, offset, buf, length);
#elif defined(RT_USING_SPINAND)
    readed = mini_ftl_read(snand_device, buf, offset, length);
    if (readed < 0)
    {
        rt_kprintf("%s: mini_ftl_read failed %d\n", __func__, readed);
        return -RT_EIO;
    }
#elif defined(RKMCU_RK2118) && defined(RT_USING_SDIO1)
    if ((offset % EMMC_SECTOR_SIZE) || (length % EMMC_SECTOR_SIZE))
    {
        rt_kprintf("%s: offset 0x%x or length 0x%x not aligned\n", __func__, offset, length);
        return -RT_EIO;
    }
    readed = rt_device_read(emmc_device, offset / EMMC_SECTOR_SIZE, buf, length / EMMC_SECTOR_SIZE);
    if (readed != length / EMMC_SECTOR_SIZE)
    {
        rt_kprintf("%s: emmc read failed %d\n", __func__, readed);
        return -RT_EIO;
    }
    readed = length;
#endif

    return readed;
}

/**
 * @brief fw flash write.
 * @param offset: offset of flash in byte.
 * @param buf: buf to save write data.
 * @param length: length in byte to write.
 * @retval < 0: read error;
 *         the actual write length;
 */
int fw_flash_write(rt_uint32_t offset, rt_uint8_t *buf, rt_uint32_t length)
{
    int writed = 0;

    if (fw_flash_init() != RT_EOK)
        return -RT_EIO;

#ifdef RT_USING_SNOR
    writed = rt_mtd_nor_write(snor_device, offset, buf, length);
#elif defined(RT_USING_SPINAND)
    writed = mini_ftl_write(snand_device, buf, offset, length);
    if (writed < 0)
    {
        rt_kprintf("%s: mini_ftl_write failed %d\n", __func__, writed);
        return -RT_EIO;
    }
#elif defined(RKMCU_RK2118) && defined(RT_USING_SDIO1)
    if ((offset % EMMC_SECTOR_SIZE) || (length % EMMC_SECTOR_SIZE))
    {
        rt_kprintf("%s: offset 0x%x or length 0x%x not aligned\n", __func__, offset, length);
        return -RT_EIO;
    }
    writed = rt_device_write(emmc_device, offset / EMMC_SECTOR_SIZE, buf, length / EMMC_SECTOR_SIZE);
    if (writed != length / EMMC_SECTOR_SIZE)
    {
        rt_kprintf("%s: emmc write failed %d\n", __func__, writed);
        return -RT_EIO;
    }
    writed = length;
#endif

    return writed;
}

/**
 * @brief fw flash erase.
 * @param offset: offset of flash in byte.
 * @param length: length in byte to erase, need block size align.
 * @retval RT_EOK: success;
 *         RT_ERROR: fail;
 */
int fw_flash_erase(rt_uint32_t offset, rt_uint32_t length)
{
    if (fw_flash_init() != RT_EOK)
        return RT_ERROR;

#ifdef RT_USING_SNOR
    if (rt_mtd_nor_erase_block(snor_device, offset, length) != RT_EOK)
        return RT_ERROR;
#elif defined(RT_USING_SPINAND)
    if (mini_ftl_erase(snand_device, offset, length) != RT_EOK)
    {
        rt_kprintf("%s: mini_ftl_erase failed offset %x length %x\n", __func__, offset, length);
        return RT_ERROR;
    }
#elif defined(RKMCU_RK2118) && defined(RT_USING_SDIO1)
    // no need erase
#endif

    return RT_EOK;
}

void fw_flash_deinit(void)
{
#if defined(RKMCU_RK2118) && defined(RT_USING_SDIO1)
    if (emmc_device != RT_NULL)
    {
        rt_device_close(emmc_device);
        emmc_device = RT_NULL;
    }
#endif
}

int fw_flash_get_page_size(void)
{
    if (fw_flash_init() != RT_EOK)
        return 0;
#ifdef RT_USING_SNOR
    return snor_device->block_size;
#elif defined(RT_USING_SPINAND)
    return snand_device->page_size;
#elif defined(RKMCU_RK2118) && defined(RT_USING_SDIO1)
    return EMMC_SECTOR_SIZE;
#else
    return 0;
#endif
}

int fw_flash_get_block_size(void)
{
    if (fw_flash_init() != RT_EOK)
        return 0;
#ifdef RT_USING_SNOR
    return snor_device->block_size;
#elif defined(RT_USING_SPINAND)
    return snand_device->page_size * snand_device->pages_per_block;
#elif defined(RKMCU_RK2118) && defined(RT_USING_SDIO1)
    return EMMC_SECTOR_SIZE;
#else
    return 0;
#endif
}

int fw_flash_get_size(void)
{
    if (fw_flash_init() != RT_EOK)
        return 0;
#ifdef RT_USING_SNOR
    return snor_device->block_end * snor_device->block_size;
#elif defined(RT_USING_SPINAND)
    return snand_device->page_size * snand_device->pages_per_block * snand_device->block_total;
#else
    return 0;
#endif
}

rt_bool_t fw_ab_data_verify(fw_ab_data *src, fw_ab_data *dest)
{
    /* Ensure magic is correct. */
    if (rt_memcmp(src->magic, AB_MAGIC, AB_MAGIC_LEN) != 0)
    {
        rt_kprintf("Magic is incorrect.\n");
        return RT_FALSE;
    }

    rt_memcpy(dest, src, sizeof(fw_ab_data));

    /* Ensure we don't attempt to access any fields if the major version
    * is not supported.
    */
    if (dest->version_major > AB_MAJOR_VERSION)
    {
        rt_kprintf("No support for given major version.\n");
        return RT_FALSE;
    }

    /* Fail if CRC32 doesn't match. */
    if (dest->jshash !=
            rt_fw_crc32(0, (char *)dest, sizeof(fw_ab_data) - sizeof(uint32_t)))
    {
        rt_kprintf("CRC does not match.\n");
        return RT_FALSE;
    }

    return RT_TRUE;
}

void fw_ab_data_init(fw_ab_data *data)
{
    rt_memset(data, '\0', sizeof(fw_ab_data));
    rt_memcpy(data->magic, AB_MAGIC, AB_MAGIC_LEN);
    data->version_major = AB_MAJOR_VERSION;
    data->version_minor = AB_MINOR_VERSION;

    data->slots[0].priority = AB_MAX_PRIORITY;
    data->slots[0].tries_remaining = AB_MAX_TRIES_REMAINING;
    data->slots[0].successful_boot = 0;

    data->slots[1].priority = AB_MAX_PRIORITY - 1;
    data->slots[1].tries_remaining = AB_MAX_TRIES_REMAINING;
    data->slots[1].successful_boot = 0;
}

int fw_ab_data_write(const fw_ab_data *data)
{
    fw_ab_data *dest;
    rt_uint8_t *serialized = RT_NULL;
    rt_size_t write_size;
    rt_uint32_t offset;
    uint32_t size = fw_flash_get_block_size();

#ifdef RT_USING_XIP
    offset = (rt_uint32_t)(get_addr_by_part_name("misc") - XIP_MAP0_BASE0);
#else
    offset = rk_ota_get_misc_part_offset();
#endif
    if (offset == 0)
        return -RT_ERROR;

    serialized = (rt_uint8_t *)rt_dma_malloc(size);
    if (!serialized)
        return -RT_ERROR;

    if (size != fw_flash_read(offset, serialized, size))
    {
        rt_kprintf(" %s: fw_flash_read happen error\n", __func__);
        if (serialized)
            rt_free_align(serialized);
        return -RT_ERROR;
    }

    dest = (fw_ab_data *)&serialized[AB_METADATA_MISC_PARTITION_OFFSET];
    rt_memcpy(dest, data, sizeof(fw_ab_data));
    dest->jshash = rt_fw_crc32(0, (char *)dest, sizeof(fw_ab_data) - 4);

    fw_flash_erase(offset, fw_flash_get_block_size());
    write_size = fw_flash_write(offset, serialized, size);
    if (write_size != size)
    {
        rt_kprintf("Fw AB data %#d, write Error!", write_size);
        if (serialized)
            rt_free_align(serialized);
        return -RT_ERROR;
    }

    if (serialized)
        rt_free_align(serialized);

    return RT_EOK;
}

rt_bool_t fw_slot_is_bootable(fw_ab_slot_data *slot)
{
    return slot->priority > 0 &&
           (slot->successful_boot || (slot->tries_remaining > 0));
}

int fw_ab_data_read(fw_ab_data *data)
{
    char *fw_data;
    fw_ab_data *p_ab_data;
    uint32_t size = fw_flash_get_page_size() * 2; //read 2 pages
    rt_uint32_t offset;

#ifdef RT_USING_XIP
    offset = (rt_uint32_t)(get_addr_by_part_name("misc") - XIP_MAP0_BASE0);
#else
    offset = rk_ota_get_misc_part_offset();
#endif
    if (offset == 0)
        return -RT_ERROR;

    fw_data = rt_dma_malloc(size);
    if (!fw_data)
        return -RT_ERROR;

    if (size == fw_flash_read(offset, (uint8_t *)fw_data, size))
    {
        p_ab_data = (fw_ab_data *)&fw_data[AB_METADATA_MISC_PARTITION_OFFSET];

        if (!fw_ab_data_verify(p_ab_data, data))
        {
            rt_kprintf("Error validating A/B metadata from disk.\n");
            fw_ab_data_init(data);

            if (fw_data)
                rt_free_align(fw_data);
            return fw_ab_data_write(data);
        }

        if (!fw_slot_is_bootable(&data->slots[0]) && !fw_slot_is_bootable(&data->slots[1]))
        {
            rt_kprintf("Error A and B metadata unbootable.\n");
            fw_ab_data_init(data);

            if (fw_data)
                rt_free_align(fw_data);
            return fw_ab_data_write(data);
        }
    }

    if (fw_data)
        rt_free_align(fw_data);

    return RT_EOK;
}

int fw_slot_set_default(void)
{
    fw_ab_data data;

    fw_ab_data_init(&data);
    return fw_ab_data_write(&data);
}

int fw_slot_reset_flag(uint32_t slot)
{
    fw_ab_data fw_ab;

    rt_kprintf("%s Enter...\n", __func__);
    if (fw_ab_data_read(&fw_ab) != RT_EOK)
        return -RT_ERROR;

    if (fw_ab.slots[1 - slot].successful_boot == 1)
    {
        fw_ab.slots[1 - slot].tries_remaining = AB_MAX_TRIES_REMAINING;
        fw_ab.slots[1 - slot].successful_boot = 0;

        if (fw_ab_data_write(&fw_ab) != RT_EOK)
            return -RT_ERROR;
    }

    rt_kprintf("%s Exit...\n", __func__);
    return RT_EOK;
}

int fw_slot_set_pending(uint32_t slot)
{
    fw_ab_data fw_ab;

    rt_kprintf("%s Enter...\n", __func__);
    if (fw_ab_data_read(&fw_ab) != RT_EOK)
        return -RT_ERROR;

    fw_ab.slots[slot].priority            = AB_MAX_PRIORITY - 1;
    fw_ab.slots[1 - slot].priority        = AB_MAX_PRIORITY;
    fw_ab.slots[1 - slot].tries_remaining = AB_MAX_TRIES_REMAINING;
    g_cur_slot = -1;

    if (fw_ab_data_write(&fw_ab) != RT_EOK)
        return -RT_ERROR;

    rt_kprintf("%s Exit...\n", __func__);
    return RT_EOK;
}

int fw_slot_set_active(uint32_t slot)
{
    fw_ab_data fw_ab;

    rt_kprintf("%s Enter...\n", __func__);
    if (fw_ab_data_read(&fw_ab) != RT_EOK)
        return -RT_ERROR;

    if (fw_ab.slots[slot].successful_boot == 0)
    {
        fw_ab.slots[slot].successful_boot = 1;
        fw_ab.slots[slot].tries_remaining = 0;

        if (fw_ab_data_write(&fw_ab) != RT_EOK)
            return -RT_ERROR;
    }

    rt_kprintf("%s Exit...\n", __func__);
    return RT_EOK;
}

void fw_slot_get_ab_info(char *ab)
{
    uint32_t slot = -1;

    ab[0] = 0;
    if (fw_slot_get_current_running(&slot) == RT_EOK)
    {
        if (slot == 0)
        {
            strcpy(ab, "_a");
        }
        else
        {
            strcat(ab, "_b");
        }
    }
}

int fw_slot_get_current_running_(uint32_t *cur_slot)
{
    fw_ab_data fw_ab;
    uint32_t slot_boot_idx;
    rt_int32_t ret = -RT_ERROR;

    ret = fw_ab_data_read(&fw_ab);
    if (ret != RT_EOK)
        return ret;

    memcpy(&g_fw_ab_data, &fw_ab, sizeof(fw_ab));

    if (fw_slot_is_bootable(&fw_ab.slots[0]) && fw_slot_is_bootable(&fw_ab.slots[1]))
    {
        if (fw_ab.slots[1].priority > fw_ab.slots[0].priority)
        {
            slot_boot_idx = 1;
        }
        else
        {
            slot_boot_idx = 0;
        }
    }
    else if (fw_slot_is_bootable(&fw_ab.slots[0]))
    {
        slot_boot_idx = 0;
    }
    else if (fw_slot_is_bootable(&fw_ab.slots[1]))
    {
        slot_boot_idx = 1;
    }
    else
    {
        /* No bootable slots! */
        rt_kprintf("No bootable slots found.\n");
        return -RT_ERROR;
    }

    g_cur_slot = slot_boot_idx;
    *cur_slot = slot_boot_idx;
    rt_kprintf("%s : get current slot is %d.\n", __func__, *cur_slot);
    return RT_EOK;
}

int fw_slot_get_current_running(uint32_t *cur_slot)
{
    static int first_time = 1;
    rt_int32_t ret;

    if (first_time)
    {
        first_time = 0;
        if (rt_mutex_init(&g_slot_lock, "slot_lock", RT_IPC_FLAG_FIFO) != RT_EOK)
            return -RT_ERROR;
    }

    rt_mutex_take(&g_slot_lock, RT_WAITING_FOREVER);

    if (g_cur_slot == 0 || g_cur_slot == 1)
    {
        *cur_slot = g_cur_slot;
        rt_mutex_release(&g_slot_lock);
        return RT_EOK;
    }

    ret = fw_slot_get_current_running_(cur_slot);

    rt_mutex_release(&g_slot_lock);
    return ret;
}

int fw_slot_change(uint32_t boot_slot)
{
    fw_ab_data fw_ab;

    /* if slot B boot fail, don't change it.*/
    if (g_fw_ab_data.slots[1].successful_boot == 0
            && g_fw_ab_data.slots[1].tries_remaining == 0)
    {
        rt_kprintf("The slot 1 fw seem to be damaged!\n");
        return 1;
    }

    fw_ab_data_init(&fw_ab);
    fw_ab.slots[1 - boot_slot].priority    = AB_MAX_PRIORITY - 1;
    fw_ab.slots[boot_slot].priority        = AB_MAX_PRIORITY;
    fw_ab.slots[boot_slot].tries_remaining = AB_MAX_TRIES_REMAINING;
    g_cur_slot = -1;

    if (fw_ab_data_write(&fw_ab) != RT_EOK)
    {
        rt_kprintf("write fw ab data fail!\n");
        return 1;
    }

    return 0;
}

fw_ab_data *fw_ab_data_get(void)
{
    return &g_fw_ab_data;
}

static int fw_slot_is_zero(fw_ab_slot_data *slot_data)
{
    if (slot_data->priority == 0
            && slot_data->successful_boot == 0
            && slot_data->tries_remaining == 0)
        return 1;

    return 0;
}

void fw_slot_info_dump(void)
{
    int now_slot = -1;
    rt_uint32_t slot_boot_idx = 0;
    const char *slot_name[2] = {"A", "B"};

    fw_ab_data *fw_slot_info;
#ifdef RT_SUPPORT_ROOT_AB
    user_ab_data *user_slot_info;
#endif

    rt_kprintf("%s Enter...\n", __func__);
    if (fw_slot_get_current_running(&slot_boot_idx) != RT_EOK)
        return;

    fw_slot_info = fw_ab_data_get();

    now_slot = slot_boot_idx;
    rt_kprintf("\n###########\n");
    rt_kprintf("current OS running slot is %d, OS%s\n", now_slot, slot_name[now_slot]);
    rt_kprintf("slot 0: priority:%d, tries_remaining:%d, successful_boot:%d\n",
               fw_slot_info->slots[0].priority,
               fw_slot_info->slots[0].tries_remaining,
               fw_slot_info->slots[0].successful_boot);

    rt_kprintf("slot 1: priority:%d, tries_remaining:%d, successful_boot:%d\n",
               fw_slot_info->slots[1].priority,
               fw_slot_info->slots[1].tries_remaining,
               fw_slot_info->slots[1].successful_boot);

    if (fw_slot_is_zero(&fw_slot_info->slots[0]))
        rt_kprintf("Slot 0 fw seem to be damaged\n");
    else if (fw_slot_is_zero(&fw_slot_info->slots[1]))
        rt_kprintf("Slot 1 fw seem to be damaged\n");
#ifdef RT_SUPPORT_ROOT_AB
    if (user_slot_get_current_running(&slot_boot_idx) != RT_EOK)
        return;
    user_slot_info = user_ab_data_get();
    now_slot = slot_boot_idx;

    rt_kprintf("current USER running slot is %d, root%s\n", now_slot, slot_name[now_slot]);

    rt_kprintf("root slot 0: priority:%d, tries_remaining:%d, successful_boot:%d\n",
               user_slot_info->slots[0].priority,
               user_slot_info->slots[0].tries_remaining,
               user_slot_info->slots[0].successful);

    rt_kprintf("root slot 1: priority:%d, tries_remaining:%d, successful_boot:%d\n",
               user_slot_info->slots[1].priority,
               user_slot_info->slots[1].tries_remaining,
               user_slot_info->slots[1].successful);
#endif

    rt_kprintf("###########\n\n");
}

#ifdef RT_USING_FINSH
static void ab_slot_test(int argc, char **argv)
{
    int slot;

    if (argc != 3)
    {
        rt_kprintf("Usage: ab_slot_data_test dump 0\n");
        rt_kprintf("       ab_slot_data_test change 0\n");
        rt_kprintf("       ab_slot_data_test active 0\n");
        rt_kprintf("       ab_slot_data_test pending 0\n");
        rt_kprintf("       ab_slot_data_test reset 0\n");
        return;
    }

    slot = atoi(argv[2]);
    if (slot != 0 && slot != 1)
    {
        rt_kprintf("error slot idx %d, [0 or 1]", slot);
        return;
    }

    if (!strcmp(argv[1], "dump"))
    {
        fw_slot_info_dump();
    }
    else if (!strcmp(argv[1], "change"))
    {
        fw_slot_change(slot);
    }
    else if (!strcmp(argv[1], "active"))
    {
        fw_slot_set_active(slot);
    }
    else if (!strcmp(argv[1], "pending"))
    {
        fw_slot_set_pending(slot);
    }
    else if (!strcmp(argv[1], "reset"))
    {
        fw_slot_reset_flag(slot);
    }
    else if (!strcmp(argv[1], "default"))
    {
        fw_slot_set_default();
    }
}

#include <finsh.h>
MSH_CMD_EXPORT(ab_slot_test, ab slot data test);
#endif

#ifdef RT_SUPPORT_ROOT_AB

static user_ab_data g_user_ab_data;

void user_ab_data_init(user_ab_data *data)
{
    rt_memset(data, '\0', sizeof(user_ab_data));
    rt_memcpy(data->magic, AB_MAGIC, AB_MAGIC_LEN);
    data->version_major = AB_MAJOR_VERSION;
    data->version_minor = AB_MINOR_VERSION;

    data->slots[0].priority = AB_MAX_PRIORITY;
    data->slots[0].tries_remaining = AB_MAX_TRIES_REMAINING;
    data->slots[0].successful = 1;

    data->slots[1].priority = AB_MAX_PRIORITY - 1;
    data->slots[1].tries_remaining = AB_MAX_TRIES_REMAINING;
    data->slots[1].successful = 0;

    return;
}

rt_bool_t user_ab_data_verify(user_ab_data *src, user_ab_data *dest)
{
    /* Ensure magic is correct. */
    if (rt_memcmp(src->magic, AB_MAGIC, AB_MAGIC_LEN) != 0)
    {
        rt_kprintf("USER Magic is incorrect.\n");
        return RT_FALSE;
    }

    rt_memcpy(dest, src, sizeof(user_ab_data));

    /* Ensure we don't attempt to access any fields if the major version
    * is not supported.
    */
    if (dest->version_major > AB_MAJOR_VERSION)
    {
        rt_kprintf("No support for given major version.\n");
        return RT_FALSE;
    }

    /* Bail if CRC32 doesn't match. */
    if (dest->jshash !=
            rt_fw_crc32(0, (char *)dest, sizeof(user_ab_data) - sizeof(uint32_t)))
    {
        rt_kprintf("CRC does not match.\n");
        return RT_FALSE;
    }

    return RT_TRUE;
}

int user_ab_data_write(const user_ab_data *data)
{
    user_ab_data *dest = RT_NULL;
    rt_uint8_t *serialized = RT_NULL;
    rt_size_t write_size = 0;

    serialized = (rt_uint8_t *)rt_malloc_align(4096, 64);
    if (!serialized)
        return -RT_ERROR;

    rt_memset(serialized, 0, sizeof(serialized));

    if (4096 != fw_flash_read(USER_AB_DATA_OFFSET, serialized, 4096))
    {
        rt_kprintf(" %s: fw_flash_read happen error\n", __func__);
        if (serialized)
            rt_free_align(serialized);
        return -RT_ERROR;
    }

    dest = (user_ab_data *)&serialized[0];
    rt_memcpy(dest, data, sizeof(user_ab_data));
    dest->jshash = rt_fw_crc32(0, (char *)dest, sizeof(user_ab_data) - 4);

    fw_flash_erase(USER_AB_DATA_OFFSET, fw_flash_get_block_size());
    write_size = fw_flash_write(USER_AB_DATA_OFFSET, (const rt_uint8_t *)serialized, fw_flash_get_page_size());
    if (write_size != fw_flash_get_page_size())
    {
        rt_kprintf("User AB data %#d, write Error!\n", write_size);
        if (serialized)
            rt_free_align(serialized);
        return -RT_ERROR;
    }

    if (serialized)
        rt_free_align(serialized);

    return RT_EOK;

}

rt_bool_t user_slot_is_bootable(user_ab_slot_data *slot)
{
    return slot->priority > 0 &&
           (slot->successful || (slot->tries_remaining > 0));
}

int user_ab_data_read(user_ab_data *data)
{
    char *user_ab_buff = RT_NULL;
    uint32_t size = 512;

    user_ab_buff = rt_malloc_align(size, 64);
    if (!user_ab_buff)
        return -RT_ERROR;

    if (size == fw_flash_read(USER_AB_DATA_OFFSET, (uint8_t *)user_ab_buff, size))
    {
        if (!user_ab_data_verify((user_ab_data *)user_ab_buff, data))
        {
            rt_kprintf("Error validating USER A/B metadata from disk.\n");
            user_ab_data_init(data);

            if (user_ab_buff)
                rt_free_align(user_ab_buff);
            return user_ab_data_write(data);
        }

        if (!user_slot_is_bootable(&data->slots[0]) && !user_slot_is_bootable(&data->slots[1]))
        {
            rt_kprintf("Error A and B metadata unbootable.\n");
            user_ab_data_init(data);

            if (user_ab_buff)
                rt_free_align(user_ab_buff);
            return user_ab_data_write(data);
        }
    }

    if (user_ab_buff)
        rt_free_align(user_ab_buff);

    return RT_EOK;
}

int user_slot_reset_flag(uint32_t slot)
{
    user_ab_data user_ab;

    rt_kprintf("%s Enter...\n", __func__);
    if (user_ab_data_read(&user_ab) != RT_EOK)
        return -RT_ERROR;

    if (user_ab.slots[1 - slot].successful == 1)
    {
        user_ab.slots[1 - slot].tries_remaining = AB_MAX_TRIES_REMAINING;
        user_ab.slots[1 - slot].successful = 0;

        if (user_ab_data_write(&user_ab) != RT_EOK)
            return -RT_ERROR;
    }

    rt_kprintf("%s Exit...\n", __func__);
    return RT_EOK;
}

int user_slot_set_pending(uint32_t slot)
{
    user_ab_data user_ab;

    rt_kprintf("%s Enter...\n", __func__);
    if (user_ab_data_read(&user_ab) != RT_EOK)
        return -RT_ERROR;

    user_ab.slots[slot].priority = AB_MAX_PRIORITY - 1;
    user_ab.slots[1 - slot].priority = AB_MAX_PRIORITY;
    user_ab.slots[1 - slot].tries_remaining = AB_MAX_TRIES_REMAINING;

    if (user_ab_data_write(&user_ab) != RT_EOK)
        return -RT_ERROR;

    rt_kprintf("%s Exit...\n", __func__);
    return RT_EOK;
}

int user_slot_set_active(uint32_t slot)
{
    user_ab_data user_ab;

    rt_kprintf("%s Enter...", __func__);
    if (user_ab_data_read(&user_ab) != RT_EOK)
        return -RT_ERROR;

    if (user_ab.slots[slot].successful == 0)
    {
        user_ab.slots[slot].successful = 1;
        user_ab.slots[slot].tries_remaining = 0;

        if (user_ab_data_write(&user_ab) != RT_EOK)
            return -RT_ERROR;
    }

    rt_kprintf("%s Exit...", __func__);
    return RT_EOK;
}

int user_slot_get_current_running(uint32_t *cur_slot)
{
    user_ab_data user_ab;
    uint32_t slot_curr = -1;
    rt_int32_t ret = -RT_ERROR;

    ret = user_ab_data_read(&user_ab);
    if (ret != RT_EOK)
        return ret;

    if (user_slot_is_bootable(&user_ab.slots[0]) && user_slot_is_bootable(&user_ab.slots[1]))
    {
        if (user_ab.slots[1].priority > user_ab.slots[0].priority)
        {
            slot_curr = 1;
        }
        else
        {
            slot_curr = 0;
        }
    }
    else if (user_slot_is_bootable(&user_ab.slots[0]))
    {
        slot_curr = 0;
    }
    else if (user_slot_is_bootable(&user_ab.slots[1]))
    {
        slot_curr = 1;
    }
    else
    {
        /* No bootable slots! */
        rt_kprintf("No using user slots found. Set default slot 0 using\n");
        slot_curr = 0;
    }

    *cur_slot = slot_curr;
    rt_kprintf(" %s : get current slot is %d.\n", __func__, *cur_slot);
    return RT_EOK;
}

user_ab_data *user_ab_data_get(void)
{
    return &g_user_ab_data;
}

#endif

/*
*---------------------------------------------------------------------------------------------------------------------
*
*                                                   local function(init) define
*
*---------------------------------------------------------------------------------------------------------------------
*/
