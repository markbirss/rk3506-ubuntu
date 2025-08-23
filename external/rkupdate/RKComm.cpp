#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "RKComm.h"
#include "RKLog.h"
#include "RKAndroidDevice.h"

CRKComm::CRKComm(CRKLog *pLog)
{
    m_log = pLog;
    m_bEmmc = false;
    m_ufs = false;
    m_hDev = m_hLbaDev = -1;
}
CRKComm::~CRKComm()
{
}

CRKUsbComm::CRKUsbComm(CRKLog *pLog): CRKComm(pLog)
{
    //char bootmode[100];
    //property_get("ro.bootmode", bootmode, "unknown");
    //if(!strcmp(bootmode, "emmc"))
    //  m_bEmmc = true;
    //else
    //  m_bEmmc = false;
    char *emmc_point = getenv(EMMC_POINT_NAME);
    m_hLbaDev = open(emmc_point, O_RDWR | O_SYNC, 0);
    if (m_hLbaDev < 0)
    {
        if (pLog)
        {
            pLog->Record(_T("INFO:is nand devices..."));
        }
        m_bEmmc = false;
    }
    else
    {
        if (pLog)
        {
            pLog->Record(_T("INFO:is emmc devices..."));
        }
        m_bEmmc = true;
        close(m_hLbaDev);
    }

    m_log = pLog;
    if(!m_bEmmc)
    {
        char param[4096];
        int fd, ret;
        char *s = NULL;
        int is_ufs = 0;

        memset(param, 0, 4096);
        fd= open("/proc/cmdline", O_RDONLY);
        ret = read(fd, (char*)param, 4096);

        s = strstr(param, "storagemedia=scsi");
        if(s != NULL)
        {
            is_ufs = 1;
        }
        else
        {
            if((strstr(param, "storagemedia=sd") != NULL) && (strstr(param, "sdfwupdate") != NULL))
            {
                printf("CRKUsbComm storagemedia=sd sdfwupdate \n");
                if((access((char*)UFS_SDA_NAME, F_OK) == 0) && (access((char*)UFS_SDB_NAME, F_OK) == 0)
                    && (access((char*)UFS_SDC_NAME, F_OK) == 0) && (access((char*)UFS_SDD_NAME, F_OK) == 0))
                {
                    printf("CRKUsbComm storagemedia=sd sdfwupdate and sdX exist!\n");
                    is_ufs = 1;
                }
            }
            else if((strstr(param,"storagemedia=usb") != NULL) && (strstr(param,"usbfwupdate") != NULL))
            {
                printf("CRKUsbComm storagemedia=usb usbfwupdate \n");
                if((access((char*)UFS_SDA_NAME, F_OK) == 0) && (access((char*)UFS_SDB_NAME, F_OK) == 0)
                    && (access((char*)UFS_SDC_NAME, F_OK) == 0) && (access((char*)UFS_SDD_NAME, F_OK) == 0))
                {
                    printf("CRKUsbComm storagemedia=usb usbfwupdate and sdX exist!\n");
                    is_ufs = 1;
                }
            }
        }
        close(fd);
        printf("CRKUsbComm is_ufs=%d \n", is_ufs);
        if(is_ufs)
        {
            if (pLog)
            {
                pLog->Record(_T("INFO:is ufs devices..."));
            }
            printf("RKU_IsUfs is ufs UFS_SDA_NAME=%s \n", (char*)UFS_SDA_NAME);
            m_hLbaDev = open((char*)UFS_SDA_NAME, O_RDWR|O_SYNC, 0);
            if(m_hLbaDev < 0)
            {
                if (pLog)
                {
                    pLog->Record(_T("ERROR:CRKUsbComm-->open %s failed,err=%s"),(char*)UFS_SDA_NAME, strerror(errno));
                }
            }
            else
            {
                if (pLog)
                {
                    pLog->Record(_T("INFO:is ufs devices UFS_SDA_NAME..."));
                }
                m_ufs = true;
                long long  filelen= lseek(m_hLbaDev, 0L, SEEK_END);
                lseek(m_hLbaDev, 0L, SEEK_SET);
                printf("ufs flashSize is %lld\n", filelen);
                m_FlashSize = filelen;
                close(m_hLbaDev);
            }
        }
    }
	printf("CRKUsbComm INFO m_bEmmc=%d m_ufs=%d \n", (int)m_bEmmc, (int)m_ufs);

    if (m_bEmmc)
    {
        if (pLog)
        {
            pLog->Record(_T("INFO:CRKUsbComm-->is emmc."));
        }
        m_hDev = open(EMMC_DRIVER_DEV_VENDOR, O_RDWR, 0);
        if (m_hDev < 0)
        {
            if (pLog)
            {
                pLog->Record(_T("ERROR:CRKUsbComm-->open %s failed,err=%s"), EMMC_DRIVER_DEV_VENDOR, strerror(errno));
                pLog->Record(_T("ERROR:CRKUsbComm-->try to read %s."), EMMC_DRIVER_DEV);
            }

            m_hDev = open(EMMC_DRIVER_DEV, O_RDWR, 0);
            if (m_hDev < 0)
            {
                if (pLog)
                {
                    pLog->Record(_T("ERROR:CRKUsbComm-->open %s failed,err=%s"), EMMC_DRIVER_DEV, strerror(errno));
                    pLog->Record(_T("ERROR:CRKUsbComm-->please to check drmboot.ko."));
                }
            }
            else
            {
                if (pLog)
                {
                    pLog->Record(_T("INFO:CRKUsbComm-->%s=%d"), EMMC_DRIVER_DEV, m_hDev);
                }
            }
        }
        else
        {
            if (pLog)
            {
                pLog->Record(_T("INFO:CRKUsbComm-->%s=%d"), EMMC_DRIVER_DEV_VENDOR, m_hDev);
            }
        }
        //get EMMC_DRIVER_DEV_LBA from
        m_hLbaDev = open(emmc_point, O_RDWR | O_SYNC, 0);
        if (m_hLbaDev < 0)
        {
            if (pLog)
            {
                pLog->Record(_T("ERROR:CRKUsbComm-->open %s failed,err=%d"), emmc_point, errno);
            }
        }
        else
        {
            if (pLog)
            {
                pLog->Record(_T("INFO:CRKUsbComm-->%s=%d"), emmc_point, m_hLbaDev);
            }
        }
    }
    else if(m_ufs)
    {
        m_hDev = -1;
        printf("CRKUsbComm is ufs UFS_SDA_NAME=%s \n", (char*)UFS_SDA_NAME);
        m_hLbaDev = open((char*)UFS_SDA_NAME, O_RDWR|O_SYNC, 0);
        if(m_hLbaDev < 0)
        {
            if (pLog)
            {
                pLog->Record(_T("ERROR:CRKUsbComm-->open %s failed,err=%s"), (char*)UFS_SDA_NAME, strerror(errno));
            }
        }
        else
        {
            if (pLog)
            {
                pLog->Record(_T("INFO:CRKUsbComm UFS_SDA_NAME-->%s=%d"), (char*)UFS_SDA_NAME, m_hLbaDev);
            }
        }
    }
    else
    {
        if (pLog)
        {
            pLog->Record(_T("INFO:CRKUsbComm-->is nand."));
        }
        m_hDev = open(NAND_DRIVER_DEV_VENDOR, O_RDWR, 0);
        if (m_hDev < 0)
        {
            if (pLog)
            {
                pLog->Record(_T("ERROR:CRKUsbComm-->open %s failed,err=%d"), NAND_DRIVER_DEV_VENDOR, strerror(errno));
                pLog->Record(_T("ERROR:CRKUsbComm-->try to read from %s."), NAND_DRIVER_DEV_VENDOR);
            }
            m_hDev = open(NAND_DRIVER_DEV, O_RDWR, 0);
            if (pLog)
            {
                pLog->Record(_T("ERROR:CRKUsbComm-->open %s failed,err=%d"), NAND_DRIVER_DEV, strerror(errno));
            }
            else
            {
                if (pLog)
                {
                    pLog->Record(_T("INFO:CRKUsbComm-->%s=%d"), NAND_DRIVER_DEV, m_hDev);
                }
            }
        }
        else
        {
            if (pLog)
            {
                pLog->Record(_T("INFO:CRKUsbComm-->%s=%d"), NAND_DRIVER_DEV_VENDOR, m_hDev);
            }
        }
    }

}
void CRKUsbComm::RKU_ReopenLBAHandle()
{
    if (m_bEmmc)
    {
        return;
    }
    if (m_ufs)
    {
        return;
    }
    if (m_hLbaDev > 0)
    {
        close(m_hLbaDev);
        m_hLbaDev = -1;
    }
    //  if (m_bEmmc)
    //  {
    //      m_hLbaDev= open(EMMC_DRIVER_DEV_LBA,O_RDWR|O_SYNC,0);
    //      if (m_hLbaDev<0)
    //      {
    //          if (m_log)
    //              m_log->Record(_T("ERROR:RKU_ReopenLBAHandle-->open %s failed,err=%d"),EMMC_DRIVER_DEV_LBA,errno);
    //      }
    //      else
    //      {
    //          if (m_log)
    //              m_log->Record(_T("INFO:RKU_ReopenLBAHandle-->%s=%d"),EMMC_DRIVER_DEV_LBA,m_hLbaDev);
    //      }

    //  }
    //  else
    //  {
    m_hLbaDev = open(NAND_DRIVER_DEV_LBA, O_RDWR | O_SYNC, 0);
    if (m_hLbaDev < 0)
    {
        if (m_log)
        {
            m_log->Record(_T("ERROR:RKU_ReopenLBAHandle-->open %s failed,err=%d"), NAND_DRIVER_DEV_LBA, errno);
        }
    }
    else
    {
        if (m_log)
        {
            m_log->Record(_T("INFO:RKU_ReopenLBAHandle-->%s=%d"), NAND_DRIVER_DEV_LBA, m_hLbaDev);
        }
    }
    //  }

}
int CRKUsbComm::RKU_ShowNandLBADevice()
{
    if (m_bEmmc)
    {
        return ERR_SUCCESS;
    }
    if (m_ufs)
    {
        return ERR_SUCCESS;
    }
    BYTE blockState[64];
    memset(blockState, 0, 64);
    int iRet;
    iRet = RKU_TestBadBlock(0, 0, MAX_TEST_BLOCKS, blockState);
    if (iRet != ERR_SUCCESS)
    {
        if (m_log)
        {
            m_log->Record(_T("ERROR:RKU_ShowNandLBADevice-->RKU_TestBadBlock failed,ret=%d"), iRet);
        }
    }
    return iRet;
}

