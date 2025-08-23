/*-
 * Copyright (c) 2011-2012 George V. Neville-Neil,
 *                         Steven Kreuzer,
 *                         Martin Burnicki,
 *                         Jan Breuer,
 *                         Gael Mace,
 *                         Alexandre Van Kempen,
 *                         Inaqui Delgado,
 *                         Rick Ratzel,
 *                         National Instruments.
 * Copyright (c) 2009-2010 George V. Neville-Neil,
 *                         Steven Kreuzer,
 *                         Martin Burnicki,
 *                         Jan Breuer,
 *                         Gael Mace,
 *                         Alexandre Van Kempen
 *
 * Copyright (c) 2005-2008 Kendall Correll, Aidan Williams
 *
 * All Rights Reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file   sys.c
 * @date   Tue Jul 20 16:19:46 2010
 *
 * @brief  Code to call kernel time routines and also display server statistics.
 *
 *
 */

#include "../ptpd.h"

#if defined(linux)
#  include <netinet/ether.h>
#elif defined( __FreeBSD__ )
#  include <net/ethernet.h>
#elif defined( __NetBSD__ )
#  include <net/if_ether.h>
#elif defined( __OpenBSD__ )
#  include <some ether header>  // force build error
#elif defined(RT_USING_GMAC_PTP)
#  include "drv_gmac.h"
#  include "hal_gmac.h"

extern rt_err_t rt_thread_sleep(rt_tick_t tick);
extern void *ptpd_get_handle(void);
#endif


/* only C99 has the round function built-in */
double round(double __x);


/*
 returns a static char * for the representation of time, for debug purposes
 DO NOT call this twice in the same printf!
*/
char *dump_TimeInternal(const TimeInternal *p)
{
    static char buf[100];

    snprint_TimeInternal(buf, 100, p);
    return buf;
}


/*
 displays 2 timestamps and their strings in sequence, and the difference between then
 DO NOT call this twice in the same printf!
*/
char *dump_TimeInternal2(const char *st1, const TimeInternal *p1, const char *st2, const TimeInternal *p2)
{
    static char buf[BUF_SIZE];
    int n = 0;

    /* display Timestamps */
    if (st1)
    {
        n += snprintf(buf + n, BUF_SIZE - n, "%s ", st1);
    }
    n += snprint_TimeInternal(buf + n, BUF_SIZE - n, p1);
    n += snprintf(buf + n, BUF_SIZE - n, "    ");

    if (st2)
    {
        n += snprintf(buf + n, BUF_SIZE - n, "%s ", st2);
    }
    n += snprint_TimeInternal(buf + n, BUF_SIZE - n, p2);
    n += snprintf(buf + n, BUF_SIZE - n, " ");

    /* display difference */
    TimeInternal r;
    subTime(&r, p1, p2);
    n += snprintf(buf + n, BUF_SIZE - n, "   (diff: ");
    n += snprint_TimeInternal(buf + n, BUF_SIZE - n, &r);
    n += snprintf(buf + n, BUF_SIZE - n, ") ");

    return buf;
}



int
snprint_TimeInternal(char *s, int max_len, const TimeInternal *p)
{
    int len = 0;

    /* always print either a space, or the leading "-". This makes the stat files columns-aligned */
    len += snprintf(&s[len], max_len - len, "%c",
                    isTimeInternalNegative(p) ? '-' : ' ');

    len += snprintf(&s[len], max_len - len, "%d.%09d",
                    abs(p->seconds), abs(p->nanoseconds));

    return len;
}


/* debug aid: convert a time variable into a static char */
char *time2st(const TimeInternal *p)
{
    static char buf[1000];

    snprint_TimeInternal(buf, sizeof(buf), p);
    return buf;
}



void DBG_time(const char *name, const TimeInternal  p)
{
    DBG("             %s:   %s\n", name, time2st(&p));

}


char *
translatePortState(PtpClock *ptpClock)
{
    char *s;
    switch (ptpClock->portState)
    {
    case PTP_INITIALIZING:
        s = "init";
        break;
    case PTP_FAULTY:
        s = "flt";
        break;
    case PTP_LISTENING:
        /* seperate init-reset from real resets */
        if (ptpClock->reset_count == 1)
        {
            s = "lstn_init";
        }
        else
        {
            s = "lstn_reset";
        }
        break;
    case PTP_PASSIVE:
        s = "pass";
        break;
    case PTP_UNCALIBRATED:
        s = "uncl";
        break;
    case PTP_SLAVE:
        s = "slv";
        break;
    case PTP_PRE_MASTER:
        s = "pmst";
        break;
    case PTP_MASTER:
        s = "mst";
        break;
    case PTP_DISABLED:
        s = "dsbl";
        break;
    default:
        s = "?";
        break;
    }
    return s;
}


