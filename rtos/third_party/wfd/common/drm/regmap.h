/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef __REGMAP_H__
#define __REGMAP_H__

#include <drm/drm_util.h>

struct regmap {
	uint32_t start;
	uint32_t size;
	uintptr_t base;
};

static inline int regmap_read(struct regmap *map, uint32_t offset, uint32_t *valp)
{
	*valp = in32(map->base + offset);

	return 0;
}

static inline int regmap_write(struct regmap *map, uint32_t offset, uint32_t val)
{
	out32(map->base + offset, val);

	return 0;
}

static inline int regmap_update_bits(struct regmap *map, uint32_t offset, uint32_t mask, uint32_t val)
{
	uint32_t reg;
	int ret;

	ret = regmap_read(map, offset, &reg);
	if (ret)
		return ret;

	reg &= ~mask;

	return regmap_write(map, offset, reg | val);
}

#define regmap_read_poll_timeout(map, addr, val, cond, timeout_us)     \
({ \
	struct timespec ts,ts_end;                                      \
	uint64_t cur_time;                                      \
	clock_gettime(CLOCK_MONOTONIC, &ts);                    \
	uint64_t timeout =  timespec2nsec(&ts) / 1000 + timeout_us; \
	int __ret; \
	for (;;) { \
		__ret = regmap_read((map), (addr), &(val)); \
		if (__ret) \
			break; \
		if (cond) \
			break; \
		if (timeout_us) { \
			clock_gettime(CLOCK_MONOTONIC, &ts_end);                        \
			cur_time = timespec2nsec(&ts_end) / 1000;                       \
			if (cur_time > timeout) {\
				__ret = regmap_read((map), (addr), &(val)); \
				break; \
			}\
		} \
	} \
	__ret ?: ((cond) ? 0 : -ETIMEDOUT); \
})

#endif