bool CRKUsbComm::RKU_IsEmmcFlash()
{
    return m_bEmmc ? true : false;
}

bool CRKUsbComm::RKU_IsUfs()
{
    if (m_bEmmc)
    {
        printf("RKU_IsUfs is_ufs = 0, is_emmc\n");
        return 0;
    }

    if (m_ufs)
    {
        printf("RKU_IsUfs is_ufs=1, m_ufs is true\n");
        return 1;
    }

    printf("RKU_IsUfs is_ufs = 0 \n");
    return 0;
}

CRKUsbComm::~CRKUsbComm()
{
    if (m_hDev > 0)
    {
        close(m_hDev);
    }
    if (m_hLbaDev > 0)
    {
        //      if (!m_bEmmc)
        //      {
        //          CtrlNandLbaRead(false);
        //          CtrlNandLbaWrite(false);
        //      }
        close(m_hLbaDev);
    }
}

int CRKUsbComm::RKU_EraseBlock(BYTE ucFlashCS, DWORD dwPos, DWORD dwCount, BYTE ucEraseType)
{
    return ERR_SUCCESS;
}
int CRKUsbComm::RKU_ReadChipInfo(BYTE *lpBuffer)
{
    return ERR_SUCCESS;
}
int CRKUsbComm::RKU_ReadFlashID(BYTE *lpBuffer)
{
    return ERR_SUCCESS;
}