int
snprint_ClockIdentity(char *s, int max_len, const ClockIdentity id)
{
    int len = 0;
    int i;

    for (i = 0; ;)
    {
        len += snprintf(&s[len], max_len - len, "%02x", (unsigned char) id[i]);

        if (++i >= CLOCK_IDENTITY_LENGTH)
            break;
    }

    return len;
}


/* show the mac address in an easy way */
int
snprint_ClockIdentity_mac(char *s, int max_len, const ClockIdentity id)
{
    int len = 0;
    int i;

    for (i = 0; ;)
    {
        /* skip bytes 3 and 4 */
        if (!((i == 3) || (i == 4)))
        {
            len += snprintf(&s[len], max_len - len, "%02x", (unsigned char) id[i]);

            if (++i >= CLOCK_IDENTITY_LENGTH)
                break;

            /* print a separator after each byte except the last one */
            len += snprintf(&s[len], max_len - len, "%s", ":");
        }
        else
        {

            i++;
        }
    }

    return len;
}


/*
 * wrapper that caches the latest value of ether_ntohost
 * this function will NOT check the last accces time of /etc/ethers,
 * so it only have different output on a failover or at restart
 *
 */
int ether_ntohost_cache(char *hostname, struct ether_addr *addr)
{
    static int valid = 0;
    static struct ether_addr prev_addr;
    static char buf[BUF_SIZE];

#if defined(linux) || defined(__NetBSD__)
    if (memcmp(addr->ether_addr_octet, &prev_addr,
               sizeof(struct ether_addr)) != 0)
    {
        valid = 0;
    }
#else // e.g. defined(__FreeBSD__)
    if (memcmp(addr->octet, &prev_addr,
               sizeof(struct ether_addr)) != 0)
    {
        valid = 0;
    }
#endif

    if (!valid)
    {
//      if(ether_ntohost(buf, addr)){
        sprintf(buf, "%s", "unknown");
//      }

        /* clean possible commas from the string */
        while (strchr(buf, ',') != NULL)
        {
            *(strchr(buf, ',')) = '_';
        }

        prev_addr = *addr;
    }

    valid = 1;
    strcpy(hostname, buf);
    return 0;
}


/* Show the hostname configured in /etc/ethers */
int
snprint_ClockIdentity_ntohost(char *s, int max_len, const ClockIdentity id)
{
    int len = 0;
    int i, j;
    char  buf[100];
    struct ether_addr e;

    /* extract mac address */
    for (i = 0, j = 0; i < CLOCK_IDENTITY_LENGTH ; i++)
    {
        /* skip bytes 3 and 4 */
        if (!((i == 3) || (i == 4)))
        {
#if defined(linux) || defined(__NetBSD__)
            e.ether_addr_octet[j] = (uint8_t) id[i];
#else // e.g. defined(__FreeBSD__)
            e.octet[j] = (uint8_t) id[i];
#endif
            j++;
        }
    }

    /* convert and print hostname */
    ether_ntohost_cache(buf, &e);
    len += snprintf(&s[len], max_len - len, "(%s)", buf);

    return len;
}


int
snprint_PortIdentity(char *s, int max_len, const PortIdentity *id)
{
    int len = 0;

#ifdef PRINT_MAC_ADDRESSES
    len += snprint_ClockIdentity_mac(&s[len], max_len - len, id->clockIdentity);
#else
    len += snprint_ClockIdentity(&s[len], max_len - len, id->clockIdentity);
#endif

    len += snprint_ClockIdentity_ntohost(&s[len], max_len - len, id->clockIdentity);

    len += snprintf(&s[len], max_len - len, "/%02x", (unsigned) id->portNumber);
    return len;
}


/*
 * Prints a message, randing from critical to debug.
 * This either prints the message to syslog, or with timestamp+state to stderr
 * (which has possibly been redirected to a file, using logtofile()/dup2().
 */
