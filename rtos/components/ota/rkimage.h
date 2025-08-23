/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef _RKIMAGE_H
#define _RKIMAGE_H

#include <stdbool.h>

#define DEFAULT_DOWNLOAD_PATH "/tmp/update.img"
#define BLOCK_WRITE_LEN (16 * 1024)
#define MTD_SIZE 2048
#define SECTOR_SIZE 512

#define BYTE2SECTOR(x)         ((x>0)?((x-1)/SECTOR_SIZE + 1):(x))
#define PAGEALIGN(x)           ((x>0)?((x-1) / 4 + 1):(x))

typedef unsigned char BYTE;
typedef BYTE *PBYTE;
typedef unsigned short USHORT;
typedef unsigned int    UINT;
typedef unsigned int    DWORD;
typedef unsigned char UCHAR;
typedef unsigned short WCHAR;
typedef signed char CHAR;

typedef enum {
    RKNONE_DEVICE = 0,
    RK27_DEVICE = 0x10,
    RKCAYMAN_DEVICE,
    RK28_DEVICE = 0x20,
    RK281X_DEVICE,
    RKPANDA_DEVICE,
    RKNANO_DEVICE = 0x30,
    RKSMART_DEVICE,
    RKCROWN_DEVICE = 0x40,
    RK29_DEVICE = 0x50,
    RK292X_DEVICE,
    RK30_DEVICE = 0x60,
    RK30B_DEVICE,
    RK31_DEVICE = 0x70,
    RK32_DEVICE = 0x80
} ENUM_RKDEVICE_TYPE;

typedef enum {
    ENTRY471 = 1,
    ENTRY472 = 2,
    ENTRYLOADER = 4
} ENUM_RKBOOTENTRY;

#define SHA_DIGEST_SIZE 20
#define PART_NAME 32
#define RELATIVE_PATH 64
#define MAX_PARTS 20
#define MAX_MACHINE_MODEL 64
#define MAX_MANUFACTURER 60
#define MAX_PACKAGE_FILES 32
#define RKIMAGE_TAG 0x46414B52
#define SSFW_TAG 0x57465353
#define RKFW_TAG 0x57464B52
#define IMAGE_RESERVED_SIZE 61
#define BOOT_RESERVED_SIZE 57
#define IDB_BLOCKS 5
#define IDBLOCK_TOP 50
#define CHIPINFO_LEN 16
#define RKANDROID_SEC2_RESERVED_LEN 473
#define RKDEVICE_SN_LEN 30
#define RKANDROID_SEC3_RESERVED_LEN 419
#define RKDEVICE_IMEI_LEN 15
#define RKDEVICE_UID_LEN 30
#define RKDEVICE_BT_LEN 6
#define RKDEVICE_MAC_LEN 6
#define SPARE_SIZE 16

#define GPT_BACKUP_FILE_NAME "gpt_backup.img"

typedef enum
{
    STORAGE_FLASH_CODE=1<<0,
    STORAGE_EMMC_CODE=1<<1,
    STORAGE_SD0_CODE=1<<2,
    STORAGE_SD1_CODE=1<<3,
    STORAGE_SPINOR_CODE=1<<9,
    STORAGE_SPINAND_CODE=1<<8,
    STORAGE_RAM_CODE=1<<6,
    STORAGE_USB_CODE=1<<7,
    STORAGE_SATA_CODE=1<<10,
    STORAGE_PCIE_CODE=1<<11
} STORAGE_CODE;

typedef struct __attribute__((aligned(1), packed))
{
    unsigned long long offset;
    unsigned long long size;
    UINT storage;
    UINT reserved[3];
    char file[32];
} STORAGE_ENTRY, *PSTORAGE_ENTRY;

typedef struct __attribute__((aligned(1), packed))
{
    UINT tag;
    UINT head_size;
    UINT entry_size;
    UINT dwVer;
    UINT entry_count;
    BYTE reserved[44];
} STORAGE_FW_HDR, *PSTORAGE_FW_HDR;

typedef struct __attribute__((aligned(1), packed)) {
    USHORT  usYear;
    BYTE    ucMonth;
    BYTE    ucDay;
    BYTE    ucHour;
    BYTE    ucMinute;
    BYTE    ucSecond;
} STRUCT_RKTIME, *PSTRUCT_RKTIME;

typedef struct __attribute__((aligned(1), packed)) {
    UINT uiTag;     //标志，固定为0x57 0x46 0x4B 0x52
    USHORT usSize;  //结构体大小
    DWORD  dwVersion;   //Image 文件版本
    DWORD  dwMergeVersion;  //打包工具版本
    STRUCT_RKTIME stReleaseTime;    //生成时间
    CHAR emSupportChip[4];   //使用芯片
    DWORD  dwBootOffset;    //Boot偏移
    DWORD  dwBootSize;  //Boot大小
    DWORD  dwFWOffset;  //固件偏移
    DWORD  dwFWSize;    //固件大小
    BYTE   reserved[61];    //预留空间，用于存放不同固件特征
} STRUCT_RKIMAGE_HEAD, *PSTRUCT_RKIMAGE_HEAD;

typedef struct __attribute__((aligned(1), packed)) tagRKIMAGE_ITEM {
    char name[PART_NAME];
    char file[RELATIVE_PATH];
    unsigned int offset; // file offset
    unsigned int flash_offset; // flash offset sector
    unsigned int usespace;
    unsigned int size; // actual size
} RKIMAGE_ITEM, *PRKIMAGE_ITEM;

