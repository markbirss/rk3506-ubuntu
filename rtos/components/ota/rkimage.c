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

#include "ota.h"
#include "rkimage.h"
#include "md5.h"

STRUCT_RKIMAGE_HEAD rkimage_head;

static unsigned char m_md5[32];
static int md5check_enable;
static int nMd5DataSize;

rt_bool_t checkdata(const char *dest_path, unsigned char *out_md5sum, DWORD offset, DWORD checkSize)
{
    MD5_CTX ctx;
    unsigned char md5sum[16];
    char buffer[512];
    int len = 0;
    //int fd;
    DWORD readSize = 0;
    int step = 512;

    RK_OTA_INF("checkdata md5sum ...\n");

    /*fd = open(dest_path, O_RDONLY);
    if (fd < 0) {
        RK_OTA_ERR("%s: can't open %s\n", __func__, dest_path);
        return RT_FALSE;
    }*/

    if (rk_ota_source_seek(offset, SEEK_SET) < 0)
    {
        RK_OTA_ERR("%s: lseek %d fail\n", __func__, offset);
        //close(fd);
        return RT_FALSE;
    }

    MD5_Init(&ctx);

    while (checkSize > 0)
    {
        readSize = checkSize > step ? step : checkSize;
        if (rk_ota_source_read((rt_uint8_t *)buffer, readSize) != readSize)
        {
            RK_OTA_ERR("%s: read failed\n", __func__);
            //close(fd);
            return RT_FALSE;
        }
        checkSize = checkSize - readSize;
        MD5_Update(&ctx, buffer, readSize);
        memset(buffer, 0, sizeof(buffer));
    }
    MD5_Final(md5sum, &ctx);
    //close(fd);

    rt_kprintf("new md5:");
    for (int i = 0; i < 16; i++)
    {
        rt_kprintf("%02x ", md5sum[i]);
    }
    rt_kprintf("\n");
    //change
    if (out_md5sum != NULL)
    {
        memset(out_md5sum, 0, 16);
        memcpy(out_md5sum, md5sum, 16);
    }
    RK_OTA_INF("MD5Check is ok of %s\n", dest_path);
    return RT_TRUE;
}

rt_bool_t rk_ota_check_fw_md5sum(const char *dest_path)
{
    unsigned char md5sum[16];
    unsigned char tmp[16][2] =
    {
        { 0x30, 0x00 },
        { 0x31, 0x01 },
        { 0x32, 0x02 },
        { 0x33, 0x03 },
        { 0x34, 0x04 },
        { 0x35, 0x05 },
        { 0x36, 0x06 },
        { 0x37, 0x07 },
        { 0x38, 0x08 },
        { 0x39, 0x09 },
        { 0x61, 0x0a },
        { 0x62, 0x0b },
        { 0x63, 0x0c },
        { 0x64, 0x0d },
        { 0x65, 0x0e },
        { 0x66, 0x0f },
    };
    DWORD checkSize;
    DWORD offset = 0;

    if (!md5check_enable)
    {
        RK_OTA_INF("skip md5 check\n");
        return RT_TRUE;
    }

    checkSize = rk_ota_source_seek(0L, SEEK_END);

    checkdata(dest_path, md5sum, offset, checkSize - nMd5DataSize);

    for (int i = 0; i < 32; i = i + 2)
    {
        for (int j = 0; j < 16; j++)
        {
            if (tmp[j][1] == (md5sum[i / 2] >> 4))
            {
                if (m_md5[i] != tmp[j][0])
                {
                    RK_OTA_ERR("MD5Check is error of %s\n", dest_path);
                    return RT_FALSE;
                }
            }
            if (tmp[j][1] == (md5sum[i / 2] & 0x0f))
            {
                if (m_md5[i + 1] != tmp[j][0])
                {
                    RK_OTA_ERR("MD5Check is error of %s\n", dest_path);
                    return RT_FALSE;
                }
            }
        }
    }
    return RT_TRUE;
}