void
message(int priority, const char *format, ...)
{
    extern RunTimeOpts rtOpts;
    va_list ap;
    va_start(ap, format);

#ifdef RUNTIME_DEBUG
    if ((priority >= LOG_DEBUG) && (priority > rtOpts.debug_level))
    {
        return;
    }
#endif

    if (rtOpts.useSysLog)
    {
#if 0
        static Boolean logOpened;
#ifdef RUNTIME_DEBUG
        /*
         *  Syslog only has 8 message levels (3 bits)
         *  important: messages will only appear if "*.debug /var/log/debug" is on /etc/rsyslog.conf
         */
        if (priority > LOG_DEBUG)
        {
            priority = LOG_DEBUG;
        }
#endif

        if (!logOpened)
        {
            openlog(PTPD_PROGNAME, 0, LOG_DAEMON);
            logOpened = TRUE;
        }
        vsyslog(priority, format, ap);

        /* Also warn operator during startup only */
        if (rtOpts.syslog_startup_messages_also_to_stdout &&
                (priority <= LOG_WARNING)
           )
        {
            va_start(ap, format);
            vfprintf(stderr, format, ap);
        }
#endif
    }
    else
    {
        char time_str[MAXTIMESTR];
        TimeInternal now;

        extern char *translatePortState(PtpClock * ptpClock);
        extern PtpClock *G_ptpClock;

        printf("   (ptpd %-9s ",
               priority == LOG_EMERG   ? "emergency)" :
               priority == LOG_ALERT   ? "alert)" :
               priority == LOG_CRIT    ? "critical)" :
               priority == LOG_ERR     ? "error)" :
               priority == LOG_WARNING ? "warning)" :
               priority == LOG_NOTICE  ? "notice)" :
               priority == LOG_INFO    ? "info)" :
               priority == LOG_DEBUG   ? "debug1)" :
               priority == LOG_DEBUG2  ? "debug2)" :
               priority == LOG_DEBUGV  ? "debug3)" :
               "unk)");

        /*
         * select debug tagged with timestamps. This will slow down PTP itself if you send a lot of messages!
         * it also can cause problems in nested debug statements (which are solved by turning the signal
         *  handling synchronous, and not calling this function inside assycnhonous signal processing)
         */
        getTime(&now);
        strftime(time_str, MAXTIMESTR, "%X", localtime((time_t *)&now.seconds));
        printf("%s.%06d ", time_str, (int)now.seconds);
        printf(" (%s)  ", G_ptpClock ?
               translatePortState(G_ptpClock) : "___");

        printf(format, ap);
    }
    va_end(ap);
}

void
increaseMaxDelayThreshold()
{
    extern RunTimeOpts rtOpts;
    NOTIFY("Increasing maxDelay threshold from %i to %i\n", rtOpts.maxDelay,
           rtOpts.maxDelay << 1);

    rtOpts.maxDelay <<= 1;
}

void
decreaseMaxDelayThreshold()
{
    extern RunTimeOpts rtOpts;
    if ((rtOpts.maxDelay >> 1) < rtOpts.origMaxDelay)
        return;
    NOTIFY("Decreasing maxDelay threshold from %i to %i\n",
           rtOpts.maxDelay, rtOpts.maxDelay >> 1);
    rtOpts.maxDelay >>= 1;
}

