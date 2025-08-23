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

#if defined(RT_USING_NEW_OTA_MODE_AB) || defined(RT_BUILD_RECOVERY_FW)

#include "ota.h"
#include "rkimage.h"
#include "crc.h"
#include "dma.h"

USHORT m_usFlashDataSec;
USHORT m_usFlashHeadSec;
USHORT m_usFlashBootSec;
DWORD  m_dwLoaderSize;
DWORD  m_dwLoaderDataSize;
DWORD  m_dwLoaderHeadSize;
DWORD uiSecNumPerIDB;
DWORD uiFlashPageSize;
DWORD uiFlashBlockSize;
USHORT usPhyBlokcPerIDB;
rt_bool_t m_bRc4Enable;
DWORD m_idBlockOffset[IDB_BLOCKS];

DWORD m_FlashSize;
DWORD m_FlasBlockNum;
PSTRUCT_RKBOOT_HEAD pBootHead;

extern int rk_ota_get_flash_info(void *dev, DWORD *block_size, DWORD *page_size);

static rt_bool_t CheckUid(BYTE uidSize, BYTE *pUid)
{
    if (uidSize != RKDEVICE_UID_LEN)
    {
        return RT_FALSE;
    }
    USHORT oldCrc, newCrc;
    oldCrc = *(USHORT *)(pUid + RKDEVICE_UID_LEN - 2);
    newCrc = CRC_CCITT(pUid, RKDEVICE_UID_LEN - 2);
    if (oldCrc != newCrc)
    {
        return RT_FALSE;
    }
    return RT_TRUE;
}

static USHORT UshortToBCD(USHORT num)
{
    USHORT bcd = 0;
    bcd = (num % 10) | (((num / 10) % 10) << 4) | (((num / 100) % 10) << 8) | (((num / 1000) % 10) << 12);
    return bcd;
}
static BYTE ByteToBCD(BYTE num)
{
    BYTE bcd = 0;
    bcd = (num % 10) | (((num / 10) % 10) << 4);
    return bcd;
}

static void WCHAR_To_char(WCHAR *src, char *dst, int len)
{
    memset(dst, 0, len * sizeof(char));
    for (int i = 0; i < len; i++)
    {
        memcpy(dst, src, 1);
        src++;
        dst++;
    }
}

static DWORD getLoaderSizeAndData(char *loaderName, const unsigned char *data_buf, unsigned char **loaderBuffer)
{

    DWORD dwOffset;
    UCHAR ucCount, ucSize;
    DWORD dwSize = 0;

    dwOffset = pBootHead->dwLoaderEntryOffset;
    ucCount = pBootHead->ucLoaderEntryCount;
    ucSize = pBootHead->ucLoaderEntrySize;
    for (UCHAR i = 0; i < ucCount; i++)
    {
        PSTRUCT_RKBOOT_ENTRY pEntry;
        char szName[20];
        WCHAR szNameSrc[20];

        pEntry = (PSTRUCT_RKBOOT_ENTRY)(data_buf + dwOffset + (ucSize * i));

        memcpy(szNameSrc, pEntry->szName, 20 * sizeof(WCHAR));
        WCHAR_To_char(szNameSrc, szName, 20);
        if (strcmp(loaderName, szName) == 0)
        {
            RK_OTA_INF("pEntry->szName = %s.\n", szName);
            dwSize = pEntry->dwDataSize;
            *loaderBuffer = (unsigned char *)rt_malloc(dwSize);
            if (*loaderBuffer == NULL)
            {
                RK_OTA_ERR("malloc error.\n");
                RT_ASSERT(0);
            }
            memset(*loaderBuffer, 0, dwSize);
            memcpy(*loaderBuffer, data_buf + pEntry->dwDataOffset, pEntry->dwDataSize);
            RK_OTA_INF("pEntry->dwDataOffset = %d, pEntry->dwDataSize = %d.\n", pEntry->dwDataOffset, pEntry->dwDataSize);

        }
    }
    return dwSize;
}

static CHAR FindValidBlocks(char bBegin, char bLen, PBYTE pblockState)
{
    char bCount = 0;
    char bIndex = bBegin;
    int begin = bBegin;

    rt_kprintf("bBegin %d bLen %d\n", bBegin, bLen);

    while (begin < IDBLOCK_TOP)
    {
        //if(0 == m_flashInfo.blockState[bBegin++])
        if (0 == pblockState[begin++])
            ++bCount;
        else
        {
            bCount = 0;
            bIndex = begin;
        }
        if (bCount >= bLen)
            break;
    }
    if (begin >= IDBLOCK_TOP)
        bIndex = -1;

    rt_kprintf("bIndex %d begin %d bCount %d\n", bIndex, begin, bCount);
    return bIndex;
}