static void display_head(PSTRUCT_RKIMAGE_HEAD pHead)
{
    RK_OTA_INF("uiTag = 0x%x.\n", pHead->uiTag);
    RK_OTA_INF("usSize = 0x%x.\n", pHead->usSize);
    RK_OTA_INF("dwVersion = 0x%x.\n", pHead->dwVersion);
    UINT btMajor = ((pHead->dwVersion) & 0XFF000000) >> 24;
    UINT btMinor = ((pHead->dwVersion) & 0X00FF0000) >> 16;
    UINT usSmall = ((pHead->dwVersion) & 0x0000FFFF);
    RK_OTA_INF("firmware version: %d.%d.%d.\n", btMajor, btMinor, usSmall);
    RK_OTA_INF("release time: %d:%02d:%02d %02d:%02d:%02d\n", pHead->stReleaseTime.usYear, pHead->stReleaseTime.ucMonth,
               pHead->stReleaseTime.ucDay, pHead->stReleaseTime.ucHour,
               pHead->stReleaseTime.ucMinute, pHead->stReleaseTime.ucSecond);
    RK_OTA_INF("dwBootOffset = 0x%x.\n", pHead->dwBootOffset);
    RK_OTA_INF("dwBootSize = 0x%x.\n", pHead->dwBootSize);
    RK_OTA_INF("dwFWOffset = 0x%x.\n", pHead->dwFWOffset);
    RK_OTA_INF("dwFWSize = 0x%x.\n", pHead->dwFWSize);
}

static void display_item(PRKIMAGE_ITEM pitem)
{
    //char name[PART_NAME];
    //char file[RELATIVE_PATH];
    //unsigned int offset;
    //unsigned int flash_offset;
    //unsigned int usespace;
    //unsigned int size;

    RK_OTA_INF("name = %s file = %s offset = %d flash_offset = 0x%x usespace = %d size = %d \n",
               pitem->name, pitem->file, pitem->offset, pitem->flash_offset, pitem->usespace, pitem->size);
}

static void display_hdr(PRKIMAGE_HDR phdr)
{

    //unsigned int tag;
    //unsigned int size;
    //char machine_model[MAX_MACHINE_MODEL];
    //char manufacturer[MAX_MANUFACTURER];
    //unsigned int version;
    //int item_count;
    //RKIMAGE_ITEM item[MAX_PACKAGE_FILES];

    RK_OTA_INF("tag = 0x%x size = %d machine_model = %s manufacturer = %s version = %d item = %d. \n",
               phdr->tag, phdr->size, phdr->machine_model, phdr->manufacturer, phdr->version, phdr->item_count);
    //RK_OTA_INF("================================================\n");
    for (int i = 0; i < phdr->item_count; i++)
    {
        display_item(&(phdr->item[i]));
    }
}

void adjustFileOffset(PRKIMAGE_HDR phdr, int rk_ssfw_offset, int head_offset, int loader_offset, int loader_size)
{
    int offset = rk_ssfw_offset + head_offset;
    for (int i = 0; i < phdr->item_count; i++)
    {
        if (strcmp(phdr->item[i].name, "bootloader") == 0)
        {
            phdr->item[i].offset = loader_offset;
            phdr->item[i].size = loader_size;
            continue ;
        }
        phdr->item[i].offset += offset;
    }
}