typedef struct __attribute__((aligned(1), packed)) {
    unsigned int tag;
    unsigned int size;
    char machine_model[MAX_MACHINE_MODEL];
    char manufacturer[MAX_MANUFACTURER];
    unsigned int version;
    int item_count;
    RKIMAGE_ITEM item[MAX_PACKAGE_FILES];
} RKIMAGE_HDR, *PRKIMAGE_HDR;

typedef struct __attribute__((aligned(1), packed)) {
    UINT uiTag;
    USHORT usSize;
    DWORD  dwVersion;
    DWORD  dwMergeVersion;
    STRUCT_RKTIME stReleaseTime;
    CHAR emSupportChip[4];
    UCHAR uc471EntryCount;
    DWORD dw471EntryOffset;
    UCHAR uc471EntrySize;
    UCHAR uc472EntryCount;
    DWORD dw472EntryOffset;
    UCHAR uc472EntrySize;
    UCHAR ucLoaderEntryCount;
    DWORD dwLoaderEntryOffset;
    UCHAR ucLoaderEntrySize;
    UCHAR ucSignFlag;
    UCHAR ucRc4Flag;
    UCHAR reserved[BOOT_RESERVED_SIZE];
} STRUCT_RKBOOT_HEAD, *PSTRUCT_RKBOOT_HEAD;

typedef struct __attribute__((aligned(1), packed)) {
    UCHAR ucSize;
    DWORD emType;
    WCHAR szName[20];
    DWORD dwDataOffset;
    DWORD dwDataSize;
    DWORD dwDataDelay;//以秒为单位
} STRUCT_RKBOOT_ENTRY, *PSTRUCT_RKBOOT_ENTRY;

typedef    struct __attribute__((aligned(1), packed)) {
    DWORD    dwTag;
    BYTE    reserved[4];
    UINT    uiRc4Flag;
    USHORT    usBootCode1Offset;
    USHORT    usBootCode2Offset;
    BYTE    reserved1[490];
    USHORT  usBootDataSize;
    USHORT    usBootCodeSize;
    USHORT    usCrc;
} RKANDROID_IDB_SEC0, *PRKANDROID_IDB_SEC0;

typedef struct __attribute__((aligned(1), packed)) {
    USHORT  usSysReservedBlock;
    USHORT  usDisk0Size;
    USHORT  usDisk1Size;
    USHORT  usDisk2Size;
    USHORT  usDisk3Size;
    UINT    uiChipTag;
    UINT    uiMachineId;
    USHORT    usLoaderYear;
    USHORT    usLoaderDate;
    USHORT    usLoaderVer;
    USHORT  usLastLoaderVer;
    USHORT  usReadWriteTimes;
    DWORD    dwFwVer;
    USHORT  usMachineInfoLen;
    UCHAR    ucMachineInfo[30];
    USHORT    usManufactoryInfoLen;
    UCHAR    ucManufactoryInfo[30];
    USHORT    usFlashInfoOffset;
    USHORT    usFlashInfoLen;
    UCHAR    reserved[384];
    UINT    uiFlashSize;
    BYTE    reserved1;
    BYTE    bAccessTime;
    USHORT  usBlockSize;
    BYTE    bPageSize;
    BYTE    bECCBits;
    BYTE    reserved2[8];
    USHORT  usIdBlock0;
    USHORT  usIdBlock1;
    USHORT  usIdBlock2;
    USHORT  usIdBlock3;
    USHORT  usIdBlock4;
} RKANDROID_IDB_SEC1, *PRKANDROID_IDB_SEC1;

typedef struct __attribute__((aligned(1), packed)) {
    USHORT  usInfoSize;
    BYTE    bChipInfo[CHIPINFO_LEN];
    BYTE    reserved[RKANDROID_SEC2_RESERVED_LEN];
    char    szVcTag[3];
    USHORT  usSec0Crc;
    USHORT  usSec1Crc;
    UINT    uiBootCodeCrc;
    USHORT  usSec3CustomDataOffset;
    USHORT  usSec3CustomDataSize;
    char    szCrcTag[4];
    USHORT  usSec3Crc;
} RKANDROID_IDB_SEC2, *PRKANDROID_IDB_SEC2;

typedef struct __attribute__((aligned(1), packed)) {
    USHORT  usSNSize;
    BYTE    sn[RKDEVICE_SN_LEN];
    BYTE    reserved[RKANDROID_SEC3_RESERVED_LEN];
    BYTE    imeiSize;
    BYTE    imei[RKDEVICE_IMEI_LEN];
    BYTE    uidSize;
    BYTE    uid[RKDEVICE_UID_LEN];
    BYTE    blueToothSize;
    BYTE    blueToothAddr[RKDEVICE_BT_LEN];
    BYTE    macSize;
    BYTE    macAddr[RKDEVICE_MAC_LEN];
} RKANDROID_IDB_SEC3, *PRKANDROID_IDB_SEC3;

typedef struct __attribute__((aligned(1), packed)) {
    char name[32];
    bool is_mtd;
}RKIMAGE_STORAGE, *PRKIMAGE_STORAGE;

bool getImageVersion(const char *filepath, char *version, int maxLength) ;
int analyticImage(char *filepath, PRKIMAGE_HDR phdr);
rt_bool_t rk_ota_check_fw_md5sum(const char *dest_path);
#endif
