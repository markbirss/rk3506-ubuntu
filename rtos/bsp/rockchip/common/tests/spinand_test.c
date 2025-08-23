/**
  * Copyright (c) 2020 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    spinand_test.c
  * @version V1.0
  * @brief   spi nand test
  *
  * Change Logs:
  * Date           Author          Notes
  * 2020-06-17     Dingqiang Lin   the first version
  *
  ******************************************************************************
  */

#include <rtdevice.h>
#include <rtthread.h>

#ifdef RT_USING_COMMON_TEST_SPINAND

#include <drivers/mtd_nand.h>
#include <dfs_fs.h>
#include "hal_base.h"
#include "mini_ftl.h"
#include "../drivers/drv_flash_partition.h"

#define NAND_PAGE_SIZE  4096
#define NAND_SPARE_SIZE 64

#define RK_PARTITION_MAX_PARTITION 16

#ifndef EUCLEAN
#define EUCLEAN                         117
#endif

struct nand_ops_t
{
    uint32_t (*erase_blk)(uint8_t cs, uint32_t page_addr);
    uint32_t (*prog_page)(uint8_t cs, uint32_t page_addr, uint32_t *data, uint32_t *spare);
    uint32_t (*read_page)(uint8_t cs, uint32_t page_addr, uint32_t *data, uint32_t *spare);
};

static uint32_t *pwrite;
static uint32_t *pread;

static uint32_t *pspare_write;
static uint32_t *pspare_read;

static uint32_t bad_blk_num;
static uint32_t bad_page_num;
static struct nand_ops_t g_nand_ops;
struct rt_mtd_nand_device *mtd_dev = RT_NULL;

static void spinand_test_show_usage()
{
    rt_kprintf("1. spinand_test write <page_addr> <page_num>\n");
    rt_kprintf("2. spinand_test read <page_addr> <page_nun>\n");
    rt_kprintf("3. spinand_test erase <blk_addr>\n");
    rt_kprintf("4. spinand_test erase_all <blk_addr>\n");
    rt_kprintf("5. spinand_test stress <loop> <blk_addr> <blks>\n");
    rt_kprintf("6. spinand_test bbt\n");
    rt_kprintf("7. spinand_test markbad <blk_addr>\n");
    rt_kprintf("8. spinand_test mini_ftl_test <byte_offset>\n");
    rt_kprintf("9. spinand_test part_list\n");
    rt_kprintf("like:\n");
    rt_kprintf("\tspinand_test write 1024 1\n");
    rt_kprintf("\tspinand_test read 1024 1\n");
    rt_kprintf("\tspinand_test erase 16\n");
    rt_kprintf("\tspinand_test erase_all 512\n");
    rt_kprintf("\tspinand_test stress 5000 512 256\n");
    rt_kprintf("\tspinand_test bbt\n");
    rt_kprintf("\tspinand_test mini_ftl_test 67108864\n");
    rt_kprintf("\tspinand_test part_list\n");
}

HAL_Status spinand_dbg_hex(char *s, void *buf, uint32_t width, uint32_t len)
{
    uint32_t i, j;
    unsigned char *p8 = (unsigned char *)buf;
    unsigned short *p16 = (unsigned short *)buf;
    uint32_t *p32 = (uint32_t *)buf;

    j = 0;
    for (i = 0; i < len; i++)
    {
        if (j == 0)
        {
            rt_kprintf("[SPI Nand TEST] %s %p + 0x%lx:", s, buf, i * width);
        }

        if (width == 4)
            rt_kprintf("0x%08lx,", p32[i]);
        else if (width == 2)
            rt_kprintf("0x%04x,", p16[i]);
        else
            rt_kprintf("0x%02x,", p8[i]);

        if (++j >= 16)
        {
            j = 0;
            rt_kprintf("\n");
        }
    }
    rt_kprintf("\n");

    return HAL_OK;
}