void rknand_print_hex_data(char *s, unsigned int *buf, unsigned int len)
{
    unsigned int i, j, count;

    printf("%s\n", s);
    for (i = 0; i < len; i += 4)
    {
        printf("%08x %08x %08x %08x\n", buf[i], buf[i + 1], buf[i + 2], buf[i + 3]);
    }
}


int CRKUsbComm::RKU_ReadFlashInfo(BYTE *lpBuffer, UINT *puiRead)
{
    long long ret;

    #if 0   //close by chad.ma
    if (m_hDev < 0)
    {
        return ERR_DEVICE_OPEN_FAILED;
    }

    ret = ioctl(m_hDev, GET_FLASH_INFO_IO, lpBuffer);
    if (ret)
    {
        if (m_log)
        {
            m_log->Record(_T("ERROR:RKU_ReadFlashInfo ioctl failed,err=%d"), errno);
        }
        return ERR_FAILED;
    }
    *puiRead = 11;
    #else
    /////////////////////////////////////////////////////////////////
    // get flashsize directly
    m_log->Record(_T("INFO: m_bEmmc = %d, m_hLbaDev = %d"), m_bEmmc, m_hLbaDev);

    if (m_hLbaDev < 0)
    {
        m_hLbaDev = open(NAND_DRIVER_DEV_LBA, O_RDWR | O_SYNC, 0);
        if (m_hLbaDev < 0)
        {
            if (m_log)
            {
                m_log->Record(_T("ERROR:RKU_ReadFlashInfo-->open %s failed,err=%d"), NAND_DRIVER_DEV_LBA, errno);
            }
            return ERR_FAILED;
        }
        else
        {
            if (m_log)
            {
                m_log->Record(_T("INFO:RKU_ReadFlashInfo-->open %s ok,handle=%d"), NAND_DRIVER_DEV_LBA, m_hLbaDev);
            }

            ret = lseek64(m_hLbaDev, 0, SEEK_END);

            if (ret < 0)
            {
                if (m_log)
                {
                    m_log->Record(_T("ERROR:RKU_ReadFlashInfo-->get %s file length fail"), NAND_DRIVER_DEV_LBA);
                }
                return ERR_FAILED;
            }
            else
            {
                char str[20] = {0};
                lseek64(m_hLbaDev, 0, SEEK_SET);  //reset the cfo to begin
                snprintf(str, sizeof(str), "%d", ret / 1024);
                *(UINT *)lpBuffer = (ret / 1024);
            }
        }
    }
    else
    {
        ret = lseek64(m_hLbaDev, 0, SEEK_END);
        m_log->Record(_T("INFO: lseek64 result = %lld"), ret);
        if (ret < 0)
        {
            if (m_log)
            {
                if (m_bEmmc)
                    m_log->Record(_T("ERROR:RKU_ReadFlashInfo-->get %s file length fail"),
                                  getenv(EMMC_POINT_NAME));
                else
                    m_log->Record(_T("ERROR:RKU_ReadFlashInfo-->get %s file length fail"),
                                  NAND_DRIVER_DEV_LBA);
            }
            return ERR_FAILED;
        }
        else
        {
            char str[20] = {0};
            lseek64(m_hLbaDev, 0, SEEK_SET); //reset the cfo to begin
            snprintf(str, sizeof(str), "%d", ret / 1024);
            *(UINT *)lpBuffer = (ret / 1024);
        }
    }
    #endif
    return ERR_SUCCESS;
}
int CRKUsbComm::RKU_ReadLBA(DWORD dwPos, DWORD dwCount, BYTE *lpBuffer, BYTE bySubCode)
{
    long long ret;
    long long dwPosBuf;
    if (m_hLbaDev < 0)
    {
        if (m_log)
            m_log->Record(_T("RKU_ReadLBA shouldn't been here!"));
        if (!m_bEmmc)
        {
            m_hLbaDev = open(NAND_DRIVER_DEV_LBA, O_RDWR | O_SYNC, 0);
            if (m_hLbaDev < 0)
            {
                if (m_log)
                {
                    m_log->Record(_T("ERROR:RKU_ReadLBA-->open %s failed,err=%d"), NAND_DRIVER_DEV_LBA, errno);
                }
                return ERR_DEVICE_OPEN_FAILED;
            }
            else
            {
                if (m_log)
                {
                    m_log->Record(_T("INFO:RKU_ReadLBA-->open %s ok,handle=%d"), NAND_DRIVER_DEV_LBA, m_hLbaDev);
                }
            }
        }
        else
        {
            return ERR_DEVICE_OPEN_FAILED;
        }
    }
    if (m_bEmmc && !CRKAndroidDevice::bGptFlag)
    {
        if (m_log)
            m_log->Record(_T("add----8192"));
        dwPos += 8192;
    }

    dwPosBuf = dwPos;

    ret = lseek64(m_hLbaDev, (off64_t)dwPosBuf * 512, SEEK_SET);
    if (ret < 0)
    {
        if (m_log)
        {
            m_log->Record(_T("ERROR:RKU_ReadLBA seek failed,err=%d,ret=%lld."), errno, ret);
            m_log->Record(_T("the dwPosBuf = dwPosBuf*512,dwPosBuf:%lld!"), dwPosBuf * 512);
        }
        return ERR_FAILED;
    }
    ret = read(m_hLbaDev, lpBuffer, dwCount * 512);
    if (ret != dwCount * 512)
    {
        if (m_log)
        {
            m_log->Record(_T("ERROR:RKU_ReadLBA read failed,err=%d"), errno);
        }
        return ERR_FAILED;
    }
    return ERR_SUCCESS;
}
int CRKUsbComm::RKU_ReadSector(DWORD dwPos, DWORD dwCount, BYTE *lpBuffer)
{
    int ret;
    if (m_hDev < 0)
    {
        return ERR_DEVICE_OPEN_FAILED;
    }
    DWORD *pOffsetSec = (DWORD *)(lpBuffer);
    DWORD *pCountSec = (DWORD *)(lpBuffer + 4);
    *pOffsetSec = dwPos;
    *pCountSec = dwCount;
    ret = ioctl(m_hDev, READ_SECTOR_IO, lpBuffer);
    if (ret)
    {
        if (m_log)
        {
            m_log->Record(_T("ERROR:RKU_ReadSector failed,err=%d"), errno);
        }
        return ERR_FAILED;
    }
    return ERR_SUCCESS;
}
int CRKUsbComm::RKU_ResetDevice(BYTE bySubCode)
{
    return ERR_SUCCESS;
}

