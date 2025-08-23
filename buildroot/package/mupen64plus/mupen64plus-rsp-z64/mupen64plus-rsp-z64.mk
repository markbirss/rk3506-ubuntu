################################################################################
#
# mupen64plus-rsp-z64
#
################################################################################
MUPEN64PLUS_RSP_Z64_VERSION = 1484988bec32ab567dc205b8fd9ff6482d5e5950
MUPEN64PLUS_RSP_Z64_SITE = $(call github,mupen64plus,mupen64plus-rsp-z64,$(MUPEN64PLUS_RSP_Z64_VERSION))

MUPEN64PLUS_RSP_Z64_DEPENDENCIES += \
	mupen64plus-core

MUPEN64PLUS_RSP_Z64_CONF += UNAME=linux

ifeq ($(BR2_arm),y)
MUPEN64PLUS_RSP_Z64_CONF += HOST_CPU=arm
else ifeq ($(BR2_aarch64),y)
MUPEN64PLUS_RSP_Z64_CONF += HOST_CPU=arm64
endif

define MUPEN64PLUS_RSP_Z64_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		PREFIX=/usr/ APIDIR="$(STAGING_DIR)/usr/include/mupen64plus" \
		$(MAKE) -C $(@D)/projects/unix all \
		CC="$(TARGET_CC)" CXX="$(TARGET_CXX)" LD="$(TARGET_CC)" \
		RANLIB="$(TARGET_RANLIB)" AR="$(TARGET_AR)" \
		$(MUPEN64PLUS_RSP_Z64_CONF)
endef

define MUPEN64PLUS_RSP_Z64_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/lib/mupen64plus
	$(INSTALL) -m 0644 $(@D)/projects/unix/mupen64plus-rsp-z64.so \
		$(TARGET_DIR)/usr/lib/mupen64plus
endef

$(eval $(generic-package))
