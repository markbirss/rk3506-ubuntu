#include <limits.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alsa/asoundlib.h>

#include <pthread.h>

#include "main.h"

#if ASR_EN
#include "asr.h"
#include "rkaudio_preprocess.h"

#if USE_RK3506
#define CAPTURE_CARD            "default"
#define PLAYBACK_CARD           "default"
#else
#define CAPTURE_CARD            "sysdefault:CARD=rockchiprk3308v"
#define PLAYBACK_CARD           "sysdefault:CARD=rockchiprk3308v"
#endif

#define INPUT_SAMPLE_RATE       16000
#define INPUT_BITS              16
#define PERIOD_SIZE             256
#define IN_SIZE                 256

static pthread_t ptid_c;
static pthread_t ptid_p;
static int pipe_fd[2];
static int running = 0;

void cmd_answer(int cmd)
{
    int ret = write(pipe_fd[1], &cmd, sizeof(cmd));
    if (ret <= 0)
        printf("%s failed %d\n", ret);
}

static char *answer_pcm[] =
{
    "/usr/share/resource/pcm/wo_zai.pcm",
    "/usr/share/resource/pcm/hao_de.pcm",
    "/usr/share/resource/pcm/zheng_zai_zhi_xing.pcm",
    "/usr/share/resource/pcm/yi_kai_qi.pcm",
    "/usr/share/resource/pcm/yi_guan_bi.pcm",
    "/usr/share/resource/pcm/huan_yin_hui_jia.pcm",
    "/usr/share/resource/pcm/zai_jian.pcm",
    "/usr/share/resource/pcm/huan_yin.pcm",
};