static uint32_t nand_flash_test(uint32_t blk_begin, uint32_t blk_end, uint32_t is_recheck)
{
    uint32_t i, block, page;
    int ret;
    uint32_t pages_num = 64;
    uint32_t block_addr = 64;
    uint32_t is_bad_blk = 0;
    char *print_tag = is_recheck ? "read" : "prog";
    uint32_t sec_per_page = 4; /* 4 - 2KB; 8 - 4KB */

    bad_blk_num = 0;
    bad_page_num = 0;
    for (block = blk_begin; block < blk_end; block++)
    {
        is_bad_blk = 0;
        rt_kprintf("Flash %s block: %x\n", print_tag, block);
        if (!is_recheck)
            g_nand_ops.erase_blk(0, block * block_addr);
        for (page = 0; page < pages_num; page++)
        {
            for (i = 0; i < sec_per_page * 128; i++)
                pwrite[i] = ((block * block_addr + page) << 16) + i;
            for (i = 0; i < sec_per_page / 2; i++)
                pspare_write[i] = ((block * block_addr + page) << 16) + 0x5AF0 + i;
            pwrite[0] |= 0xff; /* except bad block marker in data area */
            if (!is_recheck)
                g_nand_ops.prog_page(0, block * block_addr + page, pwrite, pspare_write);
            memset(pread, 0, sec_per_page * 512);
            memset(pspare_read, 0, sec_per_page * 2);
            ret = g_nand_ops.read_page(0, block * block_addr + page, pread, pspare_read);
            if (ret)
                is_bad_blk = 1;
            for (i = 0; i < sec_per_page * 128; i++)
            {
                if (pwrite[i] != pread[i])
                {
                    rt_kprintf("Data not macth,W:%x,R:%x,@%x,pageAddr=%x\n",
                               pwrite[i], pread[i], i, block * block_addr + page);
                    is_bad_blk = 1;
                    break;
                }
            }
            if (is_bad_blk)
            {
                bad_page_num++;
                spinand_dbg_hex("data w:", pwrite, 4, sec_per_page * 128);
                spinand_dbg_hex("data r:", pread, 4, sec_per_page * 128);
                while (1)
                    ;
            }
        }
        if (is_bad_blk)
            bad_blk_num++;
    }
    return 0;
}

static void flash_erase_all_block(uint32_t firstblk, uint32_t end_blk)
{
    uint32_t blk;

    for (blk = firstblk; blk < end_blk; blk++)
    {
        rt_kprintf("erase blk %d\n", blk);
        rt_mtd_nand_erase_block(mtd_dev, blk);
    }

}

static uint32_t nand_flash_stress_test(uint32_t blk_begin, uint32_t block_end)
{
    uint32_t ret = 0;

    rt_kprintf("%s\n", __func__);
    flash_erase_all_block(blk_begin, block_end);
    rt_kprintf("Flash prog/read test\n");
    ret = nand_flash_test(blk_begin, block_end, 0);
    if (ret)
        return -1;
    rt_kprintf("Flash read recheck test\n");
    ret = nand_flash_test(blk_begin, block_end, 1);
    if (ret)
        return -1;
    rt_kprintf("%s success\n", __func__);
    return ret;
}

static uint32_t erase_blk(uint8_t cs, uint32_t page_addr)
{
    if (mini_ftl_erase(mtd_dev, page_addr * mtd_dev->page_size, mtd_dev->pages_per_block * mtd_dev->page_size))
        return -1;
    else
        return 0;
}

static uint32_t prog_page(uint8_t cs, uint32_t page_addr, uint32_t *data, uint32_t *spare)
{
    int ret;

    ret = mini_ftl_write(mtd_dev, (const rt_uint8_t *)data, page_addr * mtd_dev->page_size, mtd_dev->page_size);
    if (ret != mtd_dev->page_size)
    {
        rt_kprintf("%s 0x=%x %d\n", __func__, page_addr, ret);
        return -RT_EIO;
    }
    else
    {
        return 0;
    }
}

static uint32_t read_page(uint8_t cs, uint32_t page_addr, uint32_t *data, uint32_t *spare)
{
    int ret;

    ret = mini_ftl_read(mtd_dev, (rt_uint8_t *)data, page_addr * mtd_dev->page_size, mtd_dev->page_size);
    if (ret != mtd_dev->page_size)
    {
        rt_kprintf("%s 0x=%x %d\n", __func__, page_addr, ret);
        return -RT_EIO;
    }
    else
    {
        return 0;
    }
}

