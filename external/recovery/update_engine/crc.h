/*
 * Copyright (C) 2023 Rockchip Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _CRC_H
#define _CRC_H
#include "defineHeader.h"
extern USHORT CRC_16(BYTE * aData, UINT aSize);
extern UINT CRC_32(PBYTE pData, UINT ulSize, UINT uiPreviousValue);
extern void P_RC4(BYTE * buf, USHORT len);
extern void bch_encode(BYTE *encode_in, BYTE *encode_out);
extern USHORT CRC_CCITT(UCHAR *p, UINT CalculateNumber);
extern void generate_gf();
extern void gen_poly();
#endif
