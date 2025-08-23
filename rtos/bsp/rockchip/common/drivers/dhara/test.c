/* Dhara - NAND flash management layer
 * Copyright (C) 2013 Daniel Beer <dlbeer@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <rtdevice.h>
#include <rtthread.h>

#include <string.h>
#include "bytes.h"
#include "map.h"

int dhara_trace_p_count;
int dhara_trace_e_count;
int dhara_trace_r_count;

#ifdef DHARA_RANDOM_TEST
static uint8_t buf[2048];
static int recheck = 0;
#define NUM_SECTORS     2464
#define MAX_SECTORS     2464
static dhara_sector_t sector_list[NUM_SECTORS];

void seq_gen(unsigned int seed, uint8_t *buf, size_t length)
{
    size_t i;

    srand(seed);
    for (i = 0; i < length; i++)
        buf[i] = rand();
}

void seq_assert(unsigned int seed, const uint8_t *buf, size_t length)
{
    size_t i;

    srand(seed);
    for (i = 0; i < length; i++)
    {
        const uint8_t expect = rand();

        if (buf[i] != expect)
        {
            rt_kprintf("seq_assert: mismatch at %ld in sequence %d: 0x%02x (expected 0x%02x)\n", i, seed, buf[i], expect);
            abort();
        }
    }
}

static void mt_write(struct dhara_map *m, dhara_sector_t s, int seed)
{
    dhara_error_t err;

    seq_gen(seed, buf, sizeof(buf));
    if (dhara_map_write(m, s, buf, &err) < 0)
    {
        rt_kprintf("map_write %d\n", err);
        abort();
    }

    if (dhara_map_sync(m, NULL) < 0)
    {
        rt_kprintf("map_sync %d\n", err);
        abort();
    }
}

static void mt_assert(struct dhara_map *m, dhara_sector_t s, int seed, bool unknown)
{
    dhara_error_t err = DHARA_E_NOT_FOUND;

    if (dhara_map_read(m, s, buf, &err) < 0)
    {
        rt_kprintf("map_read %d\n", err);
        abort();
    }

    if (err != DHARA_E_NOT_FOUND)
    {
        seq_assert(seed, buf, sizeof(buf));
        if (unknown)
            recheck++;
    }
}

static void shuffle(int seed)
{
    int i;

    srand(seed);
    for (i = 0; i < NUM_SECTORS; i++)
        sector_list[i] = rand() % MAX_SECTORS;

    for (i = NUM_SECTORS - 1; i > 0; i--)
    {
        const int j = rand() % i;
        const int tmp = sector_list[i];

        sector_list[i] = sector_list[j];
        sector_list[j] = tmp;
    }
}

int dhara_rand_test(struct dhara_map *m, int seed)
{
    int i, loop = 0;
    int a, b, c, d;
    int gap = 0x200;
    int i_cur = 0;
    uint32_t start_time, end_time, cost_time, size;

    shuffle(seed);
    while (1)
    {
        loop++;
        for (i_cur = 0; i_cur < NUM_SECTORS; i_cur += gap)
        {
            for (i = i_cur; i < i_cur + gap; i++)
            {
                const dhara_sector_t s = sector_list[i];
                mt_write(m, s, s);
                mt_assert(m, s, s, false);
                if (!(i & 0x2FF))
                {
                    rt_kprintf("%s loop=%d s=%d recheck=%d p=%d r=%d\n", __func__, loop, s, recheck, m->prog_total, m->read_total);
                }
            }

            a = m->prog_total;
            b = dhara_trace_p_count;
            c = dhara_trace_e_count;
            d = dhara_trace_r_count;
            start_time = HAL_GetTick();
            for (i = i_cur; i < i_cur + gap; i++)
            {
                const dhara_sector_t s = sector_list[i];

                mt_write(m, s, s);
            }
            end_time = HAL_GetTick();
            cost_time = (end_time - start_time);
            size = 2048 * gap;

            a = m->prog_total - a;
            b = dhara_trace_p_count - b;
            c = dhara_trace_e_count - c;
            d = dhara_trace_r_count - d;
            rt_kprintf("======= prog: p-P/E/R = %d-%d/%d/%d, speed %dKB/s\n", a, b, c, d, size / cost_time);

            a = m->read_total;
            b = dhara_trace_p_count;
            c = dhara_trace_e_count;
            d = dhara_trace_r_count;
            start_time = HAL_GetTick();
            for (i = i_cur; i < i_cur + gap; i++)
            {
                const dhara_sector_t s = sector_list[i];

                mt_assert(m, s, s, false);
            }
            end_time = HAL_GetTick();
            cost_time = (end_time - start_time);
            size = 2048 * gap;

            a = m->read_total - a;
            b = dhara_trace_p_count - b;
            c = dhara_trace_e_count - c;
            d = dhara_trace_r_count - d;
            rt_kprintf("======= read: r-P/E/R = %d-%d/%d/%d, speed %dKB/s\n", a, b, c, d, size / cost_time);
        }
    }

    return 0;
}
#else
int dhara_rand_test(struct dhara_map *m, int seed)
{
    return 0;
}
#endif