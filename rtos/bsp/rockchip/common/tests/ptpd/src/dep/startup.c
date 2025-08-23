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
 * @file   startup.c
 * @date   Wed Jun 23 09:33:27 2010
 *
 * @brief  Code to handle daemon startup, including command line args
 *
 * The function in this file are called when the daemon starts up
 * and include the getopt() command line argument parsing.
 */

#include "../ptpd.h"
#ifdef RT_USING_GMAC_PTP
#include "drv_gmac.h"
#endif

/*
 * Synchronous signal processing:
 * This function should be called regularly from the main loop
 */
void
check_signals(RunTimeOpts *rtOpts, PtpClock *ptpClock)
{
    (void)rtOpts;
    (void)ptpClock;

    return;
}

void
ptpdShutdown(PtpClock *ptpClock)
{
    netShutdown(&ptpClock->netPath);
    free(ptpClock->foreign);

    /* free management messages, they can have dynamic memory allocated */
    if (ptpClock->msgTmpHeader.messageType == MANAGEMENT)
        freeManagementTLV(&ptpClock->msgTmp.manage);
    freeManagementTLV(&ptpClock->outgoingManageTmp);

    free(ptpClock);
    ptpClock = NULL;

    extern PtpClock *G_ptpClock;
    G_ptpClock = NULL;
}

PtpClock *
ptpdStartup(int argc, char **argv, Integer16 *ret, RunTimeOpts *rtOpts)
{
    PtpClock *ptpClock;

    (void)rtOpts;
    (void)argc;
    (void)argv;

    ptpClock = (PtpClock *) malloc(sizeof(PtpClock));
    if (!ptpClock)
    {
        PERROR("failed to allocate memory for protocol engine data");
        *ret = 2;
        return 0;
    }
    else
    {
        DBG("allocated %d bytes for protocol engine data\n",
            (int)sizeof(PtpClock));
        memset(ptpClock, 0, sizeof(PtpClock));
        ptpClock->foreign = (ForeignMasterRecord *)
                            malloc(rtOpts->max_foreign_records * sizeof(ForeignMasterRecord));
        if (!ptpClock->foreign)
        {
            PERROR("failed to allocate memory for foreign "
                   "master data");
            *ret = 2;
            free(ptpClock);
            return 0;
        }
        else
        {
            DBG("allocated %d bytes for foreign master data\n",
                (int)(rtOpts->max_foreign_records *
                      sizeof(ForeignMasterRecord)));
        }
    }

    /* Init to 0 net buffer */
    //memset(ptpClock->msgIbuf, 0, PACKET_SIZE);
    //memset(ptpClock->msgObuf, 0, PACKET_SIZE);


    /* Init user_description */
    memset(ptpClock->user_description, 0, sizeof(ptpClock->user_description));
    memcpy(ptpClock->user_description, &USER_DESCRIPTION, sizeof(USER_DESCRIPTION));

    /* Init outgoing management message */
    ptpClock->outgoingManageTmp.tlv = NULL;

    *ret = 0;

    INFO("  Info:    Startup finished sucessfully\n");

    return ptpClock;
}

#ifdef RT_USING_GMAC_PTP
static void ptpd_thread(void *pvParaMeter)
{
    struct gptp_data *bParam = (struct gptp_data *)pvParaMeter;
    Boolean bslaveOnly;

    bslaveOnly = (bParam->slave) ? TRUE : FALSE;
    ptpd_task(bslaveOnly);
}

static struct gptp_data ptpData = {0};
void *ptpd_get_handle(void)
{
    return ptpData.gptp_handle;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
void ptp_test(int argc, char **argv)
{
    int slave;

    slave = strtol(argv[1], NULL, 16);
    ptpData.gptp_handle = rt_rockchip_ptp_get_handle();
    ptpData.slave = slave;
    if (ptpData.gptp_handle)
    {
        rt_thread_t tid;

        tid = rt_thread_create("PTPd",
                               ptpd_thread,
                               &ptpData,
                               512 * 4,
                               RT_THREAD_PRIORITY_MAX - 2,
                               2);
        if (tid != RT_NULL)
        {
            rt_thread_startup(tid);
        }
    }
}
MSH_CMD_EXPORT(ptp_test, ptp test for slave only);
#endif
#endif
