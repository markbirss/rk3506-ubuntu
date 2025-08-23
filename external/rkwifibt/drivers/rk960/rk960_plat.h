/*
 * Copyright (c) 2022, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef RK960_PLAT_H_INCLUDED
#define RK960_PLAT_H_INCLUDED

#include <linux/ioport.h>

struct rk960_platform_data {
	const char *mmc_id;
	const struct resource *irq;
	const struct resource *reset;
	int (*power_ctrl) (const struct rk960_platform_data * pdata,
			   bool enable);
	int (*clk_ctrl) (const struct rk960_platform_data * pdata, bool enable);
	int (*prcmu_ctrl) (const struct rk960_platform_data * pdata,
			   bool enable);
};

/* Declaration only. Should be implemented in arch/xxx/mach-yyy */
const struct rk960_platform_data *rk960_get_platform_data(void);

#endif /* RK960_PLAT_H_INCLUDED */
