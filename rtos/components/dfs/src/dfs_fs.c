/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2005-02-22     Bernard      The first version.
 * 2010-06-30     Bernard      Optimize for RT-Thread RTOS
 * 2011-03-12     Bernard      fix the filesystem lookup issue.
 * 2017-11-30     Bernard      fix the filesystem_operation_table issue.
 * 2017-12-05     Bernard      fix the fs type search issue in mkfs.
 */

#include <dfs_fs.h>
#include <dfs_file.h>
#include "dfs_private.h"

typedef struct _gpt_header {
    uint64_t signature;                        /* 0x0  */
    uint32_t revision;                         /* 0x8  */
    uint32_t header_size;                      /* 0xC  */
    uint32_t header_crc32;                     /* 0x10 */
    uint32_t reserved1;                        /* 0x14 */
    uint64_t my_lba;                           /* 0x18 */
    uint64_t alternate_lba;                    /* 0x20 */
    uint64_t first_usable_lba;                 /* 0x28 */
    uint64_t last_usable_lba;                  /* 0x30 */
    uint8_t  disk_guid[16];                    /* 0x38 */
    uint64_t partition_entry_lba;              /* 0x48 */
    uint32_t num_partition_entries;            /* 0x50 */
    uint32_t sizeof_partition_entry;           /* 0x54 */
    uint32_t partition_entry_array_crc32;      /* 0x58 */
} __packed gpt_header;

typedef struct _gpt_entry {
    uint8_t  partition_type_guid[16];          /* 0x0  */
    uint8_t  unique_partition_guid[16];        /* 0x10 */
    uint64_t starting_lba;                     /* 0x20 */
    uint64_t ending_lba;                       /* 0x28 */
    uint64_t attributes;                       /* 0x30 */
    uint8_t  partition_name[72];               /* 0x38 */
} __packed gpt_entry;


/* rk_partition: ui_part_property bits filed */
#define RK_PARTITION_NO_PARTITION_SIZE (1 << 2)
#define RK_PARTITION_PROPERTY_SHIFT    (8)
#define RK_PARTITION_PROPERTY_MASK     (0x3 << RK_PARTITION_PROPERTY_SHIFT)
#define RK_PARTITION_PROPERTY_ROONLY   (PART_FLAG_RDONLY << RK_PARTITION_PROPERTY_SHIFT)
#define RK_PARTITION_PROPERTY_WRONLY   (PART_FLAG_WRONLY << RK_PARTITION_PROPERTY_SHIFT)
#define RK_PARTITION_PROPERTY_RDWR     (PART_FLAG_RDWR << RK_PARTITION_PROPERTY_SHIFT)

/* rk_partition: partition date time */
typedef enum
{
    PART_VENDOR     = 1 << 0,
    PART_IDBLOCK    = 1 << 1,
    PART_MISC       = 1 << 2,
    PART_FW1        = 1 << 3,
    PART_FW2        = 1 << 4,
    PART_DATA       = 1 << 5,
    PART_FONT1      = 1 << 6,
    PART_FONT2      = 1 << 7,
    PART_CHAR       = 1 << 8,
    PART_MENU       = 1 << 9,
    PART_UI         = 1 << 10,
    PART_USER1      = 1 << 30,
    PART_USER2      = 1 << 31
} enum_partition_type;

/* rk_partition : partition date time */
struct rk_parttion_date_time
{
    uint16_t  year;
    uint8_t   month;
    uint8_t   day;
    uint8_t   hour;
    uint8_t   min;
    uint8_t   sec;
    uint8_t   reserve;
};

