/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2022.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 57-6 */

#include <sys/un.h>
#include <sys/socket.h>
#include <ctype.h>
#include <sys/types.h>  /* Type definitions used by many programs */
#include <stdio.h>      /* Standard I/O functions */
#include <stdlib.h>     /* Prototypes of commonly used library functions,
                           plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <unistd.h>     /* Prototypes for many system calls */
#include <errno.h>      /* Declares errno and defines error constants */
#include <string.h>     /* Commonly used string-handling functions */

#include "main.h"

typedef enum { FALSE, TRUE } Boolean;

#define min(m,n) ((m) < (n) ? (m) : (n))
#define max(m,n) ((m) > (n) ? (m) : (n))

#define BUF_SIZE 100             /* Maximum size of messages exchanged
                                   between client and server */

#define SV_SOCK_PATH "/tmp/rt_demo"

static int update = 0;
static int reset = 0;
static int32_t data[300] = {0,};
static int32_t data_min = 0;
static int32_t data_max = 0;
static int32_t data_avg = 0;
static int32_t data_cur = 0;

void socket_reset(void)
{
    if (reset == 0)
        reset = 1;
}

int socket_read(int32_t *y, int32_t *min, int32_t *max, int32_t *avg,
                int32_t *cur)
{
    if (!update)
        return 0;

    update = 0;
    memcpy(y, data, sizeof(data));
    *min = data_min;
    *max = data_max;
    *avg = data_avg;
    *cur = data_cur;

    return 1;
}

void *socket_thread(void *argv)
{
    struct sockaddr_un svaddr, claddr;
    int sfd;
    ssize_t numBytes;
    socklen_t len;
    char buf[BUF_SIZE];

    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);       /* Create server socket */
    if (sfd == -1)
        printf("socket");

    /* Construct well-known address and bind server socket to it */

    if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT)
        printf("remove-%s", SV_SOCK_PATH);

    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, SV_SOCK_PATH, sizeof(svaddr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_un)) == -1)
        printf("bind");

    /* Receive messages, convert to uppercase, and return to client */

    for (;;)
    {
        int idx;
        len = sizeof(struct sockaddr_un);
        numBytes = recvfrom(sfd, buf, BUF_SIZE, 0,
                            (struct sockaddr *) &claddr, &len);
        if (numBytes == -1)
            printf("recvfrom");

        buf[numBytes] = '\0';

        //printf("Received data: ");
        for (int j = 0; j < numBytes; j++)
        {
            buf[j] = toupper((unsigned char) buf[j]);
            //putchar(buf[j]);
        }
        //printf("\n");

        if (strncmp(buf, "CUR", 3) == 0)
        {
            data_cur = atoi(buf + 4);
            if (reset == 1)
            {
                reset = 2;
            }
            else
            {
                reset = 0;
                update = 1;
            }
            printf("CUR = %d\n", data_cur);
        }
        else if (strncmp(buf, "AVG", 3) == 0)
        {
            data_avg = atoi(buf + 4);
            printf("%s\n", buf);
        }
        else if (strncmp(buf, "MIN", 3) == 0)
        {
            data_min = atoi(buf + 4);
        }
        else if (strncmp(buf, "MAX", 3) == 0)
        {
            data_max = atoi(buf + 4);
        }
        else
        {
            idx = atoi(buf);
            if (idx < ARRAY_SIZE(data))
            {
                if (reset == 2)
                    data[idx] = atoi(buf + 10);
                else
                    data[idx] += atoi(buf + 10);
            }
        }

        if (sendto(sfd, buf, numBytes, 0, (struct sockaddr *) &claddr, len) !=
                numBytes)
            printf("sendto");
    }
}