void
displayStats(RunTimeOpts *rtOpts, PtpClock *ptpClock)
{
    static int start = 1;
    static char sbuf[SCREEN_BUFSZ];
    int len = 0;
    TimeInternal now;
    time_t time_s;
    static TimeInternal prev_now;
    char time_str[MAXTIMESTR];

    if (!rtOpts->displayStats)
    {
        return;
    }

    if (start)
    {
        start = 0;
        printf("# Timestamp, State, Clock ID, One Way Delay, "
               "Offset From Master, Slave to Master, "
               "Master to Slave, Drift, Discarded Packet Count, Last packet Received\n");
        //  fflush(stdout);
    }
    memset(sbuf, ' ', sizeof(sbuf));

    getTime(&now);

    /*
     * print one log entry per X seconds, to reduce disk usage.
     * This only happens to SLAVE SYNC statistics lines, which are the bulk of the log.
     * All other lines are printed, including delayreqs.
     */

    if ((ptpClock->portState == PTP_SLAVE) && (rtOpts->log_seconds_between_message))
    {
        if (ptpClock->last_packet_was_sync)
        {
            ptpClock->last_packet_was_sync = FALSE;
            if ((now.seconds - prev_now.seconds) < rtOpts->log_seconds_between_message)
            {
                //leave early and do not print the log message to save disk space
                DBGV("Skipped printing of Sync message because of option -V\n");
                return;
            }
            prev_now = now;
        }
    }
    ptpClock->last_packet_was_sync = FALSE;

    time_s = now.seconds;
    strftime(time_str, MAXTIMESTR, "%Y-%m-%d %X", localtime(&time_s));
    len += snprintf(sbuf + len, sizeof(sbuf) - len, "%s.%06d, %s, ",
                    time_str, (int)now.nanoseconds / 1000, /* Timestamp */
                    translatePortState(ptpClock)); /* State */

    if (ptpClock->portState == PTP_SLAVE)
    {
        len += snprint_PortIdentity(sbuf + len, sizeof(sbuf) - len,
                                    &ptpClock->parentPortIdentity); /* Clock ID */

        /*
         * if grandmaster ID differs from parent port ID then
         * also print GM ID
         */
        if (memcmp(ptpClock->grandmasterIdentity,
                   ptpClock->parentPortIdentity.clockIdentity,
                   CLOCK_IDENTITY_LENGTH))
        {
            len += snprint_ClockIdentity(sbuf + len,
                                         sizeof(sbuf) - len,
                                         ptpClock->grandmasterIdentity);
        }

        len += snprintf(sbuf + len, sizeof(sbuf) - len, ", ");

        if (rtOpts->delayMechanism == E2E)
        {
            len += snprint_TimeInternal(sbuf + len, sizeof(sbuf) - len,
                                        &ptpClock->meanPathDelay);
        }
        else
        {
            len += snprint_TimeInternal(sbuf + len, sizeof(sbuf) - len,
                                        &ptpClock->peerMeanPathDelay);
        }

        len += snprintf(sbuf + len, sizeof(sbuf) - len, ", ");

        len += snprint_TimeInternal(sbuf + len, sizeof(sbuf) - len,
                                    &ptpClock->offsetFromMaster);

        /* print MS and SM with sign */
        len += snprintf(sbuf + len, sizeof(sbuf) - len, ", ");

        len += snprint_TimeInternal(sbuf + len, sizeof(sbuf) - len,
                                    &(ptpClock->delaySM));

        len += snprintf(sbuf + len, sizeof(sbuf) - len, ", ");

        len += snprint_TimeInternal(sbuf + len, sizeof(sbuf) - len,
                                    &(ptpClock->delayMS));

        len += sprintf(sbuf + len, ", %d, %i, %c",
                       ptpClock->observed_drift,
                       ptpClock->discardedPacketCount,
                       ptpClock->char_last_msg);

    }
    else
    {
        if ((ptpClock->portState == PTP_MASTER) || (ptpClock->portState == PTP_PASSIVE))
        {

            len += snprint_PortIdentity(sbuf + len, sizeof(sbuf) - len,
                                        &ptpClock->parentPortIdentity);

            //len += snprintf(sbuf + len, sizeof(sbuf) - len, ")");
        }

        /* show the current reset number on the log */
        if (ptpClock->portState == PTP_LISTENING)
        {
            len += snprintf(sbuf + len,
                            sizeof(sbuf) - len,
                            " %d ", ptpClock->reset_count);
        }
    }

    /* add final \n in normal status lines */
    len += snprintf(sbuf + len, sizeof(sbuf) - len, "\n");

#if 0   /* NOTE: Do we want this? */
    if (rtOpts->nonDaemon)
    {
        /* in -C mode, adding an extra \n makes stats more clear intermixed with debug comments */
        len += snprintf(sbuf + len, sizeof(sbuf) - len, "\n");
    }
#endif
    write(1, sbuf, len);
}


void
recordSync(RunTimeOpts *rtOpts, UInteger16 sequenceId, TimeInternal *time)
{
#if 0
    if (rtOpts->recordFP)
        fprintf(rtOpts->recordFP, "%d %llu\n", sequenceId,
                ((time->seconds * 1000000000ULL) + time->nanoseconds)
               );
#endif
}



/* returns a double beween 0.0 and 1.0 */
double
getRand(void)
{
    return ((rand() * 1.0) / RAND_MAX);
}


#ifdef RT_USING_GMAC_PTP
void milliSleep(int milli_seconds)
{

    rt_thread_sleep(milli_seconds * RT_TICK_PER_SECOND / 1000);
}

/*
 * TODO: this function should have been coded in a way to manipulate both the frequency and the tick,
 * to avoid having to call setTime() when the clock is very far away.
 * This would result in situations we would force the kernel clock to run the clock twice as slow,
 * in order to avoid stepping time backwards
 */
void getTime(TimeInternal *time)
{
    struct PTP_TIME timestamp;
    rt_rockchip_ptp_get_time(ptpd_get_handle(), &timestamp);
    time->seconds = timestamp.sec;
    time->nanoseconds = timestamp.nsec;
}