static bool MakeSector0(PBYTE pSector)
{
    PRKANDROID_IDB_SEC0 pSec0;
    memset(pSector, 0, SECTOR_SIZE);
    pSec0 = (PRKANDROID_IDB_SEC0)pSector;

    pSec0->dwTag = 0x0FF0AA55;
    if (m_bRc4Enable == RT_TRUE)
    {
        pSec0->uiRc4Flag = 1;
    }
    pSec0->usBootCode1Offset = 0x4;
    pSec0->usBootCode2Offset = 0x4;
    pSec0->usBootDataSize = m_usFlashDataSec;
    pSec0->usBootCodeSize = m_usFlashDataSec + m_usFlashBootSec;
    return true;
}

static void MakeSector1(PBYTE pSector)
{
    PRKANDROID_IDB_SEC1 pSec1;
    memset(pSector, 0, SECTOR_SIZE);
    pSec1 = (PRKANDROID_IDB_SEC1)pSector;
    USHORT usSysReserved;
    if ((m_idBlockOffset[4] + 1) % 12 == 0)
    {
        usSysReserved = m_idBlockOffset[4] + 13;
    }
    else
    {
        usSysReserved = ((m_idBlockOffset[4] + 1) / 12 + 1) * 12;
    }
    if (usSysReserved > IDBLOCK_TOP)
    {
        usSysReserved = IDBLOCK_TOP;
    }
    pSec1->usSysReservedBlock = usSysReserved;


    pSec1->usDisk0Size = 0;
    pSec1->usDisk1Size = 0;
    pSec1->usDisk2Size = 0;
    pSec1->usDisk3Size = 0;
    pSec1->uiChipTag = 0x38324B52;
    pSec1->uiMachineId = 0;

    pSec1->usLoaderYear = UshortToBCD(pBootHead->stReleaseTime.usYear);
    pSec1->usLoaderDate = ByteToBCD(pBootHead->stReleaseTime.ucMonth);
    pSec1->usLoaderDate = (pSec1->usLoaderDate << 8) | ByteToBCD(pBootHead->stReleaseTime.ucDay);
    pSec1->usLoaderVer =  pBootHead->dwVersion;

    pSec1->usLastLoaderVer = 0;
    pSec1->usReadWriteTimes = 1;

    pSec1->uiFlashSize = m_FlashSize * 1024;
    RK_OTA_INF("m_FlashSize * 1024 = %d.\n", m_FlashSize * 1024);
    //pSec1->usBlockSize = m_flashInfo.usBlockSize*2;
    //pSec1->bPageSize = m_flashInfo.uiPageSize*2;
    //pSec1->bECCBits = m_flashInfo.bECCBits;
    //pSec1->bAccessTime = m_flashInfo.bAccessTime;

    pSec1->usFlashInfoLen = 0;
    pSec1->usFlashInfoOffset = 0;


    pSec1->usIdBlock0 = m_idBlockOffset[0];
    pSec1->usIdBlock1 = m_idBlockOffset[1];
    pSec1->usIdBlock2 = m_idBlockOffset[2];
    pSec1->usIdBlock3 = m_idBlockOffset[3];
    pSec1->usIdBlock4 = m_idBlockOffset[4];
}

static rt_bool_t MakeSector2(PBYTE pSector)
{
    PRKANDROID_IDB_SEC2 pSec2;
    pSec2 = (PRKANDROID_IDB_SEC2)pSector;

    pSec2->usInfoSize = 0;
    memset(pSec2->bChipInfo, 0, CHIPINFO_LEN);

    memset(pSec2->reserved, 0, RKANDROID_SEC2_RESERVED_LEN);
    pSec2->usSec3CustomDataOffset = 0;//debug
    pSec2->usSec3CustomDataSize = 0;//debug

    strcpy(pSec2->szVcTag, "VC");
    strcpy(pSec2->szCrcTag, "CRC");
    return RT_TRUE;
}
static rt_bool_t MakeSector3(PBYTE pSector)
{
    //PRKANDROID_IDB_SEC3 pSec3;
    memset(pSector, 0, SECTOR_SIZE);
    //pSec3 = (PRKANDROID_IDB_SEC3)pSector;
    return RT_TRUE;
}

