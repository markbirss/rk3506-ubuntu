// Copyright 2013 Google Inc. All Rights Reserved.
//
// Log - implemented using the standard Android logging mechanism

/*
 * Qutoing from system/core/include/log/log.h:
 * Normally we strip ALOGV (VERBOSE messages) from release builds.
 * You can modify this (for example with "#define LOG_NDEBUG 0"
 * at the top of your source file) to change that behavior.
 */
#include "log.h"
#include <stdio.h>
#include <stdarg.h>

#define LOG_BUF_SIZE 1024
static int LOG_LEVEL = LOG_DEBUG;


void InitLogging(LogPriority level)
{
    if (level > 0)
        LOG_LEVEL = level;
}

void Log(const char* file, int line, LogPriority level, const char* fmt, ...)
{
    if (level > LOG_LEVEL) {
        return;
    }

    va_list ap;
    char buf[LOG_BUF_SIZE];
    va_start(ap, fmt);
    vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
    va_end(ap);

    switch (level) {
    case LOG_ERROR:
        printf("LOG_ERROR: %s", buf);
        break;
    case LOG_WARN:
        printf("LOG_WARN: %s", buf);
        break;
    case LOG_INFO:
        printf("LOG_INFO: %s", buf);
        break;
    case LOG_DEBUG:
        printf("LOG_DEBUG: %s", buf);
        break;
    default :
        break;
    }
}