/* rk_partition: partition head file */
struct rk_partition_header
{
    uint32_t    ui_fw_tag;  /* "RKFP" */
    struct rk_parttion_date_time    dt_release_data_time;
    uint32_t    ui_fw_ver;
    uint32_t    ui_size;    /* size of sturct,unit of u8 */
    uint32_t    ui_part_entry_offset;   /* unit of sector */
    uint32_t    ui_backup_part_entry_offset;
    uint32_t    ui_part_entry_size; /* unit of u8 */
    uint32_t    ui_part_entry_count;
    uint32_t    ui_fw_size; /* unit of u8 */
    uint8_t     reserved[464];
    uint32_t    ui_part_entry_crc;
    uint32_t    ui_header_crc;
};

/* rk_partition: partition item */
typedef struct rk_parttion_item
{
    uint8_t     sz_name[32];
    enum_partition_type em_part_type;
    uint32_t    ui_pt_off;  /* unit of sector */
    uint32_t    ui_pt_sz;   /* unit of sector */
    uint32_t    ui_data_length; /* ui_data_length low 32 */
    uint32_t    reserved1;  /* ui_data_length high 32 */
    uint32_t    ui_part_property;
    uint8_t     reserved2[72];
} STRUCT_PART_ITEM, *PSTRUCT_PART_ITEM;

typedef struct rk_partition_info
{
    struct rk_partition_header hdr; /* 0.5KB */
    struct rk_parttion_item part[12];   /* 1.5KB */
} STRUCT_PART_INFO, *PSTRUCT_PART_INFO;;

#define RK_PARTITION_TAG    0x50464B52
#define RK_PARTITION_NAME_SIZE  32

#define RK_PARTITION_REGISTER_TYPE_BLK (0 << 10)
#define RK_PARTITION_REGISTER_TYPE_MTD (1 << 10)

/**
 * @addtogroup FsApi
 */
/*@{*/

/**
 * this function will register a file system instance to device file system.
 *
 * @param ops the file system instance to be registered.
 *
 * @return 0 on successful, -1 on failed.
 */
int dfs_register(const struct dfs_filesystem_ops *ops)
{
    int ret = RT_EOK;
    const struct dfs_filesystem_ops **empty = NULL;
    const struct dfs_filesystem_ops **iter;

    /* lock filesystem */
    dfs_lock();
    /* check if this filesystem was already registered */
    for (iter = &filesystem_operation_table[0];
            iter < &filesystem_operation_table[DFS_FILESYSTEM_TYPES_MAX]; iter ++)
    {
        /* find out an empty filesystem type entry */
        if (*iter == NULL)
            (empty == NULL) ? (empty = iter) : 0;
        else if (strcmp((*iter)->name, ops->name) == 0)
        {
            rt_set_errno(-EEXIST);
            ret = -1;
            break;
        }
    }

    /* save the filesystem's operations */
    if (empty == NULL)
    {
        rt_set_errno(-ENOSPC);
        LOG_E("There is no space to register this file system (%s).", ops->name);
        ret = -1;
    }
    else if (ret == RT_EOK)
    {
        *empty = ops;
    }

    dfs_unlock();
    return ret;
}

/**
 * this function will return the file system mounted on specified path.
 *
 * @param path the specified path string.
 *
 * @return the found file system or NULL if no file system mounted on
 * specified path
 */
struct dfs_filesystem *dfs_filesystem_lookup(const char *path)
{
    struct dfs_filesystem *iter;
    struct dfs_filesystem *fs = NULL;
    uint32_t fspath, prefixlen;

    prefixlen = 0;

    RT_ASSERT(path);

    /* lock filesystem */
    dfs_lock();

    /* lookup it in the filesystem table */
    for (iter = &filesystem_table[0];
            iter < &filesystem_table[DFS_FILESYSTEMS_MAX]; iter++)
    {
        if ((iter->path == NULL) || (iter->ops == NULL))
            continue;

        fspath = strlen(iter->path);
        if ((fspath < prefixlen)
            || (strncmp(iter->path, path, fspath) != 0))
            continue;

        /* check next path separator */
        if (fspath > 1 && (strlen(path) > fspath) && (path[fspath] != '/'))
            continue;

        fs = iter;
        prefixlen = fspath;
    }

    dfs_unlock();

