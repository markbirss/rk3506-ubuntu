// SPDX-License-Identifier:     GPL-2.0+
/*
 * (C) Copyright 2018 Rockchip Electronics Co., Ltd.
 *
 */

#include <rk_atags.h>

#ifdef __RT_THREAD__
#include <rtthread.h>
#endif
#include <string.h>
#include <stdio.h>

#define HASH_LEN    sizeof(u32)

#ifdef __RT_THREAD__
#define printf rt_kprintf
#define memset rt_memset
#endif


/*
 * The array is used to transform rom bootsource type to rk atags boot type.
 */
static int bootdev_map[] =
{
    BOOT_TYPE_UNKNOWN,
    BOOT_TYPE_NAND,
    BOOT_TYPE_EMMC,
    BOOT_TYPE_SPI_NOR,
    BOOT_TYPE_SPI_NAND,
    BOOT_TYPE_SD0,
    BOOT_TYPE_UNKNOWN,
    BOOT_TYPE_UFS,
    BOOT_TYPE_UNKNOWN,
    BOOT_TYPE_UNKNOWN,
    BOOT_TYPE_UNKNOWN
};

static int spl_bootdev_map[] =
{
    BOOT_TYPE_RAM,
    BOOT_TYPE_EMMC,
    BOOT_TYPE_SD0,
    BOOT_TYPE_UNKNOWN,
    BOOT_TYPE_NAND,
    BOOT_TYPE_UNKNOWN,
    BOOT_TYPE_UNKNOWN,
    BOOT_TYPE_UNKNOWN,
    BOOT_TYPE_UNKNOWN,
    BOOT_TYPE_UNKNOWN,
    BOOT_TYPE_UNKNOWN,
    BOOT_TYPE_UNKNOWN,
    BOOT_TYPE_UNKNOWN,
    BOOT_TYPE_UNKNOWN,
    BOOT_TYPE_UNKNOWN,
    BOOT_TYPE_UNKNOWN,
    BOOT_TYPE_MTD_BLK_NAND,
    BOOT_TYPE_MTD_BLK_SPI_NAND,
    BOOT_TYPE_MTD_BLK_SPI_NOR,
    BOOT_TYPE_UNKNOWN,
    BOOT_TYPE_UFS
};

static u32 js_hash(void *buf, u32 len)
{
    u32 i, hash = 0x47C6A7E6;
    char *data = buf;

    if (!buf || !len)
        return hash;

    for (i = 0; i < len; i++)
        hash ^= ((hash << 5) + data[i] + (hash >> 2));

    return hash;
}

int atags_bad_magic(u32 magic)
{
    bool bad;

    bad = ((magic != ATAG_CORE) &&
           (magic != ATAG_NONE) &&
           (magic < ATAG_SERIAL || magic > ATAG_MAX));
    if (bad)
    {
        printf("Magic(%x) is not support\n", magic);
    }

    return bad;
}

static int inline atags_size_overflow(struct tag *t, u32 tag_size)
{
    return (unsigned long)t + (tag_size << 2) - ATAGS_PHYS_BASE > ATAGS_SIZE;
}

int atags_overflow(struct tag *t)
{
    bool overflow;

    overflow = atags_size_overflow(t, 0) ||
               atags_size_overflow(t, t->hdr.size);
    if (overflow)
    {
        printf("Tag is overflow\n");
    }

    return overflow;
}

int atags_is_available(void)
{
    struct tag *t = (struct tag *)ATAGS_PHYS_BASE;

    return (t->hdr.magic == ATAG_CORE);
}

