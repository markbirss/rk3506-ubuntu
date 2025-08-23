/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2010-02-10     Bernard      first version
 * 2020-04-12     Jianjia Ma   add msh cmd
 */

#include <rtthread.h>
#include <dfs_file.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/statfs.h>

#define TEST_DATA_LEN     120

/* file read write test */
void readwrite(const char *filename)
{
    int fd;
    int index, length;
    char *test_data;
    char *buffer;
    int block_size = TEST_DATA_LEN;

    /* open with write only & create */
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0);
    if (fd < 0)
    {
        rt_kprintf("open file for write failed\n");
        return;
    }

    test_data = rt_malloc(block_size);
    if (test_data == RT_NULL)
    {
        rt_kprintf("no memory\n");
        close(fd);
        return;
    }

    buffer = rt_malloc(block_size);
    if (buffer == RT_NULL)
    {
        rt_kprintf("no memory\n");
        close(fd);
        rt_free(test_data);
        return;
    }

    /* prepare some data */
    for (index = 0; index < block_size; index ++)
    {
        test_data[index] = index + 27;
    }

    /* write to file */
    length = write(fd, test_data, block_size);
    if (length != block_size)
    {
        rt_kprintf("write data failed\n");
        close(fd);
        goto __exit;
    }

    /* close file */
    close(fd);

    /* reopen the file with append to the end */
    fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0);
    if (fd < 0)
    {
        rt_kprintf("open file for append write failed\n");
        goto __exit;;
    }

    length = write(fd, test_data, block_size);
    if (length != block_size)
    {
        rt_kprintf("append write data failed\n");
        close(fd);
        goto __exit;
    }
    /* close the file */
    close(fd);

    /* open the file for data validation. */
    fd = open(filename, O_RDONLY, 0);
    if (fd < 0)
    {
        rt_kprintf("check: open file for read failed\n");
        goto __exit;
    }

    /* read the data (should be the data written by the first time ) */
    length = read(fd, buffer, block_size);
    if (length != block_size)
    {
        rt_kprintf("check: read file failed\n");
        close(fd);
        goto __exit;
    }

    /* validate */
    for (index = 0; index < block_size; index ++)
    {
        if (test_data[index] != buffer[index])
        {
            rt_kprintf("check: check data failed at %d\n", index);
            close(fd);
            goto __exit;
        }
    }

    /* read the data (should be the second time data) */
    length = read(fd, buffer, block_size);
    if (length != block_size)
    {
        rt_kprintf("check: read file failed\n");
        close(fd);
        goto __exit;
    }

    /* validate */
    for (index = 0; index < block_size; index ++)
    {
        if (test_data[index] != buffer[index])
        {
            rt_kprintf("check: check data failed at %d\n", index);
            close(fd);
            goto __exit;
        }
    }

    /* close the file */
    close(fd);
    /* print result */
    rt_kprintf("read/write test successful!\n");

__exit:
    rt_free(test_data);
    rt_free(buffer);
}

#define WRITE_RECORD_NUM           10
#define MAX_WRITE_RECORD_LEN       (1024 * 1024)

struct write_record
{
    uint32_t pos;
    uint32_t len;
};

static struct write_record wr[WRITE_RECORD_NUM];

