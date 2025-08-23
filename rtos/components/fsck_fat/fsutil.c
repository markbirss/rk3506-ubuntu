/*	$NetBSD: fsutil.c,v 1.15 2006/06/05 16:52:05 christos Exp $	*/

/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <rtthread.h>
#include <rtdevice.h>

#include "ff.h"
#include "diskio.h"
#include "fsutil.h"

static rt_device_t dev_id;
long fsck_vol_base = 0;
int fsck_block_size = 512;

rt_device_t fsck_device_init(const char *device_name)
{
    struct rt_device_blk_geometry geometry;

    dev_id = rt_device_find(device_name);

    rt_device_open(dev_id, RT_DEVICE_OFLAG_RDWR);

    if (RT_EOK == rt_device_control(dev_id, RT_DEVICE_CTRL_BLK_GETGEOME, &geometry))
        fsck_block_size = geometry.bytes_per_sector;
    else
        fsck_block_size = 512;

    return dev_id;
}

void fsck_device_close(void)
{
    rt_device_close(dev_id);
}

int fsck_file_read(BYTE pdrv, BYTE *buff, UINT len, LBA_t offset)
{
    rt_size_t result;
    int count = len / fsck_block_size;
    int lba = offset / fsck_block_size;

    if ((offset & (fsck_block_size - 1)) || (len & (fsck_block_size - 1)))
        return 0;

    result = rt_device_read(dev_id, lba + fsck_vol_base, buff, count);
    if (result == count)
    {
        return len;
    }

    return 0;
}

int fsck_file_write(BYTE pdrv, BYTE *buff, UINT len, LBA_t offset)
{
    rt_size_t result;
    int count = len / fsck_block_size;
    int lba = offset / fsck_block_size;

    if ((offset & (fsck_block_size - 1)) || (len & (fsck_block_size - 1)))
        return 0;

    result = rt_device_write(dev_id, lba + fsck_vol_base, buff, count);
    if (result == count)
    {
        return len;
    }

    return 0;
}