static __attribute__((__unused__)) int nand_flash_misc_test(void)
{
    int ret = 0;
    uint32_t blk;

    rt_kprintf("%s\n", __func__);
    rt_kprintf("Bad block operation test\n");

    for (blk = 10; blk < 60; blk++)
    {
        rt_kprintf("%s mark bad block %d\n", __func__, blk);
        ret = rt_mtd_nand_mark_badblock(mtd_dev, blk);
        if (ret)
        {
            rt_kprintf("%s mark bad fail %d\n", __func__, ret);
            while (1)
                ;
        }
        ret = rt_mtd_nand_check_block(mtd_dev, blk);
        if (!ret)
        {
            rt_kprintf("%s mark bad check fail %d\n", __func__, ret);
            while (1)
                ;
        }

        g_nand_ops.erase_blk(0, blk);
    }
    rt_kprintf("%s success\n", __func__);

    return ret;
}

void nand_utils_test(uint32_t loop, uint32_t blk_begin, uint32_t block_end)
{
    rt_uint32_t size;

    size = mtd_dev->block_total * mtd_dev->pages_per_block * mtd_dev->page_size;
    rt_kprintf("size %d MB\n", (uint32_t)(size / 1024 / 1024));

    pwrite = malloc(NAND_PAGE_SIZE);
    pspare_write = malloc(NAND_SPARE_SIZE);
    pread = malloc(NAND_PAGE_SIZE);
    pspare_read = malloc(NAND_SPARE_SIZE);

    g_nand_ops.erase_blk = erase_blk;
    g_nand_ops.read_page = read_page;
    g_nand_ops.prog_page = prog_page;

    if (block_end > mtd_dev->block_total)
    {
        block_end = mtd_dev->block_total;
    }

    while (loop--)
    {
        nand_flash_stress_test(blk_begin, block_end);
    }

    free(pwrite);
    free(pspare_write);
    free(pread);
    free(pspare_read);
}

void nand_bbt(void)
{
    rt_uint32_t blk;

    rt_kprintf("bad block:\n");
    for (blk = 0; blk < mtd_dev->block_total; blk++)
    {
        if (rt_mtd_nand_check_block(mtd_dev, blk))
        {
            rt_kprintf("0x%x\n", blk);
        }
    }
}

rt_err_t mini_ftl_test_one_flash_block(rt_uint32_t addr)
{
    rt_uint8_t *buffer = NULL;
    rt_uint32_t block_size = mtd_dev->pages_per_block * mtd_dev->page_size;
    rt_uint32_t i, j;
    rt_err_t ret;

    if (addr % block_size)
    {
        rt_kprintf("addr not block aligned, block_size=%x\n", addr, block_size);
        return -RT_EINVAL;
    }

    buffer = (rt_uint8_t *)rt_malloc(block_size);
    if (!buffer)
    {
        rt_kprintf("alloc block buf size %d fail\n", block_size);
        return -RT_EINVAL;
    }
    for (i = 0; i < mtd_dev->pages_per_block; i++)
    {
        rt_memset(buffer + i * mtd_dev->page_size, i, mtd_dev->page_size);
    }

    ret = mini_ftl_erase(mtd_dev, addr, block_size);
    if (ret)
    {
        rt_kprintf("mini_ftl_erase failed, ret=%d\n", ret);
        goto out;
    }

    ret = mini_ftl_write(mtd_dev, buffer, addr, block_size);
    if (ret != block_size)
    {
        rt_kprintf("mini_ftl_write failed, ret=%d\n", ret);
        goto out;
    }

    ret = mini_ftl_read(mtd_dev, buffer, addr, block_size);
    if (ret != block_size)
    {
        rt_kprintf("mini_ftl_read failed, ret=%d\n", ret);
        goto out;
    }

    for (i = 0; i < mtd_dev->pages_per_block; i++)
    {
        for (j = 0; j < mtd_dev->page_size; j++)
        {
            if (buffer[i * mtd_dev->page_size + j] != i)
            {
                rt_kprintf("%s page=0x%x, index=0x%x buffer=0x%x\n", __func__, i, j, buffer[j]);
                ret = -RT_ERROR;
                break;
            }
        }
    }

    if (!ret)
    {
        rt_kprintf("%s success!\n", __func__);
    }
out:
    rt_free(buffer);

    return ret;
}

