#include <stdio.h>
#include <time.h>

#include "audio/wave_reader.h"
#include "audio/wave_writer.h"
#include "skv/rkaudio_sed.h"

#define IN_SIZE 256
#define ENABLE_NPU 1
#define ENABLE_DUMP_PCM 0

int main(int argc, char *argv[])
{
    int ret = 0;
    double Total_sample = 0.0;
    // 读取数据并处理
    clock_t startTime, endTime;
    clock_t clk_start, clk_end;

    /* For Debug  */
    int out_size = 0, in_size = 0, res = 0;

    if (argc < 3)
    {
        printf("Error: Wrong input parameters! A example is as following: \n");
        printf("fosafer_enh test_in.wav test_out.wav 0\n");
        exit(-1);
    }

    char *in_filename = argv[1];
    char *out_filename = argv[2];

#if ENABLE_DUMP_PCM
    putenv("PATH_ORI_IN=/userdata/ori_in.wav");
    putenv("PATH_NET_IN=/userdata/net_in.wav");
    putenv("PATH_NET_OUT=/userdata/net_out.wav");
    putenv("DIR_NEG_SAMPLE=/userdata/neg_sample/");
#endif

#if ENABLE_NPU
    char *sed_rknn_path = argv[3];
    printf("sed_rknn_path is %s\n", sed_rknn_path);
    int tmpres = rkaudio_sed_bcd_model_set(sed_rknn_path);
#endif
    // for wave reader
    wave_reader *wr;
    wave_reader_error rerror;

    // for wave writer
    wave_writer *ww;
    wave_writer_error werror;
    wave_writer_format format;

    // 读取输入音频
    wr = wave_reader_open(in_filename, &rerror);
    if (!wr)
    {
        printf("rerror=%d\n", rerror);
        return -1;
    }

    int mSampleRate = wave_reader_get_sample_rate(wr);
    int mBitPerSample = wave_reader_get_sample_bits(wr);
    int mNumChannel = wave_reader_get_num_channels(wr);

    // 输入检查
    if (mNumChannel > 1)
    {
        printf("This algorithm is a single channel algorithm and will run on the first channel of data\n");
    }

    // 每次读取数据大小
    int read_size = IN_SIZE * mNumChannel * mBitPerSample / 8;
    char *in = (char *)calloc(1, read_size * sizeof(char));
    int out_res_num = 3; // ch0:snr_db ch1:lsd_db ch2:bcd
    short *out = (short *)calloc(1, IN_SIZE * out_res_num * sizeof(short));
    // 输出音频格式设置
    format.num_channels = out_res_num;
    format.sample_rate = mSampleRate;
    format.sample_bits = mBitPerSample;
    ww = wave_writer_open(out_filename, &format, &werror);
    // 输出音频建立失败
    if (!ww)
    {
        printf("werror=%d\n", werror);
        wave_reader_close(wr);
        return -1;
    }

    // 声音事件检测初始化
    RKAudioSedRes sed_res;
    RKAudioSedParam *sed_param = rkaudio_sed_param_init();
    void *st_sed = rkaudio_sed_init(mSampleRate, mBitPerSample, mNumChannel, sed_param);
    char initres = rkaudio_sed_init_res(st_sed);
    rkaudio_sed_param_destroy(sed_param);
    if (st_sed == NULL)
    {
        printf("Failed to create baby cry handle\n");
        return -1;
    }

loop_again:
    startTime = clock();
    int cnt = 0;

    int snr_cnt = 0, lsd_cnt = 0, bcd_cnt = 0;
    while (0 < (res = wave_reader_get_samples(wr, IN_SIZE, in)))
    {
        in_size = res * (mBitPerSample / 8) * mNumChannel;
        cnt++;
        clk_start = clock();
        out_size = rkaudio_sed_process(st_sed, (short *)in, in_size / 2, &sed_res);
        float lsd_res =  rkaudio_sed_lsd_db(st_sed);
        if (out_size < 0)
            fprintf(stderr, "bcd process return error=%d\n", out_size);

        clk_end = clock();

        if (sed_res.bcd_res == 1)
            bcd_cnt++;

        // 输出，测试用
        for (int i = 0; i < IN_SIZE; i++)
        {
            *(out + out_res_num * i)     = 10000 * sed_res.snr_res;
            *(out + out_res_num * i + 1) = 10000 * sed_res.lsd_res;
            *(out + out_res_num * i + 2) = 10000 * sed_res.bcd_res;
        }
        wave_writer_put_samples(ww, IN_SIZE, out);
        Total_sample += in_size / 2 / mNumChannel;
    }
    endTime = clock();

    printf("\nFinished, speech_time = %f, cost_time = %f snr_cnt: %d lsd_cnt: %d bcd_cnt: %d\n", \
           Total_sample / mSampleRate, (double)(endTime - startTime) / CLOCKS_PER_SEC,
           snr_cnt, lsd_cnt, bcd_cnt);

    wave_writer_close(ww, &werror);
    wave_reader_close(wr);

    free(in);
    free(out);

    // 释放
    if (st_sed)
        rkaudio_sed_destroy(st_sed);

    printf("Sed test finished\n");
    return 0;
}
