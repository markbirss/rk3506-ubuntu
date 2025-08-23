################################################################################
#
# frecon
#
################################################################################

FRECON_VERSION = c6150b5173371cde887c6ef7f4be20e866b21686
FRECON_SITE = https://chromium.googlesource.com/chromiumos/platform/frecon
FRECON_SITE_METHOD = git
FRECON_LICENSE = ChromiumOS
FRECON_LICENSE_FILES = LICENSE

FRECON_DEPENDENCIES = host-python3 libdrm libpng libtsm udev

FRECON_MAKE_ENV = \
	PKG_CONFIG=$(PKG_CONFIG_HOST_BINARY) \
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_OPTS) OUT=$(@D)/ \
	CHROMEOS=0 DRM_NO_MASTER=1 USE_UNIFONT=1

ifeq ($(BR2_PACKAGE_FRECON_STATIC),y)
FRECON_MAKE_ENV += CFLAGS="$(TARGET_CFLAGS) -static" LDLIBS="-lm"
endif

define FRECON_BUILD_CMDS
	$(FRECON_MAKE_ENV) $(MAKE) -C $(@D)
endef

ifeq ($(BR2_PACKAGE_FRECON_USE_GETTY),y)
FRECON_ENV += "export FRECON_SHELL=getty"
else ifeq ($(BR2_PACKAGE_BASH),y)
FRECON_ENV += "export FRECON_SHELL=bash"
else
FRECON_ENV += "export FRECON_SHELL=sh"
endif

ifeq ($(BR2_PACKAGE_FRECON_VTS),y)
FRECON_ENV += "export FRECON_VTS=1"
endif

ifeq ($(BR2_PACKAGE_FRECON_VT1),y)
FRECON_ENV += "export FRECON_VT1=1"
endif

FRECON_ENV += "export FRECON_FB_ROTATE=$(BR2_PACKAGE_FRECON_ROTATE)"
FRECON_ENV += "export FRECON_FB_SCALE=$(BR2_PACKAGE_FRECON_SCALE)"
FRECON_ENV += "export FRECON_OUTPUT_CONFIG=$(BR2_PACKAGE_FRECON_OUTPUT_CONFIG)"

define FRECON_INSTALL_TARGET_CMDS
	cp $(@D)/frecon $(TARGET_DIR)/usr/bin/
	cp -rp $(FRECON_PKGDIR)/frecon $(TARGET_DIR)/etc/
endef

define FRECON_INSTALL_INIT_SYSV
	$(INSTALL) -D -m 0755 $(FRECON_PKGDIR)/S35frecon \
		$(TARGET_DIR)/etc/init.d/
endef
FRECON_INSTALL_INIT_SYSV_HOOKS += FRECON_INSTALL_INIT_SYSV

define FRECON_INSTALL_INIT_SYSTEMD
	$(INSTALL) -D -m 0755 $(FRECON_PKGDIR)/S35frecon \
		$(TARGET_DIR)/etc/init.d/frecon
	$(INSTALL) -D -m 0644 $(FRECON_PKGDIR)/frecon.service \
		$(TARGET_DIR)/usr/lib/systemd/system/
endef
FRECON_INSTALL_INIT_SYSTEMD_HOOKS += FRECON_INSTALL_INIT_SYSTEMD

define FRECON_INSTALL_TARGET_ENV
	echo $(FRECON_ENV) | xargs -n 2 > \
                $(TARGET_DIR)/etc/profile.d/frecon.sh
endef
FRECON_POST_INSTALL_TARGET_HOOKS += FRECON_INSTALL_TARGET_ENV

$(eval $(generic-package))
