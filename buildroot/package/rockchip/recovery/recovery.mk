################################################################################
#
# Rockchip Recovery For Linux
#
################################################################################

RECOVERY_VERSION = develop
RECOVERY_SITE = $(TOPDIR)/../external/recovery
RECOVERY_SITE_METHOD = local

RECOVERY_LICENSE = ROCKCHIP
RECOVERY_LICENSE_FILES = NOTICE

RECOVERY_CFLAGS = $(TARGET_CFLAGS) -I. \
	-fPIC \
	-lpthread \
	-lcurl \
	-lbz2

RECOVERY_MAKE_ENV = $(TARGET_MAKE_ENV)

RECOVERY_DEPENDENCIES += libpthread-stubs util-linux libcurl bzip2

ifeq ($(BR2_PACKAGE_RECOVERY_NO_UI),y)
RECOVERY_MAKE_ENV += RecoveryNoUi=true
else
RECOVERY_CFLAGS += -lpng -ldrm -lz -lm -I$(STAGING_DIR)/usr/include/libdrm
RECOVERY_DEPENDENCIES += libpng libdrm libzlib
endif

ifeq ($(BR2_PACKAGE_RECOVERY_USE_RKUPDATE),y)
RECOVERY_CFLAGS += -DUSE_RKUPDATE=ON
endif

ifeq ($(BR2_PACKAGE_RECOVERY_USE_UPDATEENGINE),y)
RECOVERY_CFLAGS += -DUSE_UPDATEENGINE=ON
endif

ifeq ($(BR2_PACKAGE_RECOVERY_SUCCESSFUL_BOOT),y)
RECOVERY_CFLAGS += -DSUCCESSFUL_BOOT=ON
endif

ifeq ($(BR2_PACKAGE_RECOVERY_RETRY),y)
RECOVERY_CFLAGS += -DRETRY_BOOT=ON
endif

ifeq ($(BR2_PACKAGE_RECOVERY_STATIC),y)
RECOVERY_CFLAGS += -static

# For static link with libcurl
ifeq ($(BR2_PACKAGE_OPENSSL),y)
RECOVERY_CFLAGS += -lssl -lcrypto
RECOVERY_DEPENDENCIES += openssl
endif
ifeq ($(BR2_PACKAGE_RTMPDUMP),y)
RECOVERY_CFLAGS += -lrtmp
RECOVERY_DEPENDENCIES += rtmpdump
endif
endif

define RECOVERY_BUILD_CMDS
	$(RECOVERY_MAKE_ENV) $(MAKE) -C $(@D) \
		CC="$(TARGET_CC)" CFLAGS="$(RECOVERY_CFLAGS)"
endef

ifeq ($(BR2_PACKAGE_RECOVERY_RECOVERYBIN),y)
define RECOVERYBIN_INSTALL_TARGET
	$(INSTALL) -D -m 755 $(@D)/recovery $(TARGET_DIR)/usr/bin/

	mkdir -p $(TARGET_DIR)/res/images
	cp $(@D)/res/images/* $(TARGET_DIR)/res/images/
endef

define RECOVERY_INSTALL_INIT_SYSV
	$(INSTALL) -D -m 755 $(RECOVERY_PKGDIR)/S40recovery \
		$(TARGET_DIR)/etc/init.d/S40recovery
endef
endif

ifeq ($(BR2_PACKAGE_RECOVERY_BOOTCONTROL), y)
define BOOTCONTROLBIN_INSTALL_TARGET
	$(INSTALL) -D -m 755 $(@D)/update_engine/S99_bootcontrol \
		$(TARGET_DIR)/etc/init.d/
endef
endif

ifeq ($(BR2_PACKAGE_RECOVERY_UPDATEENGINEBIN),y)
define UPDATEENGINEBIN_INSTALL_TARGET
	$(INSTALL) -D -m 755 $(@D)/updateEngine $(TARGET_DIR)/usr/bin/
endef
endif

define RECOVERY_INSTALL_TARGET_CMDS
	$(RECOVERYBIN_INSTALL_TARGET)
	$(UPDATEENGINEBIN_INSTALL_TARGET)
	$(BOOTCONTROLBIN_INSTALL_TARGET)
endef

$(eval $(generic-package))
