#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "main.h"

#define RPMSG_PATH "/dev/ttyRPMSG0"

static int update = 0;
static int reset = 0;
static int32_t data[13] = {0,};
static int32_t data_min = 0;
static int32_t data_max = 0;
static int32_t data_avg = 0;
static int32_t data_cur = 0;

void rpmsg_reset(void)
{
    reset = 1;
}

int rpmsg_read(int32_t *y, int32_t *min, int32_t *max, int32_t *avg,
               int32_t *cur)
{
    if (!update)
        return 0;

    memcpy(y, data, sizeof(data));
    update = 0;
    *min = data_min;
    *max = data_max;
    *avg = data_avg;
    *cur = data_cur;

    return 1;
}

void *rpmsg_thread(void *argv)
{
    int32_t d[13] = {0,};
    float min, max, avg, cur;
    char *avg_str;
    char *data_str;
    int fd;
    char buf[256] = {0};
    int ret;

    fd = open(RPMSG_PATH, O_RDWR);
    ret = write(fd, buf, 1);

    while (1)
    {
        ret = read(fd, buf, sizeof(buf));
        if (ret > 0)
        {
            //printf("%s %d [%.*s]\n", __func__, ret, ret, buf);
            avg_str = strstr(buf, "avg =");
            data_str = strstr(buf, "0.5 =");
            if (avg_str)
            {
                sscanf(avg_str, "avg = %f us, max = %f us, min = %f us, cur = %f us", &avg,
                       &max, &min, &cur);
                data_min = min * 1000;
                data_max = max * 1000;
                data_avg = avg * 1000;
                data_cur = cur * 1000;
                printf("<avg = %f us, max = %f us, min = %f us, cur = %f us>\n", avg, max, min,
                       cur);
                update = 1;
            }
            if (data_str)
            {
                sscanf(data_str,
                       "0.5 = %d times, 1 = %d times, 1.5 = %d times, "
                       "2 = %d times, 2.5 = %d times, 3 = %d times, "
                       "3.5 = %d times, 4 = %d times, 4.5 = %d times, "
                       "5 = %d times, 5.5 = %d times, 6 = %d times",
                       &d[1], &d[2], &d[3], &d[4], &d[5], &d[6],
                       &d[7], &d[8], &d[9], &d[10], &d[11], &d[12]);
                printf(data_str,
                       "<0.5 = %d times, 1 = %d times, 1.5 = %d times, "
                       "2 = %d times, 2.5 = %d times, 3 = %d times, "
                       "3.5 = %d times, 4 = %d times, 4.5 = %d times, "
                       "5 = %d times, 5.5 = %d times, 6 = %d times>\n",
                       d[1], d[2], d[3], d[4], d[5], d[6],
                       d[7], d[8], d[9], d[10], d[11], d[12]);
                if (reset)
                {
                    reset = 0;
                    for (int i = 0; i < ARRAY_SIZE(d); i++)
                        data[i] = d[i];
                }
                else
                {
                    for (int i = 0; i < ARRAY_SIZE(d); i++)
                        data[i] += d[i];
                }
                update = 1;
            }
        }
        usleep(10 * 1000);
    }

    close(fd);
}

