#ifndef __DITHER_H
#define __DITHER_H

#include <linux/types.h>

enum {
	DITHER_TYPE_NONE,
	DITHER_TYPE_BAYER_4X4,
	DITHER_TYPE_BAYER_16X16,
	DITHER_TYPE_MAX,
};

const char *dither_get_name(u8 type);
void dither_gray8_to_bw(u8 type, const u8 *src, u8 *dst, int width, int height);

#endif /* __DETHER_H */
