import os

# toolchains options
ARCH='arm'
CPU='cortex-m33'
CROSS_TOOL='gcc'

if os.getenv('RTT_CC'):
    CROSS_TOOL = os.getenv('RTT_CC')

# cross_tool provides the cross compiler
# EXEC_PATH is the compiler execute path, for example, CodeSourcery, Keil MDK, IAR
if  CROSS_TOOL == 'gcc':
    PLATFORM = 'gcc'
    EXEC_PATH = '/usr/bin'
    #EXEC_PATH = '/opt/gcc-arm-none-eabi-7-2018-q2-update/bin'
elif CROSS_TOOL == 'keil':
    PLATFORM = 'armcc'
    EXEC_PATH = 'C:/Keil'
elif CROSS_TOOL == 'iar':
    print('================ERROR============================')
    print('Not support iar yet!')
    print('=================================================')
    exit(0)

if os.getenv('RTT_EXEC_PATH'):
    EXEC_PATH = os.getenv('RTT_EXEC_PATH')

#BUILD = 'debug'
BUILD = 'release'

#XIP = 'Y'
XIP = 'N'
if os.getenv('RTT_BUILD_XIP'):
    XIP = os.getenv('RTT_BUILD_XIP').upper()

if PLATFORM == 'gcc':
    # toolchains
    PREFIX = 'arm-none-eabi-'
    CC = PREFIX + 'gcc'
    AS = PREFIX + 'gcc'
    AR = PREFIX + 'ar'
    CXX = PREFIX + 'g++'
    LINK = PREFIX + 'gcc'
    TARGET_EXT = 'elf'
    SIZE = PREFIX + 'size'
    OBJDUMP = PREFIX + 'objdump'
    OBJCPY = PREFIX + 'objcopy'
    STRIP = PREFIX + 'strip'

    DEVICE = ' -mcpu=cortex-m33 -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard -ffunction-sections -fdata-sections'
    CFLAGS = DEVICE + ' -g -Wall -Werror=maybe-uninitialized -Werror=implicit-function-declaration -Werror=return-type -Werror=address -Werror=int-to-pointer-cast -Werror=pointer-to-int-cast '
    AFLAGS = ' -c' + DEVICE + ' -x assembler-with-cpp -Wa,-mimplicit-it=thumb -D__ASSEMBLY__ '
    LFLAGS = DEVICE + ' -lm -lgcc -lc' + ' -nostartfiles -Wl,--gc-sections,-Map=rtthread.map,-cref,-u,Reset_Handler '

    CPATH = ''
    LPATH = ''

    if XIP == 'Y':
        AFLAGS += ' -D__STARTUP_COPY_MULTIPLE -D__STARTUP_CLEAR_BSS_MULTIPLE'
        CFLAGS += ' -D__STARTUP_COPY_MULTIPLE -D__STARTUP_CLEAR_BSS_MULTIPLE -DRT_USING_XIP'
        LINK_SCRIPT = 'gcc_xip_on.ld'
    else:
        AFLAGS += ' -D__STARTUP_CLEAR_BSS'
        CFLAGS += ' -D__STARTUP_CLEAR_BSS'
        LINK_SCRIPT = 'gcc_xip_off.ld'

    LFLAGS += '-T %s' % LINK_SCRIPT

    if BUILD == 'debug':
        CFLAGS += ' -O0 -gdwarf-2'
        AFLAGS += ' -gdwarf-2'
    else:
        CFLAGS += ' -O2'

    POST_ACTION = OBJCPY + ' -O binary $TARGET rtthread.bin\n' + SIZE + ' $TARGET \n'

    M_CFLAGS = CFLAGS + ' -mlong-calls  -Dsourcerygxx -fPIC '
    M_LFLAGS = DEVICE + ' -Wl,--gc-sections,-z,max-page-size=0x4 -shared -fPIC -e main -nostartfiles -nostdlib -static-libgcc'
    M_POST_ACTION = STRIP + ' -R .hash $TARGET\n' + SIZE + ' $TARGET \n'

elif PLATFORM == 'armcc':
    # toolchains
    CC = 'armcc'
    AS = 'armasm'
    AR = 'armar'
    LINK = 'armlink'
    TARGET_EXT = 'axf'

    DEVICE = ' --device DARMSTM'
    CFLAGS = DEVICE + ' --apcs=interwork'
    AFLAGS = DEVICE
    LFLAGS = DEVICE + ' --info sizes --info totals --info unused --info veneers --list rtthread.map --scatter pisces_rom.sct'

    CFLAGS += ' -I' + EXEC_PATH + '/ARM/RV31/INC'
    LFLAGS += ' --libpath ' + EXEC_PATH + '/ARM/RV31/LIB'

    EXEC_PATH += '/arm/bin40/'

    if BUILD == 'debug':
        CFLAGS += ' -g -O0'
        AFLAGS += ' -g'
    else:
        CFLAGS += ' -O2'

    POST_ACTION = 'fromelf --bin $TARGET --output rtthread.bin \nfromelf -z $TARGET'

elif PLATFORM == 'iar':
    # toolchains
    CC = 'iccarm'
    AS = 'iasmarm'
    AR = 'iarchive'
    LINK = 'ilinkarm'
    TARGET_EXT = 'out'

    DEVICE = ' -D USE_STDPERIPH_DRIVER'

    CFLAGS = DEVICE
    CFLAGS += ' --diag_suppress Pa050'
    CFLAGS += ' --no_cse' 
    CFLAGS += ' --no_unroll' 
    CFLAGS += ' --no_inline' 
    CFLAGS += ' --no_code_motion' 
    CFLAGS += ' --no_tbaa' 
    CFLAGS += ' --no_clustering' 
    CFLAGS += ' --no_scheduling' 
    CFLAGS += ' --debug' 
    CFLAGS += ' --endian=little' 
    CFLAGS += ' --cpu=Cortex-M4' 
    CFLAGS += ' -e' 
    CFLAGS += ' --fpu=None'
    CFLAGS += ' --dlib_config "' + EXEC_PATH + '/arm/INC/c/DLib_Config_Normal.h"'    
    CFLAGS += ' -Ol'    
    CFLAGS += ' --use_c++_inline'
        
    AFLAGS = ''
    AFLAGS += ' -s+' 
    AFLAGS += ' -w+' 
    AFLAGS += ' -r' 
    AFLAGS += ' --cpu Cortex-M4' 
    AFLAGS += ' --fpu None' 

    LFLAGS = ' --config pisces_flash.icf'
    LFLAGS += ' --redirect _Printf=_PrintfTiny' 
    LFLAGS += ' --redirect _Scanf=_ScanfSmall' 
    LFLAGS += ' --entry __iar_program_start'    

    EXEC_PATH = EXEC_PATH + '/arm/bin/'
    POST_ACTION = ''