void alsa_open(snd_pcm_t **handle, char *name, int channels, uint32_t rate,
               snd_pcm_format_t format, snd_pcm_stream_t stream)
{
    snd_pcm_hw_params_t *hw_params;
    int err;
    if ((err = snd_pcm_open(handle, name, stream, 0)) < 0)
    {
        fprintf(stderr, "cannot open audio device %s %d(%s)\n", name, stream,
                snd_strerror(err));
        exit(1);
    }

    if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0)
    {
        fprintf(stderr, "cannot allocate hardware parameter structure %s %d(%s)\n",
                name, stream,
                snd_strerror(err));
        exit(1);
    }

    if ((err = snd_pcm_hw_params_any(*handle, hw_params)) < 0)
    {
        fprintf(stderr, "cannot initialize hardware parameter structure %s %d(%s)\n",
                name, stream,
                snd_strerror(err));
        exit(1);
    }

    if ((err = snd_pcm_hw_params_set_access(*handle, hw_params,
                                            SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
    {
        fprintf(stderr, "cannot set access type %s %d(%s)\n", name, stream,
                snd_strerror(err));
        exit(1);
    }

    if ((err = snd_pcm_hw_params_set_format(*handle, hw_params, format)) < 0)
    {
        fprintf(stderr, "cannot set sample format %s %d(%s)\n", name, stream,
                snd_strerror(err));
        exit(1);
    }

    fprintf(stdout, "hw_params format set\n");
    if ((err = snd_pcm_hw_params_set_rate_near(*handle, hw_params, &rate, 0)) < 0)
    {
        fprintf(stderr, "cannot set sample rate %s %d(%s)\n", name, stream,
                snd_strerror(err));
        exit(1);
    }

    if ((err = snd_pcm_hw_params_set_channels(*handle, hw_params, channels)) < 0)
    {
        fprintf(stderr, "cannot set channel count %s %d(%s)\n", name, stream,
                snd_strerror(err));
        exit(1);
    }
#if 0
    if ((err = snd_pcm_hw_params_set_period_size(*handle, hw_params, PERIOD_SIZE,
               0)) < 0)
    {
        fprintf(stderr, "cannot set period size %s %d(%s)\n", name, stream,
                snd_strerror(err));
        exit(1);
    }
#endif
    if ((err = snd_pcm_hw_params(*handle, hw_params)) < 0)
    {
        fprintf(stderr, "cannot set parameters %s %d(%s)\n", name, stream,
                snd_strerror(err));
        exit(1);
    }

    snd_pcm_hw_params_free(hw_params);
}

static void *doplay(void *arg)
{
    snd_pcm_t *phandle;
    int play_cmd;
    int last_cmd;
    char *buf;
    int err, res;
    FILE *fd;

    alsa_open(&phandle, PLAYBACK_CARD,
              2, INPUT_SAMPLE_RATE, SND_PCM_FORMAT_S16_LE,
              SND_PCM_STREAM_PLAYBACK);

    buf = malloc(PERIOD_SIZE * 2 * 2);

    while (running)
    {
        int num = read(pipe_fd[0], &play_cmd, sizeof(int));
        if (!running)
            break;
        last_cmd = play_cmd;
        play_cmd = -1;

        if (last_cmd > sizeof(answer_pcm) / sizeof(answer_pcm[0]))
            continue;
        printf("%s\n", answer_pcm[last_cmd]);
        fd = fopen(answer_pcm[last_cmd], "rb");
        if (!fd)
            continue;

        if ((err = snd_pcm_prepare(phandle)) < 0)
        {
            fprintf(stderr, "cannot prepare audio interface for use (%s)\n",
                    snd_strerror(err));
            fclose(fd);
            continue;
        }

        while (play_cmd == -1)
        {
            memset(buf, 0, PERIOD_SIZE * 2 * 2);
            res = fread(buf, 1, PERIOD_SIZE * 2 * 2, fd);
            if (res <= 0)
                break;
            res = snd_pcm_writei(phandle, buf, PERIOD_SIZE);
            if (res <= 0)
            {
                printf("%s snd_pcm_readi error %d, continue\n", __func__, res);
                snd_pcm_prepare(phandle);
                snd_pcm_writei(phandle, buf, PERIOD_SIZE);
                continue;
            }
        }

        fclose(fd);
    }
    snd_pcm_close(phandle);
    free(buf);
    printf("thread '%s' out\n", __func__);

    return NULL;
}

static void *docap(void *arg)
{
    snd_pcm_t *handle;
    int wakeup_status = 0;
    int num_wakeup = 0;
    int num_asr = 0;
    short *buf;
    short *in;
    int err;

    alsa_open(&handle, CAPTURE_CARD, 2, INPUT_SAMPLE_RATE,
              SND_PCM_FORMAT_S16_LE,
              SND_PCM_STREAM_CAPTURE);
    if ((err = snd_pcm_prepare(handle)) < 0)
    {
        fprintf(stderr, "cannot prepare audio interface for use (%s)\n",
                snd_strerror(err));
        handle = NULL;
        return NULL;
    }

    void *st_ptr = NULL;
#if 0
    RKAUDIOParam param;
    memset(&param, 0, sizeof(RKAUDIOParam));
    param.model_en = RKAUDIO_EN_BF;
    param.aec_param = rkaudio_aec_param_init();
    param.bf_param = rkaudio_preprocess_param_init();
    param.rx_param = rkaudio_rx_param_init();
    printf("rkaudio_preprocess_init\n");
    st_ptr = rkaudio_preprocess_init(INPUT_SAMPLE_RATE, INPUT_BITS, 2, 0, &param);
#else
    printf("rkaudio_preprocess_init_by_conf\n");
    st_ptr = rkaudio_preprocess_init_by_conf(INPUT_SAMPLE_RATE, INPUT_BITS, 2, 0,
             "/usr/vqefiles/config_wakeup.json");
#endif

    short *out = (short *)malloc(IN_SIZE * 2 * sizeof(short));
    int out_size = 0, in_size = 0, res = 0;
    buf = malloc(PERIOD_SIZE * 2 * 2);
    in_size = PERIOD_SIZE * 2;
    in = buf;

    int frame_cot = 0;
    int cmd_id = 0;
    float asr_score = 0.0f, cmd_score = 0.0f;
    FILE *fd = NULL;
    char path[128];
    snprintf(path, sizeof(path), "/tmp/asrin_%d_%d_%d.pcm",
             INPUT_SAMPLE_RATE, INPUT_BITS, 2);
    while (running)
    {
        if (!fd && (access("/tmp/asrdump", F_OK) == 0))
            fd = fopen(path, "wb+");
        if (fd && (access("/tmp/asrdump", F_OK) != 0))
        {
            fclose(fd);
            fd = NULL;
        }
        int res = snd_pcm_readi(handle, buf, PERIOD_SIZE);
        if (res <= 0)
        {
            printf("%s snd_pcm_readi error %d, continue\n", __func__, res);
            snd_pcm_prepare(handle);
            continue;
        }
        if (fd)
            fwrite(in, 2, in_size, fd);
        out_size = rkaudio_preprocess_short(st_ptr, (short *)in, out, in_size,
                                            &wakeup_status);
        int32_t tmpstatus = (int32_t)wakeup_status;
        int32_t real_wakeup_status = tmpstatus & 0xfff;
        if (real_wakeup_status >= 1)
        {
            frame_cot = 375;// 6s timeout
            num_wakeup = num_wakeup + 1;
            printf("\nWakeup [%d].\n", num_wakeup);
            wakeup_status = wakeup_status >> 11 << 11;
            asr_update(0);
        }
        if (frame_cot > 0)
        {
            frame_cot--;
            rkaudio_preprocess_get_cmd_id(st_ptr, &cmd_score, &cmd_id);
            if (cmd_id != 0)
            {
                printf("\ncmd=%d\n", cmd_id);
                if (frame_cot < 480)
                    asr_update(cmd_id);
                frame_cot = 500;
            }
            else if (frame_cot == 0)
            {
                printf("\nIdle, sleep now\n");
                wakeup_status = 0;
                asr_update(-1);
                /* clean cmd */
                rkaudio_preprocess_get_cmd_id(st_ptr, &cmd_score, &cmd_id);
            }
        }
    }
    if (fd)
        fclose(fd);
    asr_update(-1);
    rkaudio_preprocess_destory(st_ptr);
    free(buf);
    free(out);
    snd_pcm_close(handle);
    printf("thread '%s' out\n", __func__);

    return NULL;
}

void asr_audio_init(void)
{
    int ret = pipe(pipe_fd);
    if (ret != 0)
    {
        printf("asr init failed\n");
        return;
    }

    printf_asr_cmds();
    running = 1;
    if (pthread_create(&ptid_c, NULL, docap, NULL))
        printf("asr thread create failed\n");
    if (pthread_create(&ptid_p, NULL, doplay, NULL))
        printf("asr thread create failed\n");
}

void asr_audio_deinit(void)
{
    if (running == 0)
        return;

    running = 0;
    cmd_answer(INT_MAX);
    pthread_join(ptid_c, NULL);
    ptid_c = 0;
    pthread_join(ptid_p, NULL);
    ptid_p = 0;
    close(pipe_fd[0]);
    pipe_fd[0] = 0;
    close(pipe_fd[1]);
    pipe_fd[1] = 0;
}
#endif