#if 0
static int MakeIDBlockData(PBYTE lpIDBlock, PBYTE loaderCodeBuffer, PBYTE loaderDataBuffer)
{
    int ret = 0;
    PRKANDROID_IDB_SEC0 sector0Info;
    PRKANDROID_IDB_SEC1 sector1Info;
    PRKANDROID_IDB_SEC2 sector2Info;
    PRKANDROID_IDB_SEC3 sector3Info;

    sector0Info = rt_malloc(sizeof(RKANDROID_IDB_SEC0));
    RT_ASSERT(sector0Info != RT_NULL);
    sector1Info = rt_malloc(sizeof(RKANDROID_IDB_SEC1));
    RT_ASSERT(sector1Info != RT_NULL);
    sector2Info = rt_malloc(sizeof(RKANDROID_IDB_SEC2));
    RT_ASSERT(sector2Info != RT_NULL);
    sector3Info = rt_malloc(sizeof(RKANDROID_IDB_SEC3));
    RT_ASSERT(sector3Info != RT_NULL);

    MakeSector0((PBYTE)sector0Info);
    MakeSector1((PBYTE)sector1Info);
    if (!MakeSector2((PBYTE)sector2Info))
    {
        ret = -6;
        goto make_idb_out;
    }

    if (!MakeSector3((PBYTE)sector3Info))
    {
        ret = -7;
        goto make_idb_out;
    }

    sector2Info->usSec0Crc = CRC_16((PBYTE)sector0Info, SECTOR_SIZE);
    sector2Info->usSec1Crc = CRC_16((PBYTE)sector1Info, SECTOR_SIZE);
    sector2Info->usSec3Crc = CRC_16((PBYTE)sector3Info, SECTOR_SIZE);
    memcpy(lpIDBlock, sector0Info, SECTOR_SIZE);
    memcpy(lpIDBlock + SECTOR_SIZE, sector1Info, SECTOR_SIZE);
    memcpy(lpIDBlock + SECTOR_SIZE * 3, sector3Info, SECTOR_SIZE);

    //close rc4 encryption
    if (sector0Info->uiRc4Flag)
    {
        for (int i = 0; i < m_dwLoaderDataSize / SECTOR_SIZE; i++)
        {
            P_RC4(loaderDataBuffer + SECTOR_SIZE * i, SECTOR_SIZE);
        }
        for (int i = 0; i < m_dwLoaderSize / SECTOR_SIZE; i++)
        {
            P_RC4(loaderCodeBuffer + SECTOR_SIZE * i, SECTOR_SIZE);
        }
    }
    memcpy(lpIDBlock + SECTOR_SIZE * 4, loaderDataBuffer, m_dwLoaderDataSize);
    memcpy(lpIDBlock + SECTOR_SIZE * (4 + m_usFlashDataSec), loaderCodeBuffer, m_dwLoaderSize);
    sector2Info->uiBootCodeCrc = CRC_32((PBYTE)(lpIDBlock + SECTOR_SIZE * 4), sector0Info->usBootCodeSize * SECTOR_SIZE, 0);
    memcpy(lpIDBlock + SECTOR_SIZE * 2, sector2Info, SECTOR_SIZE);
    for (int i = 0; i < 4; i++)
    {
        if (i == 1)
        {
            continue;
        }
        else
        {
            P_RC4(lpIDBlock + SECTOR_SIZE * i, SECTOR_SIZE);
        }
    }

make_idb_out:
    rt_free(sector0Info);
    rt_free(sector1Info);
    rt_free(sector2Info);
    rt_free(sector3Info);

    return ret;
}
#else
static int MakeIDBlockData(PBYTE lpIDBlock, PBYTE loaderHeadBuffer, PBYTE loaderCodeBuffer, PBYTE loaderDataBuffer)
{
    int i;

    if (m_bRc4Enable == RT_TRUE)
    {
        for (i = 0; i < m_dwLoaderHeadSize / SECTOR_SIZE; i++)
        {
            P_RC4(loaderHeadBuffer + SECTOR_SIZE * i, SECTOR_SIZE);
        }

        for (i = 0; i < m_dwLoaderDataSize / SECTOR_SIZE; i++)
        {
            P_RC4(loaderDataBuffer + SECTOR_SIZE * i, SECTOR_SIZE);
        }

        for (i = 0; i < m_dwLoaderSize / SECTOR_SIZE; i++)
        {
            P_RC4(loaderCodeBuffer + SECTOR_SIZE * i, SECTOR_SIZE);
        }
    }

    /*for (int i = 0; i < 16; i++)
    {
        rt_kprintf("%02x ", loaderHeadBuffer[i]);
    }
    rt_kprintf("\n");

    for (int i = 0; i < 16; i++)
    {
        rt_kprintf("%02x ", loaderDataBuffer[i]);
    }
    rt_kprintf("\n");

    for (int i = 0; i < 16; i++)
    {
        rt_kprintf("%02x ", loaderCodeBuffer[i]);
    }
    rt_kprintf("\n");*/

    memcpy(lpIDBlock, loaderHeadBuffer, m_dwLoaderHeadSize);
    memcpy(lpIDBlock + m_dwLoaderHeadSize, loaderDataBuffer, m_dwLoaderDataSize);
    memcpy(lpIDBlock + m_dwLoaderHeadSize + m_dwLoaderDataSize, loaderCodeBuffer, m_dwLoaderSize);

    return 0;
}
#endif

