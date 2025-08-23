MODULE_NAME = aic_btusb

CONFIG_AIC8800_BTUSB_SUPPORT = m
CONFIG_SUPPORT_VENDOR_APCF = n
# Need to set fw path in BOARD_KERNEL_CMDLINE
CONFIG_USE_FW_REQUEST = n

ifeq ($(CONFIG_SUPPORT_VENDOR_APCF), y)
obj-$(CONFIG_AIC8800_BTUSB_SUPPORT) := $(MODULE_NAME).o aic_btusb_external_featrue.o
else
obj-$(CONFIG_AIC8800_BTUSB_SUPPORT) := $(MODULE_NAME).o
endif

ccflags-$(CONFIG_SUPPORT_VENDOR_APCF) += -DCONFIG_SUPPORT_VENDOR_APCF
#$(MODULE_NAME)-y := aic_btusb_ioctl.o\
#	aic_btusb.o \

ccflags-$(CONFIG_USE_FW_REQUEST) += -DCONFIG_USE_FW_REQUEST


# Platform support list
CONFIG_PLATFORM_ROCKCHIP ?= y
CONFIG_PLATFORM_ALLWINNER ?= n
CONFIG_PLATFORM_AMLOGIC ?= n
CONFIG_PLATFORM_UBUNTU ?= n

ifeq ($(CONFIG_PLATFORM_ROCKCHIP), y)
ccflags-$(CONFIG_PLATFORM_ROCKCHIP) += -DCONFIG_PLATFORM_ROCKCHIP
ccflags-y += -Wno-unused-function -Wno-unused-variable
ARCH = arm
KDIR ?= $(RK_SDK_DIR)/kernel-6.1
CROSS_COMPILE ?= $(RK_SDK_DIR)/prebuilts/gcc/linux-x86/arm/gcc-arm-10.3-2021.07-x86_64-arm-none-linux-gnueabihf/bin/arm-none-linux-gnueabihf-
endif

KVER := $(shell uname -r)
MODDESTDIR = /lib/modules/$(KVER)/kernel/drivers/net/wireless/bluetooth

all: modules
modules:
	make -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

install:
	mkdir -p $(MODDESTDIR)
	install -p -m 644 $(MODULE_NAME).ko  $(MODDESTDIR)/
	#/sbin/depmod -a ${KVER}
	#echo $(MODULE_NAME) >> $(MODDESTDIR)/etc/modules-load.d/aic_bt.conf

clean:
	rm -rf *.o *.ko *.o.* *.mod.* modules.* Module.* .a* .o* .*.o.* *.mod .tmp* .cache.mk .Module.symvers.cmd .modules.order.cmd