    return fs;
}

/**
 * this function will return the mounted path for specified device.
 *
 * @param device the device object which is mounted.
 *
 * @return the mounted path or NULL if none device mounted.
 */
const char *dfs_filesystem_get_mounted_path(struct rt_device *device)
{
    const char *path = NULL;
    struct dfs_filesystem *iter;

    dfs_lock();
    for (iter = &filesystem_table[0];
            iter < &filesystem_table[DFS_FILESYSTEMS_MAX]; iter++)
    {
        /* find the mounted device */
        if (iter->ops == NULL) continue;
        else if (iter->dev_id == device)
        {
            path = iter->path;
            break;
        }
    }

    /* release filesystem_table lock */
    dfs_unlock();

    return path;
}

void utf16le_to_ascii(char* ascii_str, const char* utf16le_str, int n) {
    int i = 0;

    while (*utf16le_str || *(utf16le_str + 1)) {
        if (++i >= n)
            break;

        *ascii_str++ = *utf16le_str;
        utf16le_str += 2;
    }
    *ascii_str = '\0';
}

/**
 * this function will fetch the GPT partition table on specified buffer.
 *
 * @param part the returned partition structure.
 * @param buf the buffer contains partition table.
 * @param sect_count the sector count in the buffer.
 * @param pindex the index of partition table to fetch.
 *
 * @return RT_EOK on successful or -RT_ERROR on failed.
 */
static int dfs_filesystem_get_gpt_partition(struct dfs_partition *part,
                                      uint8_t         *buf,
                                      uint8_t         sect_count,
                                      uint32_t        pindex)
{
    gpt_header *gpt_head;
    gpt_entry  *gpt_ent;
    uint32_t gpt_table_begin, index;

    if (sect_count <= (2 + pindex / 4))
        return -EIO;

    /* check gpt main header */
    gpt_head = (gpt_header *)(&buf[512]);
    if (gpt_head->signature != 0x5452415020494645 || gpt_head->my_lba != 1) {
        return -EIO;
    }

    /* get partition table LBA */
    gpt_table_begin = gpt_head->partition_entry_lba;
    index = ((gpt_table_begin + pindex / 4) * 512) + ((pindex % 4) * 128);
    if (index >= (sect_count * 512)) {
        return -EIO;
    }

    gpt_ent = (gpt_entry *)(&buf[index]);

    /* get partition info for gpt entry */
    utf16le_to_ascii(part->name, (const char *)gpt_ent->partition_name, sizeof(part->name));    /* the name of gpt partition is unicode, do not use strcpy */
    part->type = 0x0B;
    part->offset = gpt_ent->starting_lba;
    part->size = gpt_ent->ending_lba - gpt_ent->starting_lba;
    if (part->size != 0)
        part->size += 1;    /* ending_lba is inclusive */
    //rt_kprintf("pindex=%d, name=%s, offset=%d, size=%d\n", pindex, part->name, part->offset, part->size);

    if (part->size != 0)
        return RT_EOK;
    else
        return -RT_ERROR;
}


/**
 * this function will fetch the RK partition table on specified buffer.
 *
 * @param part the returned partition structure.
 * @param buf the buffer contains partition table.
 * @param sect_count the sector count in the buffer.
 * @param pindex the index of partition table to fetch.
 *
 * @return RT_EOK on successful or -RT_ERROR on failed.
 */
