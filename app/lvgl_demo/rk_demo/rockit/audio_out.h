#ifndef __AUDIO_OUT_H__
#define __AUDIO_OUT_H__

int ao_init(void);
int ao_push(int (*hook)(void *, char *, int), void *arg);
int ao_deinit(void);

#endif

