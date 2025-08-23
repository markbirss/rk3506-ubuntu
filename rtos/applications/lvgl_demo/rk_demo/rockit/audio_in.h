/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
*/
#ifndef __AUDIO_IN_H__
#define __AUDIO_IN_H__

int ai_init(void);
int ai_fetch(int (*hook)(void *, char *, int), void *arg);
int ai_deinit(void);

#endif