static int dfs_filesystem_get_rk_partition(struct dfs_partition *part,
                                           uint8_t         *buf,
                                           uint8_t         sect_count,
                                           uint32_t        pindex)
{
    struct rk_partition_info *part_temp = (struct rk_partition_info *)buf;
    int part_num;

    if (sect_count < 4) {
        rt_kprintf("%s input invald, sect_count=%d\n", __func__, sect_count);
        return -EIO;
    }

    part_num = part_temp->hdr.ui_part_entry_count;

    if (pindex >= part_num) {
        rt_kprintf("%s input invald, pindex=%d>part_num=%d\n", __func__, pindex, part_num);
        return -EIO;
    }

    /* get partition info for gpt entry */
    rt_strncpy(part->name, (char *)part_temp->part[pindex].sz_name, sizeof(part->name));
    part->type = 0x0B;
    part->offset = (uint32_t)part_temp->part[pindex].ui_pt_off;
    if (part_temp->part[pindex].ui_pt_sz == 0xFFFFFFFF || (part_temp->part[pindex].ui_part_property & RK_PARTITION_NO_PARTITION_SIZE))
        part->size = 0xFFFFFFFF;
    else
        part->size = ((uint32_t)part_temp->part[pindex].ui_pt_sz);
    rt_kprintf("pindex=%d, name=%s, offset=%d, size=%d\n", pindex, part_temp->part[pindex].sz_name, part->offset, part->size);

    if (part->size != 0)
        return RT_EOK;
    else
        return -RT_ERROR;
}

/**
 * this function will fetch the partition table on specified buffer.
 *
 * @param part the returned partition structure.
 * @param buf the buffer contains partition table.
 * @param sect_count the sector count in the buffer.
 * @param pindex the index of partition table to fetch.
 *
 * @return RT_EOK on successful or -RT_ERROR on failed.
 */
int dfs_filesystem_get_partition(struct dfs_partition *part,
                                      uint8_t         *buf,
                                      uint8_t         sect_count,
                                      uint32_t        pindex)
{
#define DPT_ADDRESS     0x1be       /* device partition offset in Boot Sector */
#define DPT_ITEM_SIZE   16          /* partition item size */

    uint8_t *dpt;
    uint8_t type;
    uint32_t type2;

    RT_ASSERT(part != NULL);
    RT_ASSERT(buf != NULL);

    /* check gpt partion */
    dpt = buf + DPT_ADDRESS + 4;
    type = *dpt;
    if (type == 0xee)
        return dfs_filesystem_get_gpt_partition(part, buf, sect_count, pindex);

    type2 = ((uint32_t *)buf)[0];
    if (type2 == RK_PARTITION_TAG)
      return dfs_filesystem_get_rk_partition(part, buf, sect_count, pindex);

    dpt = buf + DPT_ADDRESS + pindex * DPT_ITEM_SIZE;

    /* check if it is a valid partition table */
    if ((*dpt != 0x80) && (*dpt != 0x00))
        return -EIO;

    /* get partition type */
    type = *(dpt + 4);
    if (type == 0)
        return -EIO;

    /* set partition information
     *    size is the number of 512-Byte */
    rt_memset(part->name, 0, sizeof(part->name));      /* MBR has no partition name */
    part->type = type;
    part->offset = *(dpt + 8) | *(dpt + 9) << 8 | *(dpt + 10) << 16 | *(dpt + 11) << 24;
    part->size = *(dpt + 12) | *(dpt + 13) << 8 | *(dpt + 14) << 16 | *(dpt + 15) << 24;

    rt_kprintf("found part[%d], begin: %d, size: ",
               pindex, part->offset * 512);
    if ((part->size >> 11) == 0)
        rt_kprintf("%d%s", part->size >> 1, "KB\n"); /* KB */
    else
    {
        unsigned int part_size;
        part_size = part->size >> 11;                /* MB */
        if ((part_size >> 10) == 0)
            rt_kprintf("%d.%d%s", part_size, (part->size >> 1) & 0x3FF, "MB\n");
        else
            rt_kprintf("%d.%d%s", part_size >> 10, part_size & 0x3FF, "GB\n");
    }

    return RT_EOK;
}

/**
 * this function will mount a file system on a specified path.
 *
 * @param device_name the name of device which includes a file system.
 * @param path the path to mount a file system
 * @param filesystemtype the file system type
 * @param rwflag the read/write etc. flag.
 * @param data the private data(parameter) for this file system.
 *
 * @return 0 on successful or -1 on failed.
 */
