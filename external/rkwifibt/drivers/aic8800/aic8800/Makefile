CONFIG_AIC_LOADFW_SUPPORT := m
CONFIG_AIC8800_WLAN_SUPPORT := m

obj-$(CONFIG_AIC_LOADFW_SUPPORT)    += aic_load_fw/
obj-$(CONFIG_AIC8800_WLAN_SUPPORT) += aic8800_fdrv/

########## config option ##########
export CONFIG_USE_FW_REQUEST = n
export CONFIG_PREALLOC_RX_SKB = y
export CONFIG_PREALLOC_TXQ = y
###################################

########## platform support list ##########
export CONFIG_PLATFORM_ROCKCHIP = y
export CONFIG_PLATFORM_ALLWINNER = n
export CONFIG_PLATFORM_AMLOGIC = n
export CONFIG_PLATFORM_HI = n
export CONFIG_PLATFORM_UBUNTU = n

ifeq ($(CONFIG_PLATFORM_ROCKCHIP), y)
ARCH = arm
KDIR ?= $(RK_SDK_DIR)/kernel-6.1
CROSS_COMPILE ?= $(RK_SDK_DIR)/prebuilts/gcc/linux-x86/arm/gcc-arm-none-eabi-10-2020-q4-major-x86_64-linux/bin/arm-none-eabi-
endif

KVER := $(shell uname -r)
MODDESTDIR ?= /lib/modules/$(KVER)/kernel/drivers/net/wireless/
###########################################

MAKEFLAGS +=-j$(shell nproc)

all: modules
modules:
	make -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

install:
	mkdir -p $(MODDESTDIR)
	install -p -m 644 aic_load_fw/aic_load_fw.ko  $(MODDESTDIR)/
	install -p -m 644 aic8800_fdrv/aic8800_fdrv.ko  $(MODDESTDIR)/
	#/sbin/depmod -a ${KVER}

uninstall:
	rm -rfv $(MODDESTDIR)/aic_load_fw.ko
	rm -rfv $(MODDESTDIR)/aic8800_fdrv.ko
	#/sbin/depmod -a ${KVER}

clean:
	cd aic_load_fw/;make clean;cd ..
	cd aic8800_fdrv/;make clean;cd ..
	rm -rf modules.order Module.symvers .modules.order.cmd .Module.symvers.cmd .tmp_versions/