static void calcIDBCount()
{
    DWORD block_size = 0;
    DWORD page_size = 0;

    rk_ota_get_flash_info(NULL, &block_size, &page_size);

    uiSecNumPerIDB = m_usFlashHeadSec + m_usFlashDataSec + m_usFlashBootSec;
    RK_OTA_INF("uiSecNumPerIDB = %d.\n", uiSecNumPerIDB);

    /*if (block_size != 0)
    {
        usPhyBlokcPerIDB = (uiSecNumPerIDB * SECTOR_SIZE - 1) / block_size + 1;
        RT_ASSERT(usPhyBlokcPerIDB <= 2);
        RK_OTA_INF("usPhyBlokcPerIDB = %d.\n", usPhyBlokcPerIDB);
    }*/
}

static int reserveIDBlock()
{
    // 3. ReserveIDBlock
    CHAR iRet;
    char iBlockIndex = 0;
    int i;

    BYTE blockState[IDBLOCK_TOP];
    memset(m_idBlockOffset, 0, sizeof(DWORD)*IDB_BLOCKS);
    memset(blockState, 0, IDBLOCK_TOP);
    for (i = 0; i < IDB_BLOCKS; i++)
    {
        iRet = iBlockIndex = FindValidBlocks(iBlockIndex, usPhyBlokcPerIDB, blockState);
        if (iRet < 0)
        {
            RK_OTA_ERR("FindValidBlocks Error.\n");
            return -1;
        }
        m_idBlockOffset[i] = iBlockIndex;
        iBlockIndex += usPhyBlokcPerIDB;
    }

    return 0;
}

static int WriteIDBlock(PBYTE lpIDBlock, DWORD dwSectorNum, char *dest_path)
{
    DWORD offset;
    DWORD size;
    DWORD block_size = 0;
    DWORD page_size = 0;

    /*for (int i = 0; i < 16; i++)
    {
        rt_kprintf("%02x ", lpIDBlock[i]);
    }
    rt_kprintf("\n");*/

    rk_ota_get_flash_info(NULL, &block_size, &page_size);
    if (block_size == 0)
        return -1;

    /*
     * block size 128KB or 256KB；or 64K (spi nor)
     * block0: partition info
     * if idb size <= block_size
     * block1: idb copy 1st
     * block2: idb copy 2nd
     * if idb size > block_size
     * block1/2: idb copy 1st
     * block3/4: idb copy 2nd
     */

    size = dwSectorNum * SECTOR_SIZE;
#if defined(RKMCU_RK2118) && defined(RT_USING_SDIO1)
    offset = 32 * 1024;
#else
    offset = block_size;
#endif
    if (rk_ota_upgrade_loader_idb("NIDB_a", lpIDBlock, offset, size) != OTA_STATUS_OK)
    {
        return -1;
    }

#if defined(RKMCU_RK2118) && defined(RT_USING_SDIO1)
    offset += 512 * 1024;
#else
    if (size <= block_size)
    {
        offset = block_size + block_size;
    }
    else
    {
        offset = block_size + 2 * block_size;
    }
#endif
    if (rk_ota_upgrade_loader_idb("NIDB_b", lpIDBlock, offset, size) != OTA_STATUS_OK)
    {
        return -1;
    }

    return 0;
}