#ifdef RT_USING_DFS
static rt_err_t dfs_part_list(void)
{
    rt_uint8_t *buffer = NULL;
    rt_uint32_t page_size = mtd_dev->page_size;
    rt_uint32_t i;
    rt_err_t ret;
    struct dfs_partition part;

    buffer = (rt_uint8_t *)rt_malloc(page_size);
    if (!buffer)
    {
        rt_kprintf("alloc page buf size %d fail\n", page_size);
        return -RT_EINVAL;
    }

    ret = mini_ftl_read(mtd_dev, buffer, 0, page_size);
    if (ret != page_size)
    {
        rt_kprintf("mini_ftl_read failed, ret=%d\n", ret);
        goto out;
    }

    for (i = 0; i < RK_PARTITION_MAX_PARTITION; i++)
    {
        ret = dfs_filesystem_get_partition(&part, buffer, 4, i);
        if (ret == RT_EOK)
        {
            rt_kprintf("part%d type=%x off=%x size=%x\n", i, part.type, part.offset, part.size);
        }
        else
        {
            break;
        }
    }
out:
    rt_free(buffer);

    return ret;
}
#else
static rt_err_t dfs_part_list(void)
{
    rt_kprintf("define RT_USING_DFS firstly!\n");

    return 0;
}
#endif

static int32_t rk_part_list(void)
{
    struct rt_flash_partition *part;
    uint32_t partnum, i;

    partnum = get_rk_partition(&part);
    if (partnum)
    {
        for (i = 0; i < partnum; i++)
        {
            rt_kprintf("rt_flash_partition flags=%08x type= %08x off=%08x size=%08x %s\n",
                       part[i].mask_flags,
                       part[i].type,
                       part[i].offset,
                       part[i].size,
                       part[i].name);
        }
    }

    return 0;
}

