/*
 * Copyright (c) 2022, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef RK960_SBUS_H
#define RK960_SBUS_H

/*
 * sbus priv forward definition.
 * Implemented and instantiated in particular modules.
 */
struct sbus_priv;

typedef void (*sbus_irq_handler) (void *priv);

struct sbus_ops {
	int (*sbus_memcpy_fromio) (struct sbus_priv * self, unsigned int addr,
				   void *dst, int count);
	int (*sbus_memcpy_toio) (struct sbus_priv * self, unsigned int addr,
				 const void *src, int count);
	void (*lock) (struct sbus_priv * self);
	void (*unlock) (struct sbus_priv * self);
	int (*irq_subscribe) (struct sbus_priv * self, sbus_irq_handler handler,
			      void *priv);
	int (*irq_unsubscribe) (struct sbus_priv * self);
	int (*reset) (struct sbus_priv * self);
	 size_t(*align_size) (struct sbus_priv * self, size_t size);
	int (*power_mgmt) (struct sbus_priv * self, bool suspend);
	int (*set_block_size) (struct sbus_priv * self, size_t size);
};

#endif /* RK960_SBUS_H */
