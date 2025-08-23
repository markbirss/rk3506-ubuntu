#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "soc.h"

char *get_soc_name(char *buf)
{
    char *start;
    char *ret;
    int size;

    start = strstr(buf, "rk");
    ret = strdup(start);
    size = strlen(ret);

    ret[0] = 'R';
    ret[1] = 'K';
    for (int i = 2; i < size; i++)
    {
        char c = *(ret + i);
        if (((c > '9') || (c < '0')) &&
                ((c != 'x') || (c != 'X')))
        {
            *(ret + i) = '\0';
            break;
        }
        *(ret + i) = toupper(c);
    }

    return ret;
}

char *get_compatible_name(void)
{
    const char *path = "/proc/device-tree/compatible";
    int fd = open(path, O_RDONLY);
    char name[128];
    int size = sizeof(name);
    ssize_t soc_name_len = 0;

    if (fd < 0)
    {
        printf("open %s error\n", path);
        return NULL;
    }

    snprintf(name, size - 1, "unknown");
    soc_name_len = read(fd, name, size - 1);
    if (soc_name_len > 0)
    {
        name[soc_name_len] = '\0';
        /* replacing the termination character to space */
        for (char *ptr = name;; ptr = name)
        {
            ptr += strnlen(name, size);
            if (ptr >= name + soc_name_len - 1)
                break;
            *ptr = ' ';
        }

        printf("chip name: %s\n", name);
    }
    else
    {
        printf("read failed %d\n", soc_name_len);
    }

    close(fd);

    return strdup(name);
}

char *get_kernel_version(char *buf)
{
    char *info = NULL;
    char *ret;
    int size;

    if (!buf)
        info = get_system_version();
    else
        info = buf;

    ret = strdup(info + (sizeof("Linux version ") - 1));
    size = strlen(ret);
    for (int i = 0; i < size; i++)
    {
        char c = *(ret + i);
        if (c == ' ')
        {
            *(ret + i) = '\0';
            break;
        }
    }

    if (!buf)
        free(info);

    return ret;
}

char *get_system_version(void)
{
    const char *path = "/proc/version";
    int fd = open(path, O_RDONLY);
    char info[1024];
    int size = sizeof(info);
    ssize_t len = 0;

    if (fd < 0)
    {
        printf("open %s error\n", path);
        return NULL;
    }

    snprintf(info, size - 1, "unknown");
    len = read(fd, info, size - 1);
    if (len <= 0)
    {
        printf("read failed %d\n", len);
        return NULL;
    }
    info[len] = '\0';

    close(fd);

    return strdup(info);
}