void setTime(TimeInternal *time)
{
    struct PTP_TIME ts;

    ts.sec = time->seconds;
    ts.nsec = time->nanoseconds;
    rt_rockchip_ptp_set_time(ptpd_get_handle(), &ts);
    DBG("resetting system clock to %d sec %d nsec\n", time->seconds, time->nanoseconds);
}

void updateTime(TimeInternal *time)
{
    struct PTP_TIME_OFFSET timeoffset = {0};

    DBGV("updateTime: %d sec %d nsec\n", time->seconds, time->nanoseconds);

    timeoffset.sec = -time->seconds;
    timeoffset.nsec = -time->nanoseconds;

    /* Coarse update method */
    rt_rockchip_ptp_update_offset(ptpd_get_handle(), &timeoffset);
    DBGV("updateTime: updated\n");
}

Boolean  adjFreq(int32_t adj)
{
    DBGV("adjFreq %d\n", adj);

    if (adj > ADJ_FREQ_MAX)
        adj = ADJ_FREQ_MAX;
    else if (adj < -ADJ_FREQ_MAX)
        adj = -ADJ_FREQ_MAX;

    /* Fine update method */
    rt_rockchip_ptp_adj_freq(ptpd_get_handle(), adj);

    //sprintf(g_debug_message[1], "PTP adjust: %d\n", adj);

    return TRUE;
}
#endif

void
setTimexFlags(int flags, Boolean quiet)
{
    (void)flags;
    (void)quiet;

#if 0
    struct timex tmx;
    int ret;

    memset(&tmx, 0, sizeof(tmx));

    tmx.modes = MOD_STATUS;

    tmx.status = getTimexFlags();
    if (tmx.status == -1)
        return;
    /* unset all read-only flags */
    tmx.status &= ~STA_RONLY;
    tmx.status |= flags;

    ret = adjtimex(&tmx);

    if (ret < 0)
        PERROR("Could not set adjtimex flags: %s", strerror(errno));

    if (!quiet && ret > 2)
    {
        switch (ret)
        {
        case TIME_OOP:
            WARNING("Adjtimex: leap second already in progress\n");
            break;
        case TIME_WAIT:
            WARNING("Adjtimex: leap second already occurred\n");
            break;
#if !defined(TIME_BAD)
        case TIME_ERROR:
#else
        case TIME_BAD:
#endif /* TIME_BAD */
        default:
            DBGV("unsetTimexFlags: adjtimex() returned TIME_BAD\n");
            break;
        }
    }
#endif
}

void
unsetTimexFlags(int flags, Boolean quiet)
{
    (void)flags;
    (void)quiet;

#if 0

    struct timex tmx;
    int ret;

    memset(&tmx, 0, sizeof(tmx));

    tmx.modes = MOD_STATUS;

    tmx.status = getTimexFlags();
    if (tmx.status == -1)
        return;
    /* unset all read-only flags */
    tmx.status &= ~STA_RONLY;
    tmx.status &= ~flags;

    ret = adjtimex(&tmx);

    if (ret < 0)
        PERROR("Could not unset adjtimex flags: %s", strerror(errno));

    if (!quiet && ret > 2)
    {
        switch (ret)
        {
        case TIME_OOP:
            WARNING("Adjtimex: leap second already in progress\n");
            break;
        case TIME_WAIT:
            WARNING("Adjtimex: leap second already occurred\n");
            break;
#if !defined(TIME_BAD)
        case TIME_ERROR:
#else
        case TIME_BAD:
#endif /* TIME_BAD */
        default:
            DBGV("unsetTimexFlags: adjtimex() returned TIME_BAD\n");
            break;
        }
    }
#endif
}

int getTimexFlags(void)
{
#if 0
    struct timex tmx;
    int ret;

    memset(&tmx, 0, sizeof(tmx));

    tmx.modes = 0;
    ret = adjtimex(&tmx);
    if (ret < 0)
    {
        PERROR("Could not read adjtimex flags: %s", strerror(errno));
        return (-1);

    }

    return (tmx.status);
#endif
    return 0;
}

Boolean
checkTimexFlags(int flags)
{
    (void)flags;
#if 0
    int tflags = getTimexFlags();
    if (tflags == -1)
        return FALSE;
    return ((tflags & flags) == flags);
#endif
    return FALSE;
}

#if 0 && defined (linux)  /* NOTE: This is actually not used */

long
get_current_tickrate(void)
{
    struct timex t;

    t.modes = 0;
    adjtimex(&t);
    DBG2(" (Current tick rate is %d)\n", t.tick);

    return (t.tick);
}

#endif  /* defined(linux) */
