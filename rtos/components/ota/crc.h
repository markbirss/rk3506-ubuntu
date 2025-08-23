/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef _CRC_H
#define _CRC_H

#include "rkimage.h"

extern USHORT CRC_16(BYTE * aData, UINT aSize);
extern UINT CRC_32(PBYTE pData, UINT ulSize, UINT uiPreviousValue);
extern void P_RC4(BYTE * buf, USHORT len);
extern void bch_encode(BYTE *encode_in, BYTE *encode_out);
extern USHORT CRC_CCITT(UCHAR *p, UINT CalculateNumber);
extern void generate_gf();
extern void gen_poly();

#endif