int dfs_mount(const char   *device_name,
              const char   *path,
              const char   *filesystemtype,
              unsigned long rwflag,
              const void   *data)
{
    const struct dfs_filesystem_ops **ops;
    struct dfs_filesystem *iter;
    struct dfs_filesystem *fs = NULL;
    char *fullpath = NULL;
    rt_device_t dev_id;

    /* open specific device */
    if (device_name == NULL)
    {
        /* which is a non-device filesystem mount */
        dev_id = NULL;
    }
    else if ((dev_id = rt_device_find(device_name)) == NULL)
    {
        /* no this device */
        rt_set_errno(-ENODEV);
        return -1;
    }

    /* find out the specific filesystem */
    dfs_lock();

    for (ops = &filesystem_operation_table[0];
            ops < &filesystem_operation_table[DFS_FILESYSTEM_TYPES_MAX]; ops++)
        if ((*ops != NULL) && (strcmp((*ops)->name, filesystemtype) == 0))
            break;

    dfs_unlock();

    if (ops == &filesystem_operation_table[DFS_FILESYSTEM_TYPES_MAX])
    {
        /* can't find filesystem */
        rt_set_errno(-ENODEV);
        return -1;
    }

    /* check if there is mount implementation */
    if ((*ops == NULL) || ((*ops)->mount == NULL))
    {
        rt_set_errno(-ENOSYS);
        return -1;
    }

    /* make full path for special file */
    fullpath = dfs_normalize_path(NULL, path);
    if (fullpath == NULL) /* not an abstract path */
    {
        rt_set_errno(-ENOTDIR);
        return -1;
    }

    /* Check if the path exists or not, raw APIs call, fixme */
    if ((strcmp(fullpath, "/") != 0) && (strcmp(fullpath, "/dev") != 0))
    {
        struct dfs_fd fd;

        if (dfs_file_open(&fd, fullpath, O_RDONLY | O_DIRECTORY) < 0)
        {
            rt_free(fullpath);
            rt_set_errno(-ENOTDIR);

            return -1;
        }
        dfs_file_close(&fd);
    }

    /* check whether the file system mounted or not  in the filesystem table
     * if it is unmounted yet, find out an empty entry */
    dfs_lock();

    for (iter = &filesystem_table[0];
            iter < &filesystem_table[DFS_FILESYSTEMS_MAX]; iter++)
    {
        /* check if it is an empty filesystem table entry? if it is, save fs */
        if (iter->ops == NULL)
            (fs == NULL) ? (fs = iter) : 0;
        /* check if the PATH is mounted */
        else if (strcmp(iter->path, path) == 0)
        {
            rt_set_errno(-EINVAL);
            goto err1;
        }
    }

    if ((fs == NULL) && (iter == &filesystem_table[DFS_FILESYSTEMS_MAX]))
    {
        rt_set_errno(-ENOSPC);
        LOG_E("There is no space to mount this file system (%s).", filesystemtype);
        goto err1;
    }

    /* register file system */
    fs->path   = fullpath;
    fs->ops    = *ops;
    fs->dev_id = dev_id;
    /* release filesystem_table lock */
    dfs_unlock();

    /* open device, but do not check the status of device */
    if (dev_id != NULL)
    {
        if (rt_device_open(fs->dev_id,
                           RT_DEVICE_OFLAG_RDWR) != RT_EOK)
        {
            /* The underlying device has error, clear the entry. */
            dfs_lock();
            rt_memset(fs, 0, sizeof(struct dfs_filesystem));

            goto err1;
        }
    }

    /* call mount of this filesystem */
    if ((*ops)->mount(fs, rwflag, data) < 0)
    {
        /* close device */
        if (dev_id != NULL)
            rt_device_close(fs->dev_id);

        /* mount failed */
        dfs_lock();
        /* clear filesystem table entry */
        rt_memset(fs, 0, sizeof(struct dfs_filesystem));

        goto err1;
    }

    return 0;

err1:
    dfs_unlock();
    rt_free(fullpath);

    return -1;
}

