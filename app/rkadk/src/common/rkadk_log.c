/* Copyright (c) Rockchip Electronics Co. Ltd. */

#include "rkadk_log.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef OS_RTT
static int m_rkadk_log_level = RKADK_LOG_LEVEL_WARN;
#else
static int m_rkadk_log_level = RKADK_LOG_LEVEL_INFO;
#endif

static int m_rkadk_log_cnt = 0;

int get_log_level() {
    if (m_rkadk_log_cnt == 0) {
        if (getenv("rkadk_log_level"))
            m_rkadk_log_level = atoi(getenv("rkadk_log_level"));

        m_rkadk_log_cnt++;
    }

    return m_rkadk_log_level;
}

static void _rkadk_log(int level, const char *fmt, const char *fname,
                             const int row, va_list args) {
    char line[256];
    char tag = ' ';

    if (level > get_log_level())
        return;

    switch(level) {
      case RKADK_LOG_LEVEL_PRINT:
        tag = 'P';
        break;

      case RKADK_LOG_LEVEL_ERROR:
        tag = 'E';
        break;

      case RKADK_LOG_LEVEL_WARN:
        tag = 'W';
        break;

      case RKADK_LOG_LEVEL_INFO:
        tag = 'I';
        break;

      case RKADK_LOG_LEVEL_DEBUG:
        tag = 'D';
        break;
    }

    snprintf(line, sizeof(line), "[%s_%c] {%-25.25s:%03d} %s\n", MODULE_TAG, tag, fname, row, fmt);
    vfprintf(stdout, line, args);
}

void rkadk_log(int level, const char *fmt,
                const char *fname, const int row, ...) {
    va_list args;
    va_start(args, row);
    _rkadk_log(level, fmt, fname, row, args);
    va_end(args);
}
