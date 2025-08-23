################################################################################
#
# umtprd
#
################################################################################

UMTPRD_VERSION = 7d7d099df0dd929f95de0f45a2b6b006545207c6
UMTPRD_SITE = https://github.com/viveris/uMTP-Responder.git
UMTPRD_SITE_METHOD = git
UMTPRD_LICENSE = GPL-3.0+
UMTPRD_LICENSE_FILES = LICENSE

ifeq ($(BR2_PACKAGE_SYSTEMD),y)
UMTPRD_DEPENDENCIES = systemd
UMTPRD_MAKE_OPTS += SYSTEMD=1
endif

ifeq ($(BR2_PACKAGE_UMTPRD_USE_SYSLOG),y)
UMTPRD_MAKE_OPTS += USE_SYSLOG=1
endif

ifeq ($(BR2_PACKAGE_UMTPRD_STATIC),y)
UMTPRD_MAKE_OPTS += CC="$(TARGET_CC) -static"
endif

define UMTPRD_FIXUP_USB_PATH
	sed -i -e "s~/dev/ffs-umtp/~/dev/usb-ffs/mtp/~" \
		$(@D)/inc/default_cfg.h || true
endef
UMTPRD_POST_PATCH_HOOKS += UMTPRD_FIXUP_USB_PATH

define UMTPRD_BUILD_CMDS
	$(TARGET_CONFIGURE_OPTS) $(MAKE) $(UMTPRD_MAKE_OPTS) -C $(@D)
endef

define UMTPRD_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/umtprd $(TARGET_DIR)/usr/bin/umtprd
endef

$(eval $(generic-package))