/**
 * this function will unmount a file system on specified path.
 *
 * @param specialfile the specified path which mounted a file system.
 *
 * @return 0 on successful or -1 on failed.
 */
int dfs_unmount(const char *specialfile)
{
    char *fullpath;
    struct dfs_filesystem *iter;
    struct dfs_filesystem *fs = NULL;

    fullpath = dfs_normalize_path(NULL, specialfile);
    if (fullpath == NULL)
    {
        rt_set_errno(-ENOTDIR);

        return -1;
    }

    /* lock filesystem */
    dfs_lock();

    for (iter = &filesystem_table[0];
            iter < &filesystem_table[DFS_FILESYSTEMS_MAX]; iter++)
    {
        /* check if the PATH is mounted */
        if ((iter->path != NULL) && (strcmp(iter->path, fullpath) == 0))
        {
            fs = iter;
            break;
        }
    }

    if (fs == NULL ||
        fs->ops->unmount == NULL ||
        fs->ops->unmount(fs) < 0)
    {
        goto err1;
    }

    /* close device, but do not check the status of device */
    if (fs->dev_id != NULL)
        rt_device_close(fs->dev_id);

    if (fs->path != NULL)
        rt_free(fs->path);

    /* clear this filesystem table entry */
    rt_memset(fs, 0, sizeof(struct dfs_filesystem));

    dfs_unlock();
    rt_free(fullpath);

    return 0;

err1:
    dfs_unlock();
    rt_free(fullpath);

    return -1;
}

/**
 * make a file system on the special device
 *
 * @param fs_name the file system name
 * @param device_name the special device name
 *
 * @return 0 on successful, otherwise failed.
 */
int dfs_mkfs(const char *fs_name, const char *device_name)
{
    int index;
    rt_device_t dev_id = NULL;

    /* check device name, and it should not be NULL */
    if (device_name != NULL)
        dev_id = rt_device_find(device_name);

    if (dev_id == NULL)
    {
        rt_set_errno(-ENODEV);
        LOG_E("Device (%s) was not found", device_name);
        return -1;
    }

    /* lock file system */
    dfs_lock();
    /* find the file system operations */
    for (index = 0; index < DFS_FILESYSTEM_TYPES_MAX; index ++)
    {
        if (filesystem_operation_table[index] != NULL &&
            strcmp(filesystem_operation_table[index]->name, fs_name) == 0)
            break;
    }
    dfs_unlock();

    if (index < DFS_FILESYSTEM_TYPES_MAX)
    {
        /* find file system operation */
        const struct dfs_filesystem_ops *ops = filesystem_operation_table[index];
        if (ops->mkfs == NULL)
        {
            LOG_E("The file system (%s) mkfs function was not implement", fs_name);
            rt_set_errno(-ENOSYS);
            return -1;
        }

        return ops->mkfs(dev_id);
    }

    LOG_E("File system (%s) was not found.", fs_name);

    return -1;
}

/**
 * this function will return the information about a mounted file system.
 *
 * @param path the path which mounted file system.
 * @param buffer the buffer to save the returned information.
 *
 * @return 0 on successful, others on failed.
 */
int dfs_statfs(const char *path, struct statfs *buffer)
{
    struct dfs_filesystem *fs;

    fs = dfs_filesystem_lookup(path);
    if (fs != NULL)
    {
        if (fs->ops->statfs != NULL)
            return fs->ops->statfs(fs, buffer);
    }

    return -1;
}

