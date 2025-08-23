编译：
$YOUR_COMPILE_PATH/arm-rockchip830-linux-uclibcgnueabihf-gcc -o test_sed sed_main.c -I include/ -L ../uclibc/ -lrkaudio_detect -lrkaudio_common

./test_sed total.wav outsed.wav /oem/usr/share/vqefiles/rkaudio_model_sed_bcd.rknn

执行命令：
./test_sed 输入音频文件地址  输出音频文件地址  SED模型全路径

例如：
./test_sed sed_in.wav sed_out.wav /oem/usr/share/vqefiles/rkaudio_model_sed_bcd.rknn

其中outsed.wav，第一通道为语音信噪比snr_db的检测结果，第二通道为超大声lsd_db的检测结果，第三通道为哭声BCD的检测结果。

带有outfile的lib，在设定环境变量PATH_ORI_IN，PATH_NET_IN，PATH_NET_OUT后
将在对应位置生成ori_in.wav  net_in.wav  net_out.wav用于分析样例
例如：export PATH_ORI_IN=/USERDATA/ori_in.wav

