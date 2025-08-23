/**
  * Copyright (c) 2024 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ***************************************************************************
  * @file    at_test.h
  * @author  Zain Wang
  * @version V1.0
  * @date    21-Mar-2024
  * @brief   auto test machine
  *
  ******************************************************************************
  */

#ifndef _AT_TEST_H_
#define _AT_TEST_H

#if (RTTHREAD_VERSION >= RT_VERSION_CHECK(4, 1, 0))
struct at_case
{
    const char *name;
    int (*func)(void);
};

struct at_test_info
{
    void *begin;
    int size;
};

#define DEFINE_AT_TEST_INFO(case)                   \
        struct at_test_info at_test_info = {        \
            .begin = (void *)case,                  \
            .size = sizeof(case),                   \
        };
#endif

#endif // _AT_TEST_H_