int CRKUsbComm::RKU_TestBadBlock(BYTE ucFlashCS, DWORD dwPos, DWORD dwCount, BYTE *lpBuffer)
{
    int ret;
    if (m_hDev < 0)
    {
        return ERR_DEVICE_OPEN_FAILED;
    }
    ret = ioctl(m_hDev, GET_BAD_BLOCK_IO, lpBuffer);
    if (ret)
    {
        if (m_log)
        {
            m_log->Record(_T("ERROR:RKU_TestBadBlock failed,err=%d"), errno);
        }
        return ERR_FAILED;
    }
    if (m_log)
    {
        string strOutput;
        m_log->PrintBuffer(strOutput, lpBuffer, 64);
        m_log->Record(_T("INFO:BadBlockState:\r\n%s"), strOutput.c_str());
    }
    return ERR_SUCCESS;
}
int CRKUsbComm::RKU_TestDeviceReady(DWORD *dwTotal, DWORD *dwCurrent, BYTE bySubCode)
{
    return ERR_DEVICE_READY;
}
int CRKUsbComm::RKU_WriteLBA(DWORD dwPos, DWORD dwCount, BYTE *lpBuffer, BYTE bySubCode)
{
    long long ret;
    long long dwPosBuf;
    if (m_hLbaDev < 0)
    {
        if (!m_bEmmc)
        {
            m_hLbaDev = open(NAND_DRIVER_DEV_LBA, O_RDWR | O_SYNC, 0);
            if (m_hLbaDev < 0)
            {
                if (m_log)
                {
                    m_log->Record(_T("ERROR:RKU_WriteLBA-->open %s failed,err=%d"), NAND_DRIVER_DEV_LBA, errno);
                }
                return ERR_DEVICE_OPEN_FAILED;
            }
            else
            {
                if (m_log)
                {
                    m_log->Record(_T("INFO:RKU_WriteLBA-->open %s ok,handle=%d"), NAND_DRIVER_DEV_LBA, m_hLbaDev);
                }
            }
        }
        else
        {
            return ERR_DEVICE_OPEN_FAILED;
        }
    }
    if (m_bEmmc && !CRKAndroidDevice::bGptFlag)
    {
        if (m_log)
            m_log->Record(_T("add----8192"));
        dwPos += 8192;
    }

    dwPosBuf = dwPos;

    ret = lseek64(m_hLbaDev, (off64_t)dwPosBuf * 512, SEEK_SET);
    if (ret < 0)
    {
        if (m_log)
        {
            m_log->Record(_T("ERROR:RKU_WriteLBA seek failed,err=%d,ret:%lld"), errno, ret);
            m_log->Record(_T("the dwPosBuf = dwPosBuf*512,dwPosBuf:%lld!"), dwPosBuf * 512);
        }

        return ERR_FAILED;
    }

    ret = write(m_hLbaDev, lpBuffer, dwCount * 512);
    if (ret != dwCount * 512)
    {
        sleep(1);
        if (m_log)
        {
            m_log->Record(_T("ERROR:RKU_WriteLBA write failed,err=%d"), errno);
        }
        return ERR_FAILED;
    }

    return ERR_SUCCESS;
}

