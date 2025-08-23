#ifndef __AUDIO_SERVER_H__
#define __AUDIO_SERVER_H__

enum
{
    STATE_IDLE,
    STATE_RUNNING,
    STATE_EXIT,
    STATE_ERROR,
    STATE_PAUSE,
};

void *audio_server_new(void);
int audio_server_del(void *arg);
int audio_server_state(void *arg);
int audio_server_auto_connect(void *arg, int en);
int audio_server_connected(void *arg);
int audio_server_connect(void *arg, const char *ip);
int audio_server_disconnect(void *arg);

void *audio_client_new(const char *ip);
int audio_client_del(void *arg);
int audio_client_state(void *arg);

#endif