void spinand_test(int argc, char **argv)
{
    char *cmd;
    rt_uint8_t *buffer = NULL;
    uint32_t page_addr, page_num, block_addr, blocks, loop, start_time, end_time, cost_time, addr;
    int i;
    rt_err_t ret;

    if (argc < 2)
        goto out;

    mtd_dev = (struct rt_mtd_nand_device *)rt_device_find("spinand0");
    if (mtd_dev == RT_NULL)
    {
        rt_kprintf("Did not find device: spinand0....\n");
        return;
    }

    cmd = argv[1];
    if (!rt_strcmp(cmd, "write"))
    {
        if (argc != 4)
            goto out;
        page_addr = atoi(argv[2]);
        page_num = atoi(argv[3]);

        if ((page_addr + page_num) / mtd_dev->pages_per_block > mtd_dev->block_end)
        {
            rt_kprintf("write over block end=%d\n", mtd_dev->block_end);
            return;
        }

        buffer = (rt_uint8_t *)rt_malloc_align(mtd_dev->page_size, 64);
        if (!buffer)
        {
            rt_kprintf("spi write alloc buf size %d fail\n", mtd_dev->page_size);
            return;
        }

        for (i = 0; i < mtd_dev->page_size; i++)
            buffer[i] = i % 256;

        start_time = HAL_GetTick();
        for (i = 0; i < page_num; i++)
        {
            ret = rt_mtd_nand_write(mtd_dev, page_addr + i,
                                    (const rt_uint8_t *)buffer, mtd_dev->page_size,
                                    RT_NULL, 0);
            if (ret)
            {
                rt_kprintf("%s write failed, ret=%d\n", __func__, ret);
                break;
            }
        }
        end_time = HAL_GetTick();
        cost_time = (end_time - start_time); /* ms */

        spinand_dbg_hex("write: ", buffer, 4, 16);
        rt_kprintf("spinand write speed: %ld KB/s\n", mtd_dev->page_size / cost_time);

        rt_free_align(buffer);
    }
    else if (!rt_strcmp(cmd, "read"))
    {
        if (argc != 4)
            goto out;
        page_addr = atoi(argv[2]);
        page_num = atoi(argv[3]);

        if ((page_addr + page_num) / mtd_dev->pages_per_block > mtd_dev->block_end)
        {
            rt_kprintf("write over block end=%d\n", mtd_dev->block_end);
            return;
        }

        buffer = (rt_uint8_t *)rt_malloc_align(mtd_dev->page_size, 64);
        if (!buffer)
        {
            rt_kprintf("spi read alloc buf size %d fail\n", mtd_dev->page_size);
            return;
        }

        for (i = 0; i < mtd_dev->page_size; i++)
            buffer[i] = i % 256;

        start_time = HAL_GetTick();
        for (i = 0; i < page_num; i++)
        {
            ret = rt_mtd_nand_read(mtd_dev, page_addr + i,
                                   (rt_uint8_t *)buffer, mtd_dev->page_size,
                                   RT_NULL, 0);
            if (ret)
            {
                rt_kprintf("%s write failed, ret=%d\n", __func__, ret);
                break;
            }
        }
        end_time = HAL_GetTick();
        cost_time = (end_time - start_time); /* ms */

        spinand_dbg_hex("read: ", buffer, 4, 16);
        rt_kprintf("spinand read speed: %ld KB/s\n", mtd_dev->page_size / cost_time);

        rt_free_align(buffer);
    }
    else if (!rt_strcmp(cmd, "erase"))
    {
        if (argc != 3)
            goto out;
        block_addr = atoi(argv[2]);

        if (block_addr > mtd_dev->block_end)
        {
            rt_kprintf("write over block end=%d\n", mtd_dev->block_end);
            return;
        }

        start_time = HAL_GetTick();
        rt_mtd_nand_erase_block(mtd_dev, block_addr);
        end_time = HAL_GetTick();
        cost_time = (end_time - start_time); /* ms */

        rt_kprintf("spinand erase speed: %ld KB/s\n", mtd_dev->page_size * mtd_dev->pages_per_block / cost_time);
    }
    else if (!rt_strcmp(cmd, "erase_all"))
    {
        if (argc != 3)
            goto out;
        block_addr = atoi(argv[2]);

        g_nand_ops.erase_blk = erase_blk;
        g_nand_ops.read_page = read_page;
        g_nand_ops.prog_page = prog_page;

        flash_erase_all_block(block_addr, mtd_dev->block_total);
        rt_kprintf("erase all finished\n");
    }
    else if (!rt_strcmp(cmd, "stress"))
    {
        if (argc != 5)
            goto out;
        loop = atoi(argv[2]);
        block_addr = atoi(argv[3]);
        blocks = atoi(argv[4]);

        nand_utils_test(loop, block_addr, block_addr + blocks);
    }
    else if (!rt_strcmp(cmd, "bbt"))
    {
        if (argc != 2)
            goto out;

        nand_bbt();
    }
    else if (!rt_strcmp(cmd, "markbad"))
    {
        if (argc != 3)
            goto out;
        block_addr = atoi(argv[2]);

        ret = rt_mtd_nand_mark_badblock(mtd_dev, block_addr);
        if (ret)
        {
            rt_kprintf("markbad failed, ret=%d\n", ret);
        }
    }
    else if (!rt_strcmp(cmd, "mini_ftl_test"))
    {
        if (argc != 3)
            goto out;
        addr = atoi(argv[2]);

        ret = mini_ftl_test_one_flash_block(addr);
        if (ret)
        {
            rt_kprintf("mini_ftl test failed, ret=%d\n", ret);
        }
    }
    else if (!rt_strcmp(cmd, "part_list"))
    {
        dfs_part_list();
        rk_part_list();
    }
    else
    {
        goto out;
    }

    return;
out:
    spinand_test_show_usage();
    return;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(spinand_test, spt test cmd);
#endif

#endif
