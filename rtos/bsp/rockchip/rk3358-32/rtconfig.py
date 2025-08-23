import os

# toolchains options
ARCH        ='arm'
CPU         ='cortex-a'
CROSS_TOOL  ='gcc'

if os.getenv('RTT_ROOT'):
    RTT_ROOT = os.getenv('RTT_ROOT')
else:
    RTT_ROOT = r'../../..'

if os.getenv('RTT_CC'):
    CROSS_TOOL = os.getenv('RTT_CC')

PLATFORM    = 'gcc'
EXEC_PATH   = r'/usr/bin/'

if os.getenv('RTT_EXEC_PATH'):
    EXEC_PATH = os.getenv('RTT_EXEC_PATH')

BUILD = 'debug'

if PLATFORM == 'gcc':
    # toolchains
    PREFIX = 'arm-none-eabi-'
    CC      = PREFIX + 'gcc'
    CXX     = PREFIX + 'g++'
    AS      = PREFIX + 'gcc'
    AR      = PREFIX + 'ar'
    LINK    = PREFIX + 'gcc'
    TARGET_EXT = 'elf'
    SIZE    = PREFIX + 'size'
    OBJDUMP = PREFIX + 'objdump'
    OBJCPY  = PREFIX + 'objcopy'

    DEVICE = ' -mcpu=cortex-a35+crypto -mfloat-abi=hard -mfpu=neon-vfpv4 -marm -ffast-math'
    CFLAGS = DEVICE + ' -std=gnu99 -Wall -g -Wno-stringop-truncation -D__RT_THREAD__'
    AFLAGS = DEVICE + ' -c -x assembler-with-cpp -D__ASSEMBLY__'
    LINK_SCRIPT = 'gcc_arm.ld.S'
    LFLAGS  = DEVICE + ' -lm -lgcc -lc' + ' -nostartfiles -Wl,--gc-sections,-Map=rtthread.map,-cref,-u,system_vectors -T gcc_arm.ld'
    CPATH   = ''
    LPATH   = ''

    if BUILD == 'debug':
        CFLAGS += ' -O0 -gdwarf-2'
        AFLAGS += ' -gdwarf-2'
    else:
        CFLAGS += ' -O2'

    if os.getenv('RTT_PRMEM_BASE'):
        PRMEM_BASE = os.getenv('RTT_PRMEM_BASE')
    else:
        PRMEM_BASE = 0x02600000

    if os.getenv('RTT_PRMEM_SIZE'):
        PRMEM_SIZE = os.getenv('RTT_PRMEM_SIZE')
    else:
        PRMEM_SIZE = 0x00900000

    if os.getenv('RTT_SHMEM_BASE'):
        SHMEM_BASE = os.getenv('RTT_SHMEM_BASE')
    else:
        SHMEM_BASE = 0x02f00000

    if os.getenv('RTT_SHMEM_SIZE'):
        SHMEM_SIZE = os.getenv('RTT_SHMEM_SIZE')
    else:
        SHMEM_SIZE = 0x00100000

    CUR_CPU = os.getenv('CUR_CPU')
    if CUR_CPU == '1':
        CFLAGS += ' -DPRIMARY_CPU'

    CFLAGS += ' -DFIRMWARE_BASE={a} -DDRAM_SIZE={b} -DSHMEM_BASE={c} -DSHMEM_SIZE={d}'.format(a=PRMEM_BASE, b=PRMEM_SIZE, c=SHMEM_BASE, d=SHMEM_SIZE)

    CXXFLAGS = CFLAGS

DUMP_ACTION = OBJDUMP + ' -D -S $TARGET > rtt.asm\n'
POST_ACTION = OBJCPY + ' -O binary $TARGET rtthread.bin\n' + SIZE + ' $TARGET \n'