rt_bool_t rk_ota_download_loader(unsigned char *data_buf, int size, char *dest_path)
{
    char loaderHeadName[] = "FlashHead";
    unsigned char *loaderHeadBuffer = RT_NULL;
    char loaderDataName[] = "FlashData";
    unsigned char *loaderDataBuffer = RT_NULL;
    char loaderName[] = "FlashBoot";
    unsigned char *loaderCodeBuffer = RT_NULL;
    PBYTE pIDBData = RT_NULL;
    rt_bool_t ret = RT_TRUE;

    //generate_gf();
    //gen_poly();

    pBootHead = (PSTRUCT_RKBOOT_HEAD)(data_buf);

    if (pBootHead->uiTag != 0x544F4F42 && pBootHead->uiTag != 0x2052444C)
    {
        RK_OTA_ERR("pBootHead->uiTag!=0x544F4F42 && pBootHead->uiTag!=0x2052444C, cur is 0x%08x\n", pBootHead->uiTag);
        ret = RT_FALSE;
        goto dw_loader_out;
    }

    if (pBootHead->ucRc4Flag)
    {
        m_bRc4Enable = RT_TRUE;
    }
    else
    {
        m_bRc4Enable = RT_FALSE;
    }
    RK_OTA_INF("ucRc4Flag %d ucSignFlag %d\n", pBootHead->ucRc4Flag, pBootHead->ucSignFlag);

    m_dwLoaderHeadSize = getLoaderSizeAndData(loaderHeadName, data_buf, &loaderHeadBuffer);
    m_usFlashHeadSec = PAGEALIGN(BYTE2SECTOR(m_dwLoaderHeadSize)) * 4;
    RK_OTA_INF("m_usFlashHeadSec = %d, m_dwLoaderHeadSize = %d.\n", m_usFlashHeadSec, m_dwLoaderHeadSize);
    RT_ASSERT(m_usFlashHeadSec * SECTOR_SIZE == m_dwLoaderHeadSize);

    m_dwLoaderDataSize = getLoaderSizeAndData(loaderDataName, data_buf, &loaderDataBuffer);
    m_usFlashDataSec = PAGEALIGN(BYTE2SECTOR(m_dwLoaderDataSize)) * 4;
    RK_OTA_INF("m_usFlashDataSec = %d, m_dwLoaderDataSize = %d.\n", m_usFlashDataSec, m_dwLoaderDataSize);
    RT_ASSERT(m_usFlashDataSec * SECTOR_SIZE == m_dwLoaderDataSize);

    m_dwLoaderSize = getLoaderSizeAndData(loaderName, data_buf, &loaderCodeBuffer);
    m_usFlashBootSec = PAGEALIGN(BYTE2SECTOR(m_dwLoaderSize)) * 4;
    RK_OTA_INF("m_usFlashBootSec = %d, m_dwLoaderSize = %d.\n", m_usFlashBootSec, m_dwLoaderSize);
    RT_ASSERT(m_usFlashBootSec * SECTOR_SIZE == m_dwLoaderSize);

    calcIDBCount();

    pIDBData = (PBYTE)rt_malloc(uiSecNumPerIDB * SECTOR_SIZE);
    if (pIDBData == RT_NULL)
    {
        ret = RT_FALSE;
        goto dw_loader_out;
    }
    memset(pIDBData, 0, uiSecNumPerIDB * SECTOR_SIZE);
    if (MakeIDBlockData(pIDBData, loaderHeadBuffer, loaderCodeBuffer, loaderDataBuffer) != 0)
    {
        RK_OTA_ERR("[%s:%d] MakeIDBlockData failed.\n", __func__, __LINE__);
        ret = RT_FALSE;
        goto dw_loader_out;
    }

    if (WriteIDBlock(pIDBData, uiSecNumPerIDB, dest_path) != 0)
    {
        RK_OTA_ERR("[%s:%d] WriteIDBlock failed.\n", __func__, __LINE__);
        ret = RT_FALSE;
        goto dw_loader_out;
    }

dw_loader_out:
    if (pIDBData != RT_NULL)
        rt_free(pIDBData);
    if (loaderHeadBuffer != RT_NULL)
        rt_free(loaderHeadBuffer);
    if (loaderCodeBuffer != RT_NULL)
        rt_free(loaderCodeBuffer);
    if (loaderDataBuffer != RT_NULL)
        rt_free(loaderDataBuffer);

    return ret;
}

#endif