int atags_set_tag(u32 magic, void *tagdata)
{
    struct tag *t = (struct tag *)ATAGS_PHYS_BASE;
    u32 length, size = 0, hash;
    int append = 1; /* 0: override */

    if (!atags_is_available())
    {
        printf("atags is not available\n");
        return -EPERM;
    }

    if (!tagdata)
    {
        printf("tagdata is null\n");
        return -ENODATA;
    }

    if (atags_bad_magic(magic))
    {
        printf("magic is not support\n");
        return -EINVAL;
    }

    /* Not allowed to be set by user directly, so do nothing */
    if ((magic == ATAG_CORE) || (magic == ATAG_NONE))
    {
        printf("magic is not support\n");
        return -EPERM;
    }

    /* If not initialized, setup now! */
    if (t->hdr.magic != ATAG_CORE)
    {
        t->hdr.magic = ATAG_CORE;
        t->hdr.size = tag_size(tag_core);
        t->u.core.flags = 0;
        t->u.core.pagesize = 0;
        t->u.core.rootdev = 0;

        t = tag_next(t);
    }
    else
    {
        /* Find the end, and use it as a new tag */
        for_each_tag(t, (struct tag *)ATAGS_PHYS_BASE)
        {
            if (atags_overflow(t))
            {
                printf("tag is overflow\n");
                return -EINVAL;
            }

            if (atags_bad_magic(t->hdr.magic))
            {
                printf("tag magic is not support\n");
                return -EINVAL;
            }

            /* This is an old tag, override it */
            if (t->hdr.magic == magic)
            {
                append = 0;
                break;
            }

            if (t->hdr.magic == ATAG_NONE)
                break;
        }
    }

    /* Initialize new tag */
    switch (magic)
    {
    case ATAG_SERIAL:
        size = tag_size(tag_serial);
        break;
    case ATAG_BOOTDEV:
        size = tag_size(tag_bootdev);
        break;
    case ATAG_TOS_MEM:
        size = tag_size(tag_tos_mem);
        break;
    case ATAG_DDR_MEM:
        size = tag_size(tag_ddr_mem);
        break;
    case ATAG_RAM_PARTITION:
        size = tag_size(tag_ram_partition);
        break;
    case ATAG_ATF_MEM:
        size = tag_size(tag_atf_mem);
        break;
    case ATAG_PUB_KEY:
        size = tag_size(tag_pub_key);
        break;
    case ATAG_SOC_INFO:
        size = tag_size(tag_soc_info);
        break;
    case ATAG_BOOT1_PARAM:
        size = tag_size(tag_boot1p);
        break;
    case ATAG_PSTORE:
        size = tag_size(tag_pstore);
        break;
    case ATAG_FWVER:
        size = tag_size(tag_fwver);
        break;
    case ATAG_CONSOLE:
        size = tag_size(tag_console);
        break;
    };

    if (!size)
    {
        printf("tag size is 0\n");
        return -EINVAL;
    }

    if (atags_size_overflow(t, size))
    {
        printf("tag size overflow\n");
        return -ENOMEM;
    }

    /* It's okay to setup a new tag or override tag */
    t->hdr.magic = magic;
    t->hdr.size = size;
    length = (t->hdr.size << 2) - sizeof(struct tag_header) - HASH_LEN;
    memcpy(&t->u, (char *)tagdata, length);
    hash = js_hash(t, (size << 2) - HASH_LEN);
    memcpy((char *)&t->u + length, &hash, HASH_LEN);

    if (append)
    {
        /* Next tag */
        t = tag_next(t);

        /* Setup done */
        t->hdr.magic = ATAG_NONE;
        t->hdr.size = 0;
    }

    printf("set tag success\n");
    return 0;
}

int atags_set_shared_fwver(u32 fwid, char *ver)
{
    struct tag_fwver fw = {}, *pfw;
    struct tag *t;

    if (!ver || (strlen(ver) >= FWVER_LEN) || fwid >= FW_MAX)
        return -EINVAL;

    t = atags_get_tag(ATAG_FWVER);
    if (!t)
    {
        pfw = &fw;
        pfw->version = 0;
    }
    else
    {
        pfw = &t->u.fwver;
    }

    strcpy(pfw->ver[fwid], ver);
    atags_set_tag(ATAG_FWVER, pfw);

    return 0;
}

struct tag *atags_get_tag(u32 magic)
{
    u32 *hash, calc_hash, size;
    struct tag *t;

    if (!atags_is_available())
    {
        printf("atags is not available\n");
        return NULL;
    }

    for_each_tag(t, (struct tag *)ATAGS_PHYS_BASE)
    {
        if (atags_overflow(t))
        {
            printf("tag is overflow\n");
            return NULL;
        }

        if (atags_bad_magic(t->hdr.magic))
        {
            printf("tag magic is not support\n");
            return NULL;
        }

        if (t->hdr.magic != magic)
        {
            printf("tag magic is not match, magic = %x, expect = %x\n", t->hdr.magic, magic);
            continue;
        }

        size = t->hdr.size;
        hash = (u32 *)((ulong)t + (size << 2) - HASH_LEN);
        if (!*hash)
        {
            printf("No hash, magic(%x)\n", magic);
            return t;
        }
        else
        {
            calc_hash = js_hash(t, (size << 2) - HASH_LEN);
            if (calc_hash == *hash)
            {
                printf("Hash okay, magic(%x)\n", magic);
                return t;
            }
            else
            {
                printf("Hash bad, magic(%x), orgHash=%x, nowHash=%x\n",
                       magic, *hash, calc_hash);
                return NULL;
            }
        }
    }

    printf("get tag failed\n");
    return NULL;
}

void atags_destroy(void)
{
    if (atags_is_available())
        memset((char *)ATAGS_PHYS_BASE, 0, sizeof(struct tag));
}

