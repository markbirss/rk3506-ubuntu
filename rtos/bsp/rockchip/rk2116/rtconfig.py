import os

# toolchains options
ARCH        ='risc-v'
CPU         ='nuclei'
VENDOR      ='nuclei'
CROSS_TOOL  ='gcc'

if os.getenv('RTT_CC'):
    CROSS_TOOL = os.getenv('RTT_CC')

if  CROSS_TOOL == 'gcc':
    PLATFORM    = 'gcc'
    EXEC_PATH   = '../../../../../prebuilts/gcc/linux-x86/riscv64/nuclei_riscv_newlibc_prebuilt_linux64_2024.06/bin'
else:
    print ('Please make sure your toolchains is GNU GCC!')
    exit(0)

if os.getenv('RTT_EXEC_PATH'):
    EXEC_PATH = os.getenv('RTT_EXEC_PATH')

# BUILD = 'debug'
BUILD = 'release'

if PLATFORM == 'gcc':
    # toolchains
    PREFIX  = 'riscv64-unknown-elf-'
    CC      = PREFIX + 'gcc'
    CXX     = PREFIX + 'g++'
    AS      = PREFIX + 'gcc'
    AR      = PREFIX + 'ar'
    LINK    = PREFIX + 'g++'
    TARGET_EXT = 'elf'
    SIZE    = PREFIX + 'size'
    OBJDUMP = PREFIX + 'objdump'
    OBJCPY  = PREFIX + 'objcopy'
    STRIP   = PREFIX + 'strip'
    DEVICE = ' -mtune=nuclei-300-series -march=rv32ima_zicond_zba_zbb_zbc_zbs_zca_zcb_zcmp_zcmt_xxldspn3x -mabi=ilp32 -mcmodel=medlow -DCPU_SERIES=300 -DBOOT_HARTID=0 -DRTOS_RTTHREAD -fpic'
    COMMON_FLAGS = ' '
    CFLAGS  = DEVICE + COMMON_FLAGS + ' -c -g -ffunction-sections -fdata-sections -Wall -mcmodel=medlow'
    AFLAGS  = ' -c' + DEVICE + COMMON_FLAGS + ' -D__ASSEMBLY__ -x assembler-with-cpp'
    LFLAGS  = DEVICE + ' -nostartfiles -Wl,--no-whole-archive -lm -lc -lgcc -Wl,-gc-sections -Wl,-zmax-page-size=1024 -Wl,-Map=rtt.map,-u,Reset_Handler'
    CPATH   = ''
    LPATH   = ''
    LINK_SCRIPT = 'gcc_riscv.ld'
    LFLAGS += ' -T %s' % LINK_SCRIPT

    if BUILD == 'debug':
        CFLAGS += ' -O0 -gdwarf-2'
        AFLAGS += ' -gdwarf-2'
    else:
        CFLAGS += ' -O2 -g2'

    CXXFLAGS = CFLAGS

DUMP_ACTION = OBJDUMP + ' -D -S $TARGET > rtt.asm\n'
POST_ACTION = OBJCPY + ' -O binary $TARGET rtthread.bin\n' + SIZE + ' $TARGET \n' + DUMP_ACTION
