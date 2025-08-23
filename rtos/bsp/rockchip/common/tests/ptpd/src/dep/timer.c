/* timer.c */

#include "ptpd.h"
#include "hal_base.h"

static struct rt_timer ptptimer;
static UInteger32 local_time = 0;
static UInteger32 old_local_time = 0;
static unsigned int elapsed_ms;

static Boolean timer_init = FALSE;

void catchAlarm(void *param)
{
    local_time = HAL_GetTick() / 1000;
    elapsed_ms += local_time - old_local_time;
    old_local_time = local_time;
    DBGV("catch_alarm: elapsed %d\n", elapsed_ms);
}

void initTimer(void)
{
    DBG("initTimer\n");
    elapsed_ms = 0;
    if (!timer_init)
    {
        /* 1s  update */
        rt_timer_init(&ptptimer, "ptp_timer", catchAlarm, NULL, RT_TICK_PER_SECOND, RT_TIMER_FLAG_PERIODIC);
        timer_init = TRUE;
        old_local_time = HAL_GetTick() / 1000;
        rt_timer_start(&ptptimer);
    }
}

void timerUpdate(IntervalTimer *itimer)
{

    int i, delta;

    delta = elapsed_ms;
    elapsed_ms = 0;

    if (delta <= 0)
        return;


    DBGV("timerUpdate: timer1 interval: %d, left: %d, delta: %d\n", itimer[1].interval, itimer[1].left, delta);

    for (i = 0; i < TIMER_ARRAY_SIZE; ++i)
    {
        if ((itimer[i].interval) >= 0 && ((itimer[i].left) -= delta) <= 0)
        {
            itimer[i].left = itimer[i].interval;
            itimer[i].expire = TRUE;
            DBGV("timerUpdate: timer %u expired\n", i);
        }
    }

}

void timerStop(UInteger16 index, IntervalTimer *itimer)
{
    if (index >= TIMER_ARRAY_SIZE)
        return;

    itimer[index].interval = 0;
}

void timerStart(UInteger16 index, UInteger32 interval_ms, IntervalTimer *itimer)
{
    if (index >= TIMER_ARRAY_SIZE)
        return;

    itimer[index].expire = FALSE;

    itimer[index].left = interval_ms;

    itimer[index].interval = itimer[index].left;

    DBGV("timerStart: set timer %d to %d\n", index, interval_ms);
}

/*
 * This function arms the timer with a uniform range, as requested by page 105 of the standard (for sending delayReqs.)
 * actual time will be U(0, interval * 2.0);
 *
 * PTPv1 algorithm was:
 *    ptpClock->R = getRand(&ptpClock->random_seed) % (PTP_DELAY_REQ_INTERVAL - 2) + 2;
 *    R is the number of Syncs to be received, before sending a new request
 *
 */
void timerStart_random(UInteger16 index, float interval, IntervalTimer *itimer)
{
    float new_value;

    new_value = getRand() * interval * 2.0;
    DBG2(" timerStart_random: requested %.2f, got %.2f\n", interval, new_value);

    timerStart(index, new_value, itimer);
}

Boolean timerExpired(UInteger16 index, IntervalTimer *itimer)
{
    timerUpdate(itimer);

    if (index >= TIMER_ARRAY_SIZE)
        return FALSE;

    if (!itimer[index].expire)
        return FALSE;

    itimer[index].expire = FALSE;

    return TRUE;
}
