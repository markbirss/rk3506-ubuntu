#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "main.h"

#define TRIGGLE_PATH "/dev/rk_flexbus_adda"
#define FLEXBUS_PATH "/sys/kernel/debug/2a2f0000.rk-flexbus:rk-flexbus-adda/adc_buf/adc_buf_show"
#define RATE_PATH    "/sys/kernel/debug/clk/ref_clk1_out_pll/clk_rate"

static int samplerate = 100000000;

void flexbus_set_rate(int _rate)
{
    char cmd[1024];
    int rate;
    int ret;

    rate = (int)(_rate / 1000) * 1000;
    if (rate < _rate)
        rate += 1000;

    if (rate == samplerate)
        return;

    samplerate = rate;
    snprintf(cmd, sizeof(cmd), "echo %d > "RATE_PATH, samplerate);
    ret = system(cmd);
    printf("%s = %d\n", cmd, ret);
}

int flexbus_get_rate(void)
{
    return samplerate;
}

int flexbus_read(int32_t *x, int32_t *y, int32_t count)
{
    char cmd[1024];
    char buf[1024];
    int cx, cy;
    int idx = 0;
    int ret;
    FILE *fd;

    snprintf(cmd, sizeof(cmd), "echo rx %d 1 > "TRIGGLE_PATH, count);
    ret = system(cmd);
    printf("%s = %d\n", cmd, ret);

    fd = fopen(FLEXBUS_PATH, "rb");
    ret = fscanf(fd, "%s %s %s %s\n", buf, buf, buf, buf);

    while (1)
    {
        ret = fscanf(fd, "%d %d\n", &cx, &cy);
        if (ret == -1)
        {
            printf("Read end of file %d\n", ret);
            break;
        }
        if (ret == 2)
        {
//            printf("data %d %d\n", cx, cy);
            x[idx] = cx;
            y[idx] = cy;
            idx++;
        }
    }

    fclose(fd);

    return 1;
}