#ifdef RT_USING_DFS_MNTTABLE
int dfs_mount_table(void)
{
    int index = 0;

    while (1)
    {
        if (mount_table[index].path == NULL) break;

        if (dfs_mount(mount_table[index].device_name,
                      mount_table[index].path,
                      mount_table[index].filesystemtype,
                      mount_table[index].rwflag,
                      mount_table[index].data) != 0)
        {
            LOG_E("mount fs[%s] on %s failed.\n", mount_table[index].filesystemtype,
                       mount_table[index].path);
            return -RT_ERROR;
        }

        index ++;
    }
    return 0;
}
INIT_ENV_EXPORT(dfs_mount_table);

int dfs_mount_device(rt_device_t dev)
{
  int index = 0;

  if(dev == RT_NULL) {
    rt_kprintf("the device is NULL to be mounted.\n");
    return -RT_ERROR;
  }

  while (1)
  {
    if (mount_table[index].path == NULL) break;

    if(strncmp(mount_table[index].device_name, dev->parent.name, RT_NAME_MAX) == 0) {
      if (dfs_mount(mount_table[index].device_name,
                    mount_table[index].path,
                    mount_table[index].filesystemtype,
                    mount_table[index].rwflag,
                    mount_table[index].data) != 0)
      {
        LOG_E("mount fs[%s] device[%s] to %s failed.\n", mount_table[index].filesystemtype, dev->parent.name,
                   mount_table[index].path);
        return -RT_ERROR;
      } else {
        LOG_D("mount fs[%s] device[%s] to %s ok.\n", mount_table[index].filesystemtype, dev->parent.name,
                   mount_table[index].path);
        return RT_EOK;
      }
    }

    index ++;
  }

  rt_kprintf("can't find device:%s to be mounted.\n", dev->parent.name);
  return -RT_ERROR;
}

int dfs_unmount_device(rt_device_t dev)
{
    struct dfs_filesystem *iter;
    struct dfs_filesystem *fs = NULL;

    /* lock filesystem */
    dfs_lock();

    for (iter = &filesystem_table[0];
            iter < &filesystem_table[DFS_FILESYSTEMS_MAX]; iter++)
    {
        if (iter->dev_id == NULL)
            continue;

        /* check if the PATH is mounted */
        if (strcmp(iter->dev_id->parent.name, dev->parent.name) == 0)
        {
            fs = iter;
            break;
        }
    }

    if (fs == NULL ||
        fs->ops->unmount == NULL ||
        fs->ops->unmount(fs) < 0)
    {
        goto err1;
    }

    /* close device, but do not check the status of device */
    if (fs->dev_id != NULL)
        rt_device_close(fs->dev_id);

    if (fs->path != NULL)
        rt_free(fs->path);

    /* clear this filesystem table entry */
    rt_memset(fs, 0, sizeof(struct dfs_filesystem));

    dfs_unlock();

    return 0;

err1:
    dfs_unlock();

    return -1;
}

#endif

#ifdef RT_USING_FINSH
#include <finsh.h>
void mkfs(const char *fs_name, const char *device_name)
{
    dfs_mkfs(fs_name, device_name);
}
FINSH_FUNCTION_EXPORT(mkfs, make a file system);

int df(const char *path)
{
    int result;
    int minor = 0;
    long long cap;
    struct statfs buffer;

    int unit_index = 0;
    char *unit_str[] = {"KB", "MB", "GB"};

    result = dfs_statfs(path ? path : NULL, &buffer);
    if (result != 0)
    {
        rt_kprintf("dfs_statfs failed.\n");
        return -1;
    }

    cap = ((long long)buffer.f_bsize) * ((long long)buffer.f_bfree) / 1024LL;
    for (unit_index = 0; unit_index < 2; unit_index ++)
    {
        if (cap < 1024) break;

        minor = (cap % 1024) * 10 / 1024; /* only one decimal point */
        cap = cap / 1024;
    }

    rt_kprintf("disk free: %d.%d %s [ %d block, %d bytes per block ]\n",
               (unsigned long)cap, minor, unit_str[unit_index], buffer.f_bfree, buffer.f_bsize);
    return 0;
}
FINSH_FUNCTION_EXPORT(df, get disk free);
#endif

/* @} */
