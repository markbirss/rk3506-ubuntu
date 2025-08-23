export PATH=../../../prebuilts/gcc/linux-x86/aarch64/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin:$PATH

aarch64-none-linux-gnu-gcc ec_master_test_RK3568_MADHT1505BA1.c  -I ../ethercat_igh/include/ -L ../ethercat_igh/lib/ -g -lpthread -lethercat -o example

aarch64-none-linux-gnu-gcc Rockchip_MADHT1505BA1.c -I ../ethercat_igh/include/ -L ../ethercat_igh/lib/ -g -lethercat -lpthread -shared -fPIC -o libmadht1505ba1.so

aarch64-none-linux-gnu-gcc rk_test_two.c  -I ../ethercat_igh/include/ -L ../ethercat_igh/lib/ -L ./ -g -lmadht1505ba1 -lethercat -lpthread -o rk_test_two

aarch64-none-linux-gnu-gcc rk_test.c  -I ../ethercat_igh/include/ -L ../ethercat_igh/lib/ -L ./ -g -lmadht1505ba1 -lethercat -lpthread -o rk_test