static void seek_read_write(const char *filename)
{
    int i, j, fd, len, ret;
    uint32_t *buf, write_file_size = 0, read_file_size = 0;
    uint32_t pos;

    buf = (uint32_t *)rt_malloc_align(MAX_WRITE_RECORD_LEN, sizeof(uint32_t));
    if (!buf)
    {
        rt_kprintf("allocate buffer failed\n");
        return;
    }

    /* open with write only & create */
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0);
    if (fd < 0)
    {
        rt_kprintf("open file for write failed\n");
        rt_free_align(buf);
        return;
    }

    for (i = 0; i < MAX_WRITE_RECORD_LEN / sizeof(uint32_t); i++)
    {
        buf[i] = 0x55aaaa55;
    }

    /* write with random size */
    for (i = 0; i < WRITE_RECORD_NUM; i++)
    {
        len = (rand() % MAX_WRITE_RECORD_LEN) & (~0x3);
        ret = write(fd, buf, len);
        if (ret != len)
        {
            rt_kprintf("write failed, ret=%d, len=%d\n", ret, len);
            rt_free_align(buf);
            close(fd);
            return;
        }
        write_file_size += len;
    }
    close(fd);

    /* reopen with read only */
    fd = open(filename, O_RDONLY, 0);
    if (fd < 0)
    {
        rt_kprintf("check: open file for read failed\n");
        rt_free_align(buf);
        return;
    }

    /* read & check data */
    memset(buf, 0, MAX_WRITE_RECORD_LEN);
    pos = 0;
    while (1)
    {
        len = (rand() % MAX_WRITE_RECORD_LEN) & (~0x3);
        ret = read(fd, buf, len);
        if (ret > 0)
        {
            for (i = 0; i < ret / sizeof(uint32_t); i++)
            {
                if (buf[i] != 0x55aaaa55)
                {
                    rt_kprintf("pattern 1: check read data failed, 0x%x@0x%x\n", buf[i], pos + (i * sizeof(uint32_t)));
                    rt_free_align(buf);
                    close(fd);
                    return;
                }
            }
            pos += ret;
        }

        /* end of file */
        if (ret < len)
        {
            read_file_size = pos;
            break;
        }
    };

    if (read_file_size != write_file_size)
    {
        rt_kprintf("pattern 1: file size not matched, write_file_size=%d, read_file_size=%d\n",
                   write_file_size, read_file_size);
        rt_free_align(buf);
        close(fd);
        return;
    }
    close(fd);

    /* open with write only */
    fd = open(filename, O_WRONLY, 0);
    if (fd < 0)
    {
        rt_kprintf("open file for write failed\n");
        rt_free_align(buf);
        return;
    }

    /* write with random seek */
    for (i = 0; i < WRITE_RECORD_NUM; i++)
    {
        len = (rand() % MAX_WRITE_RECORD_LEN) & (~0x3);
        pos = (rand() % (write_file_size - len)) & (~0x3);
        wr[i].pos = pos;
        wr[i].len = len;

        for (j = 0; j < len / sizeof(uint32_t); j++)
        {
            buf[j] = pos + (j * sizeof(uint32_t));
        }

        lseek(fd, pos, SEEK_SET);
        ret = write(fd, buf, len);
        if (ret != len)
        {
            rt_kprintf("pattern 2 write failed, ret=%d, len=%d\n", ret, len);
            rt_free_align(buf);
            close(fd);
            return;
        }
    }
    close(fd);

    /* reopen with read only */
    fd = open(filename, O_RDONLY, 0);
    if (fd < 0)
    {
        rt_kprintf("check: open file for read failed\n");
        rt_free_align(buf);
        return;
    }

    for (i = 0; i < WRITE_RECORD_NUM; i++)
    {
        memset(buf, 0, wr[i].len);
        lseek(fd, wr[i].pos, SEEK_SET);
        ret = read(fd, buf, wr[i].len);
        if (ret != wr[i].len)
        {
            rt_kprintf("pattern 2 read failed, ret=%d, len=%d\n", ret, wr[i].len);
            rt_free_align(buf);
            close(fd);
            return;
        }

        for (j = 0; j < wr[i].len / sizeof(uint32_t); j++)
        {
            if (buf[j] != wr[i].pos + (j * sizeof(uint32_t)))
            {
                rt_kprintf("pattern 2: check read data failed, 0x%x@0x%x\n", buf[j], wr[i].pos + (j * sizeof(uint32_t)));
                rt_free_align(buf);
                close(fd);
                return;
            }
        }
    }
    close(fd);
    rt_free_align(buf);

    rt_kprintf("all pattern for seek_read_write test success\n");
}

#ifdef RT_USING_FINSH
#include <finsh.h>
/* export to finsh */
FINSH_FUNCTION_EXPORT(readwrite, perform file read and write test);

static void cmd_readwrite(int argc, char *argv[])
{
    char *filename;
    int count = 0;

    if (argc == 3)
    {
        filename = argv[1];
        count = atoi(argv[2]);
    }
    else if (argc == 2)
    {
        filename = argv[1];
    }
    else
    {
        rt_kprintf("Usage: readwrite [file_path]\n");
        return;
    }

    if (count > 0)
    {
        while (--count >= 0)
        {
            seek_read_write(filename);
        }
    }
    else
    {
        readwrite(filename);
    }
}
MSH_CMD_EXPORT_ALIAS(cmd_readwrite, readwrite, perform file read and write test);
#endif /* RT_USING_FINSH */
