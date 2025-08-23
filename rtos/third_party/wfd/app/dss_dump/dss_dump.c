/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <common/dss_service.h>

int dss_dump(int argc, char *argv[])
{
	bool dump_all;

	if (argc < 1 || argc > 2) {
		printf("Usage: dss_dump [a/all]\n");
		return 0;
	}

	if (argc == 1) {
		dump_all = false;
	} else if (!strcmp(argv[1], "a") || !strcmp(argv[1], "all")) {
		dump_all = true;
	} else {
		printf("Usage: dss_dump [a/all]\n");
		return 0;
	}

	dss_regs_dump(dump_all);

	return 0;
}

#if defined(RT_USING_FINSH)
#include <finsh.h>
MSH_CMD_EXPORT(dss_dump, display related regs dump. e.g: dss_dump);
#endif