//解析固件，获得固件头部信息
static int analyticRKFWImage(const char *filepath, PRKIMAGE_HDR phdr, DWORD offset, DWORD fw_size)
{
    long long ulFwSize;
    //unsigned char m_md5[32];

    RK_OTA_INF("================ parse %s\n", filepath);

    /*int fd = open(filepath, O_RDONLY);
    if (fd < 0) {
        RK_OTA_ERR("Can't open %s\n", filepath);
        return -2;
    }*/
    rk_ota_source_seek(offset, SEEK_SET);

    //1. image 头部信息读取
    if (rk_ota_source_read((rt_uint8_t *)&rkimage_head, sizeof(STRUCT_RKIMAGE_HEAD)) != sizeof(STRUCT_RKIMAGE_HEAD))
    {
        RK_OTA_ERR("Can't read rkimage_head from %s\n(%s)\n", filepath, strerror(errno));
        //close(fd);
        return -2;
    }

    if ((rkimage_head.reserved[14] == 'H') && (rkimage_head.reserved[15] == 'I'))
    {
        ulFwSize = *((DWORD *)(&rkimage_head.reserved[16]));
        ulFwSize <<= 32;
        ulFwSize += rkimage_head.dwFWOffset;
        ulFwSize += rkimage_head.dwFWSize;
    }
    else
    {
        ulFwSize = rkimage_head.dwFWOffset + rkimage_head.dwFWSize;
    }
    rkimage_head.dwFWSize = ulFwSize - rkimage_head.dwFWOffset;
    display_head(&rkimage_head);

    //2. 固件md5 校验
    // long long fileSize;

    // fileSize = lseek(fd, 0L, SEEK_END);
    // fileSize = fw_size;
    nMd5DataSize = fw_size - ulFwSize;
    if (nMd5DataSize >= 160)
    {
        md5check_enable = 1;
        rk_ota_source_seek(ulFwSize, SEEK_SET);
        rk_ota_source_read(m_md5, 32);
    }
    else
    {
        md5check_enable = 1;
        rk_ota_source_seek(-32, SEEK_END);
        if (rk_ota_source_read(m_md5, 32) != 32)
        {
            RK_OTA_ERR("lseek failed.\n");
            //close(fd);
            return -2;
        }
    }

    /*rt_kprintf("read md5:");
    for (int i = 0; i < 32; i++)
    {
        rt_kprintf("%02x ", m_md5[i]);
    }
    rt_kprintf("\n");*/

    //3. image 地址信息读取
    if (rk_ota_source_seek(offset + rkimage_head.dwFWOffset, SEEK_SET) == -1)
    {
        RK_OTA_ERR("lseek failed.\n");
        //close(fd);
        return -2;
    }

    if (rk_ota_source_read((rt_uint8_t *)phdr, sizeof(RKIMAGE_HDR)) != sizeof(RKIMAGE_HDR))
    {
        RK_OTA_ERR("Can't read RKIMAGE_HDR from %s\n(%s)\n", filepath, strerror(errno));
        //close(fd);
        return -2;
    }

    if (phdr->tag != RKIMAGE_TAG)
    {
        RK_OTA_ERR("tag: %x\n", phdr->tag);
        RK_OTA_ERR("Invalid image\n");
        //close(fd);
        return -3;
    }

    if ((phdr->manufacturer[56] == 0x55) && (phdr->manufacturer[57] == 0x66))
    {
        USHORT *pItemRemain;
        pItemRemain = (USHORT *)(&phdr->manufacturer[58]);
        phdr->item_count += *pItemRemain;
    }

    if (rkimage_head.dwFWOffset)
    {
        adjustFileOffset(phdr, offset, rkimage_head.dwFWOffset, rkimage_head.dwBootOffset, rkimage_head.dwBootSize);
    }

    display_hdr(phdr);

    //close(fd);
#if 0
    if (rk_ota_check_fw_md5sum() != RT_TRUE)
    {
        RK_OTA_ERR("Md5Check update.img fwSize:%ld\n", fw_size - 32);
        return -1;
    }
#endif
    RK_OTA_INF("analyticImage ok.\n");
    return 0;
}

int analyticImage(char *filepath, PRKIMAGE_HDR phdr)
{
    long long ulFwSize;
    UINT rkimage_tag;
    //unsigned char m_md5[32];
    long long fileSize;

    /*int fd = open(filepath, O_RDONLY);
    if (fd < 0) {
        RK_OTA_ERR("Can't open %s\n", filepath);
        return -2;
    }*/

    rk_ota_source_seek(0, SEEK_SET);

    if (rk_ota_source_read((rt_uint8_t *)&rkimage_tag, sizeof(UINT)) != sizeof(UINT))
    {
        RK_OTA_ERR("Can't read %s\n(%s)\n", filepath, strerror(errno));
        //close(fd);
        return -2;
    }

    fileSize = rk_ota_source_seek(0L, SEEK_END);
    RK_OTA_INF("%s size %d\n", filepath, fileSize);

    //close(fd);

    if (rkimage_tag == SSFW_TAG)
    {
        RK_OTA_ERR("not support update.img format\n");
        return -2;
    }
    else if (rkimage_tag == RKFW_TAG)
    {
        int ret;
        // 非多存储固件
        ret = analyticRKFWImage(filepath, phdr, 0, fileSize);
        if (ret == 0)
        {
            return ret;
        }
        else
        {
            RK_OTA_ERR("UNKNOW FIRMWARE TAG, PLEASE MAKE SURE THE FIRMWARE IS IN RIGHT FORMAT\n");
            return -2;
        }
    }

    RK_OTA_ERR("not support update.img format\n");
    return -2;
}

// 获得Image 打包版本号
bool getImageVersion(const char *filepath, char *version, int maxLength)
{
    UINT btMajor = ((rkimage_head.dwVersion) & 0XFF000000) >> 24;
    UINT btMinor = ((rkimage_head.dwVersion) & 0x00FF0000) >> 16;
    UINT usSmall = ((rkimage_head.dwVersion) & 0x0000FFFF);

    //转换成字符串
    sprintf(version, "%d.%d.%d", btMajor, btMinor, usSmall);

    return true;
}