#define RK_LOADER_FILL_SIZE		(4096)
static unsigned char fill_4k[RK_LOADER_FILL_SIZE] = {0};

int CRKUsbComm::RKU_WriteUfsLoader(DWORD dwPos, DWORD dwCount, BYTE* lpBuffer, BYTE bySubCode)
{
    long long ret;
    long long dwPosBuf;
    int m_ufs_loader_hLbaDev;
    (void)bySubCode;

    m_ufs_loader_hLbaDev = open((char*)UFS_SDB_NAME, O_RDWR|O_SYNC, 0);
    if(m_ufs_loader_hLbaDev < 0)
    {
        printf("RKU_WriteUfsLoader Open %s failed\n", (char*)UFS_SDB_NAME);
        if (m_log)
        {
            m_log->Record(_T("ERROR:RKU_WriteUfsLoader-->open %s failed,err=%d"), (char*)UFS_SDB_NAME, errno);
        }
        return ERR_DEVICE_OPEN_FAILED;
    }
    else
    {
        printf("RKU_WriteUfsLoader Open %s Sucessful!\n", (char*)UFS_SDB_NAME);
        if (m_log)
        {
            m_log->Record(_T("INFO:RKU_WriteUfsLoader-->open %s Successful"), (char*)UFS_SDB_NAME);
        }
    }

    dwPosBuf = dwPos;
    ret = lseek64(m_ufs_loader_hLbaDev, (off64_t)dwPosBuf * 512, SEEK_SET);
    if (ret < 0)
    {
        if (m_log)
        {
            m_log->Record(_T("ERROR:RKU_WriteUfsLoader seek failed,err=%d,ret:%lld"), errno, ret);
            m_log->Record(_T("ERROR:the dwPosBuf = dwPosBuf*512,dwPosBuf:%lld!"), dwPosBuf * 512);
        }

        return ERR_FAILED;
    }
    /*1.write zero to first 4k*/
    memset(fill_4k, 0x0, RK_LOADER_FILL_SIZE);
    ret = write(m_ufs_loader_hLbaDev, fill_4k, RK_LOADER_FILL_SIZE);
    if (ret != RK_LOADER_FILL_SIZE)
    {
        sleep(1);
        if (m_log)
            m_log->Record(_T("ERROR:RKU_WriteUfsLoader write fill 4k failed,err=%d"), errno);
        return ERR_FAILED;
    }

    /*2.write remainding data*/
    ret = write(m_ufs_loader_hLbaDev, lpBuffer + RK_LOADER_FILL_SIZE, (dwCount * 512 - RK_LOADER_FILL_SIZE));
    if (ret != (dwCount * 512 - RK_LOADER_FILL_SIZE))
    {
        sleep(1);
        if (m_log)
            m_log->Record(_T("ERROR:RKU_WriteUfsLoader write remainding data failed,err=%d"), errno);
        return ERR_FAILED;
    }

    /*3.write first 4k*/
    ret = lseek64(m_ufs_loader_hLbaDev, (off64_t)dwPosBuf * 512, SEEK_SET);
    if (ret < 0)
    {
        if (m_log)
        {
            m_log->Record(_T("ERROR:RKU_WriteUfsLoader Reseek to first failed,err=%d,ret:%lld"), errno, ret);
            m_log->Record(_T("ERROR:the dwPosBuf = dwPosBuf*512,dwPosBuf:%lld!"), dwPosBuf * 512);
        }

        return ERR_FAILED;
    }
    ret = write(m_ufs_loader_hLbaDev, lpBuffer, RK_LOADER_FILL_SIZE);
    if (ret != RK_LOADER_FILL_SIZE)
    {
        sleep(1);
        if (m_log)
            m_log->Record(_T("ERROR:RKU_WriteUfsLoader write failed,err=%d"), errno);
        return ERR_FAILED;
    }

    fsync(m_ufs_loader_hLbaDev);

    if(m_ufs_loader_hLbaDev > 0)
    {
        close(m_ufs_loader_hLbaDev);
    }

    return ERR_SUCCESS;
}

