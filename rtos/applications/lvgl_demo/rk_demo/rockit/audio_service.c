/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
*/
#include "main.h"
#if MULTIMEDIA_EN

#include <rtthread.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "audio_in.h"
#include "audio_out.h"
#include "audio_service.h"

#include "rk_defines.h"
#include "rk_debug.h"
#include "rk_mpi_sys.h"

struct audio_server
{
    int fd;
    int remote_client;
    int state;
    void *client;
    pthread_t tid;
};

static int ao_read(void *arg, char *buf, int len)
{
    struct audio_server *server = (struct audio_server *)arg;
    int num = 0;
    int _len = len;

    if (!buf)
    {
        RK_LOGE("buf is NULL");
        return 0;
    }

    while (_len && (server->state == STATE_RUNNING))
    {
        num = read(server->remote_client,
                   buf + (len - _len), _len);
        if (num <= 0)
            return 0;
        _len -= num;
    }

    if (server->state != STATE_RUNNING)
        return 0;

    return len;
}

static int audio_server_init(void)
{
    struct sockaddr_in saddr;
    int fd;
    int ret;

    fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd < 0)
    {
        printf("new socket failed");
        return fd;
    }

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(9999);
    ret = bind(fd, (struct sockaddr *)&saddr, sizeof(saddr));

    if (ret < 0)
    {
        printf("bind socket failed");
        close(fd);
        return ret;
    }

    return fd;
}

static void *audio_server(void *arg)
{
    struct audio_server *server = (struct audio_server *)arg;
    char clientIP[16];
    unsigned short clientPort;
    struct sockaddr_in clientaddr;
    int len;
    int cfd;
    int ret;

    server->state = STATE_RUNNING;
    while (server->state == STATE_RUNNING)
    {
        printf("start listening...\n");
        ret = listen(server->fd, 8);
        if (ret == -1)
        {
            printf("listen error\n");
            server->state = STATE_ERROR;
            break;
        }

        len = sizeof(clientaddr);
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        cfd = accept(server->fd, (struct sockaddr *)&clientaddr, &len);
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

        if (cfd == -1)
        {
            printf("accept error\n");
            continue;
        }

        server->remote_client = cfd;

        inet_ntop(AF_INET, &clientaddr.sin_addr.s_addr, clientIP, sizeof(clientIP));
        clientPort = ntohs(clientaddr.sin_port);
        printf("client ip is %s, port is %d\n", clientIP, clientPort);

        if (audio_server_connect(server, clientIP) < 0)
        {
            printf("connect to %s error\n", clientIP);
            goto ao_err;
        }
        ret = ao_init();
        if (ret == -1)
        {
            printf("ao init error\n");
            goto ao_err;
        }

        server->state = STATE_RUNNING;
        while (server->state == STATE_RUNNING)
        {
            if (ao_push(ao_read, server) == -1)
            {
                printf("ao push failed\n");
                break;
            }
        }
        ao_deinit();
ao_err:
        close(cfd);
        if (server->client)
            audio_client_del(server->client);
        server->client = NULL;
        server->remote_client = -1;
        if (server->state == STATE_PAUSE)
            server->state = STATE_RUNNING;
    }
    close(server->fd);
    server->fd = -1;
    if (server->state != STATE_ERROR)
        server->state = STATE_IDLE;
    printf("audio server exit\n");

    return NULL;
}

int audio_server_state(void *arg)
{
    struct audio_server *server = (struct audio_server *)arg;

    if (!server || !server->client)
        return STATE_IDLE;

    return audio_client_state(server->client);
}

int audio_server_connect(void *arg, const char *ip)
{
    struct audio_server *server = (struct audio_server *)arg;

    if (!server)
    {
        printf("%s null ptr\n", __func__);
        return -1;
    }

    if (server->client)
        printf("server is connected\n");
    else
        server->client = audio_client_new(ip);

    return 0;
}

int audio_server_connected(void *arg)
{
    struct audio_server *server = (struct audio_server *)arg;

    if (!server)
    {
        printf("%s null ptr\n", __func__);
        return -1;
    }

    return server->client != NULL;
}

int audio_server_disconnect(void *arg)
{
    struct audio_server *server = (struct audio_server *)arg;

    if (!server)
    {
        printf("%s null ptr\n", __func__);
        return -1;
    }

    if (server->state != STATE_RUNNING)
    {
        printf("%s wrong state %d\n", __func__, server->state);
        return -1;
    }

    server->state = STATE_PAUSE;
}

int audio_server_del(void *arg)
{
    struct audio_server *server = (struct audio_server *)arg;

    if (!server || !server->tid)
    {
        printf("%s %p error\n", __func__, server);
        return -1;
    }

    server->state = STATE_EXIT;
    pthread_cancel(server->tid);
    pthread_join(server->tid, NULL);
    if (server->fd != -1)
    {
        close(server->fd);
        server->fd = -1;
    }

    free(server);

    return 0;
}

void *audio_server_new(void)
{
    struct audio_server *server;
    pthread_t tid;
    int fd;
    int ret;

    server = calloc(1, sizeof(*server));
    if (!server)
    {
        printf("audio server calloc failed %d\n", sizeof(*server));
        return NULL;
    }

    fd = audio_server_init();
    if (fd < 0)
    {
        printf("audio server init failed %d\n", fd);
        ret = fd;
        goto fd_err;
    }

    server->fd = fd;

    ret = pthread_create(&server->tid, NULL, audio_server, server);
    if (ret < 0)
    {
        printf("audio server start failed %d\n", ret);
        goto t_err;
    }

    return server;

t_err:
    close(fd);
fd_err:
    free(server);

    return NULL;
}

#endif
