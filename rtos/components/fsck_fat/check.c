/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (C) 1995, 1996, 1997 Wolfgang Solfrank
 * Copyright (c) 1995 Martin Husemann
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <rtthread.h>
#include <rtdevice.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "ext.h"
#include "fsutil.h"

static int alwaysyes = 1;	/* assume "yes" for all questions */
static int preen = 1;		/* set when preening */

/*VARARGS*/
int ask(int def, const char *fmt, ...)
{
	va_list ap;
	char prompt[256];

	if (alwaysyes)
		def = alwaysyes;

	if (preen) {
		if (def)
			printf("FIXED\n");
		return def;
	}

	va_start(ap, fmt);
	vsnprintf(prompt, sizeof(prompt), fmt, ap);
	va_end(ap);

	if (alwaysyes) {
		printf("%s? %s\n", prompt, def ? "yes" : "no");
		return def;
	}

	return 0;
}

int checkfilesys(const char *fname)
{
	int dosfs = 0;
	struct bootblock boot;
	struct fat_descriptor *fat = NULL;
	int finish_dosdirsection=0;
	int mod = 0;
	int ret = 8;
	int64_t freebytes;
	int64_t badbytes;
	rt_device_t dev;

	fsck_vol_base = 0;

	dev = fsck_device_init(fname);
	if (dev == RT_NULL)
		return 8;

	if (fsck_boot_buf_init()) {
		printf("boot buf malloc error\n");
		ret = 8;
		goto out;
	}

	if (fsck_find_vol()) {
		printf("find volume error\n");
		ret = 8;
		goto out;
	}

	if (readboot(dosfs, &boot) == FSFATAL) {
		printf("\n");
		ret = 8;
		goto out;
	}

	if (preen && checkdirty(dosfs, &boot)) {
		printf("%s: ", fname);
		printf("FILESYSTEM CLEAN; SKIPPING CHECKS\n");
		ret = 0;
		goto out;
	}

	if (!preen)  {
		printf("** Phase 1 - Read FAT and checking connectivity\n");
	}

	mod |= readfat(dosfs, &boot, &fat);
	if (mod & FSFATAL) {
		ret = 8;
		goto out;
	}

	if (!preen)
		printf("** Phase 2 - Checking Directories\n");

	mod |= resetDosDirSection(fat);
	finish_dosdirsection = 1;
	if (mod & FSFATAL) {
		ret = 8;
		goto out;
	}

	/* delay writing FATs */
	mod |= handleDirTree(fat);
	if (mod & FSFATAL)
	{
		ret = 8;
		goto out;
	}

	if (!preen)
		printf("** Phase 3 - Checking for Lost Files\n");

	mod |= checklost(fat);
	if (mod & FSFATAL) {
		ret = 8;
		goto out;

	}

	/* now write the FATs */
	if (mod & FSFATMOD) {
		if (ask(1, "Update FATs")) {
			mod |= writefat(fat);
			if (mod & FSFATAL)
				goto out;
		} else
			mod |= FSERROR;
	}

	freebytes = (int64_t)boot.NumFree * boot.ClusterSize;
	badbytes = (int64_t)boot.NumBad * boot.ClusterSize;

	if (boot.NumBad)
		pwarn("%d files, %jd KiB free (%d clusters), %jd KiB bad (%d clusters)\n",
		      boot.NumFiles, (intmax_t)freebytes / 1024, boot.NumFree,
		      (intmax_t)badbytes / 1024, boot.NumBad);
	else
		pwarn("%d files, %jd KiB free (%d clusters)\n",
		      boot.NumFiles, (intmax_t)freebytes / 1024, boot.NumFree);

	if (mod && (mod & FSERROR) == 0) {
		if (mod & FSDIRTY) {
			if (ask(1, "MARK FILE SYSTEM CLEAN") == 0)
				mod &= ~FSDIRTY;

			if (mod & FSDIRTY) {
				pwarn("MARKING FILE SYSTEM CLEAN\n");
				mod |= cleardirty(fat);
			} else {
				pwarn("\n***** FILE SYSTEM IS LEFT MARKED AS DIRTY *****\n");
				mod |= FSERROR; /* file system not clean */
			}
		}
	}

	if (mod & (FSFATAL | FSERROR))
		goto out;

	ret = 0;

out:
	if (finish_dosdirsection)
		finishDosDirSection();
	if (fat)
		rt_free(fat);

	fsck_boot_buf_free();
	fsck_device_close();

	if (mod & (FSFATMOD|FSDIRMOD)){
		pwarn("\n***** FILE SYSTEM WAS MODIFIED *****\n");
		return 4;
	}

	return ret;
}
