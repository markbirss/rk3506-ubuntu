################################################################################
#
# Rockchip rkupdate For Linux
#
################################################################################

RKUPDATE_VERSION = develop
RKUPDATE_SITE = $(TOPDIR)/../external/rkupdate
RKUPDATE_SITE_METHOD = local

RKUPDATE_LICENSE = Apache V2.0
RKUPDATE_LICENSE_FILES = NOTICE

RKUPDATE_DEPENDENCIES = \
	libpthread-stubs util-linux

RKUPDATE_CFLAGS = $(TARGET_CFLAGS) -fPIC -lpthread -luuid

ifeq ($(BR2_PACKAGE_RKUPDATE_SINGNATURE_FW),y)
RKUPDATE_CFLAGS += -DUSE_SIGNATURE_FW=ON
endif

ifeq ($(BR2_PACKAGE_RKUPDATE_SIMULATE_ABNORMAL_POWER_OFF),y)
RKUPDATE_CFLAGS += -DUSE_SIMULATE_POWER_OFF=ON
endif

ifeq ($(BR2_PACKAGE_RKUPDATE_STATIC),y)
RKUPDATE_CFLAGS += -static
endif

define RKUPDATE_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D) \
		CXX="$(TARGET_CXX)" CFLAGS="$(RKUPDATE_CFLAGS)"
endef

define RKUPDATE_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 755 $(@D)/rkupdate $(TARGET_DIR)/usr/bin/
endef

$(eval $(generic-package))