int CRKUsbComm::RKU_LoaderWriteLBA(DWORD dwPos, DWORD dwCount, BYTE *lpBuffer, BYTE bySubCode)
{
    long long ret;
    long long dwPosBuf;
    if (m_hLbaDev < 0)
    {
        if (!m_bEmmc)
        {
            m_hLbaDev = open(NAND_DRIVER_DEV_LBA, O_RDWR | O_SYNC, 0);
            if (m_hLbaDev < 0)
            {
                if (m_log)
                {
                    m_log->Record(_T("ERROR:RKU_WriteLBA-->open %s failed,err=%d"), NAND_DRIVER_DEV_LBA, errno);
                }
                return ERR_DEVICE_OPEN_FAILED;
            }
            else
            {
                if (m_log)
                {
                    m_log->Record(_T("INFO:RKU_WriteLBA-->open %s ok,handle=%d"), NAND_DRIVER_DEV_LBA, m_hLbaDev);
                }
            }
        }
        else
        {
            return ERR_DEVICE_OPEN_FAILED;
        }
    }

    dwPosBuf = dwPos;
    //if (m_log)
    //    m_log->Record(_T("INFO: dwPosBuf = %d ,will seek to pos = 0x%08x"), dwPosBuf, dwPosBuf*512);

    ret = lseek64(m_hLbaDev, (off64_t)dwPosBuf * 512, SEEK_SET);
    if (ret < 0)
    {
        if (m_log)
        {
            m_log->Record(_T("ERROR:RKU_WriteLBA seek failed,err=%d,ret:%lld"), errno, ret);
            m_log->Record(_T("the dwPosBuf = dwPosBuf*512,dwPosBuf:%lld!"), dwPosBuf * 512);
        }

        return ERR_FAILED;
    }

    ret = write(m_hLbaDev, lpBuffer, dwCount * 512);
    if (ret != dwCount * 512)
    {
        sleep(1);
        if (m_log)
        {
            m_log->Record(_T("ERROR:RKU_WriteLBA write failed,err=%d"), errno);
        }
        return ERR_FAILED;
    }

    return ERR_SUCCESS;
}

int CRKUsbComm::RKU_WriteSector(DWORD dwPos, DWORD dwCount, BYTE *lpBuffer)
{
    int ret;
    if (m_hDev < 0)
    {
        return ERR_DEVICE_OPEN_FAILED;
    }
    DWORD *pOffset = (DWORD *)(lpBuffer);
    DWORD *pCount = (DWORD *)(lpBuffer + 4);
    *pOffset = dwPos;
    *pCount = dwCount;
    ret = ioctl(m_hDev, WRITE_SECTOR_IO, lpBuffer);
    if (ret)
    {
        if (m_log)
        {
            m_log->Record(_T("ERROR:RKU_WriteSector failed,err=%d"), errno);
        }
        return ERR_FAILED;
    }
    return ERR_SUCCESS;
}

int CRKUsbComm::RKU_EndWriteSector(BYTE *lpBuffer)
{
    int ret;
    if (m_hDev < 0)
    {
        return ERR_DEVICE_OPEN_FAILED;
    }
    ret = ioctl(m_hDev, END_WRITE_SECTOR_IO, lpBuffer);
    if (ret)
    {
        if (m_log)
        {
            m_log->Record(_T("ERROR:RKU_EndWriteSector failed,err=%d"), errno);
        }
        return ERR_FAILED;
    }
    return ERR_SUCCESS;
}
int CRKUsbComm::RKU_GetLockFlag(BYTE *lpBuffer)
{
    int ret;
    if (m_hDev < 0)
    {
        return ERR_DEVICE_OPEN_FAILED;
    }
    ret = ioctl(m_hDev, GET_LOCK_FLAG_IO, lpBuffer);
    if (ret)
    {
        if (m_log)
        {
            m_log->Record(_T("ERROR:RKU_GetLockFlag failed,err=%d"), errno);
        }
        return ERR_FAILED;
    }
    DWORD *pFlag = (DWORD *)lpBuffer;
    if (m_log)
    {
        m_log->Record(_T("INFO:LockFlag:0x%08x"), *pFlag);
    }
    return ERR_SUCCESS;
}
int CRKUsbComm::RKU_GetPublicKey(BYTE *lpBuffer)
{
    int ret;
    if (m_hDev < 0)
    {
        return ERR_DEVICE_OPEN_FAILED;
    }
    ret = ioctl(m_hDev, GET_PUBLIC_KEY_IO, lpBuffer);
    if (ret)
    {
        if (m_log)
        {
            m_log->Record(_T("ERROR:RKU_GetPublicKey failed,err=%d"), errno);
        }
        return ERR_FAILED;
    }
    return ERR_SUCCESS;
}
bool CRKUsbComm::CtrlNandLbaWrite(bool bEnable)
{
    int ret;
    if (m_bEmmc)
    {
        return false;
    }
    if (m_ufs)
    {
        return false;
    }
    if (m_hLbaDev < 0)
    {
        return false;
    }
    if (bEnable)
    {
        ret = ioctl(m_hLbaDev, ENABLE_NAND_LBA_WRITE_IO);
    }
    else
    {
        ret = ioctl(m_hLbaDev, DISABLE_NAND_LBA_WRITE_IO);
    }
    if (ret)
    {
        if (m_log)
        {
            m_log->Record(_T("ERROR:CtrlNandLbaWrite failed,enable=%d,err=%d"), bEnable, errno);
        }
        return false;
    }

    return true;
}
bool CRKUsbComm::CtrlNandLbaRead(bool bEnable)
{
    int ret;
    if (m_bEmmc)
    {
        return false;
    }
    if (m_ufs)
    {
        return false;
    }
    if (m_hLbaDev < 0)
    {
        return false;
    }
    if (bEnable)
    {
        ret = ioctl(m_hLbaDev, ENABLE_NAND_LBA_READ_IO);
    }
    else
    {
        ret = ioctl(m_hLbaDev, DISABLE_NAND_LBA_READ_IO);
    }
    if (ret)
    {
        if (m_log)
        {
            m_log->Record(_T("ERROR:CtrlNandLbaRead failed,enable=%d,err=%d"), bEnable, errno);
        }
        return false;
    }
    return true;
}




