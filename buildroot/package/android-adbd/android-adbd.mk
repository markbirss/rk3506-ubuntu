################################################################################
#
# android-adbd
#
################################################################################

ANDROID_ADBD_VERSION = 8.1.0+r23-8
ANDROID_ADBD_FULL_VERSION = 1%$(ANDROID_ADBD_VERSION)
ANDROID_ADBD_SITE = https://salsa.debian.org/android-tools-team/android-platform-system-core/-/archive/debian/$(ANDROID_ADBD_FULL_VERSION)
ANDROID_ADBD_SOURCE = android-platform-system-core-debian-$(ANDROID_ADBD_FULL_VERSION).tar.gz
ANDROID_ADBD_LICENSE = Apache-2.0
ANDROID_ADBD_LICENSE_FILES = debian/copyright
ANDROID_ADBD_DEPENDENCIES = host-pkgconf openssl

ANDROID_ADBD_STATIC = $(BR2_PACKAGE_ANDROID_ADBD_STATIC)

ifeq ($(ANDROID_ADBD_STATIC),y)
ANDROID_ADBD_CFLAGS += -static
ANDROID_ADBD_CXXFLAGS += -static
ANDROID_ADBD_FCFLAGS += -static
ANDROID_ADBD_LDFLAGS += -static
endif

# Apply the Debian patches before applying the Buildroot patches
define ANDROID_ADBD_DEBIAN_PATCH
	$(APPLY_PATCHES) $(@D) $(@D)/debian/patches \*
endef
ANDROID_ADBD_PRE_PATCH_HOOKS += ANDROID_ADBD_DEBIAN_PATCH

define ANDROID_ADBD_INSTALL_TARGET_LOGCAT
	echo -e "#!/bin/sh\ntail -f \$${@:--n 99999999} /var/log/messages" > \
		$(TARGET_DIR)/usr/bin/logcat
	chmod 755 $(TARGET_DIR)/usr/bin/logcat
endef
ANDROID_ADBD_POST_INSTALL_TARGET_HOOKS += ANDROID_ADBD_INSTALL_TARGET_LOGCAT

define ANDROID_ADBD_INSTALL_TARGET_SHELL
	mkdir -p $(TARGET_DIR)/etc/profile.d
	echo "[ -x /bin/bash ] && export ADBD_SHELL=/bin/bash" > \
		$(TARGET_DIR)/etc/profile.d/adbd.sh
endef
ANDROID_ADBD_PRE_INSTALL_TARGET_HOOKS += ANDROID_ADBD_INSTALL_TARGET_SHELL

ifneq ($(BR2_PACKAGE_ANDROID_ADBD_TCP_PORT),0)
define ANDROID_ADBD_INSTALL_TARGET_TCP_PORT
	echo "export ADB_TCP_PORT=$(BR2_PACKAGE_ANDROID_ADBD_TCP_PORT)" >> \
		$(TARGET_DIR)/etc/profile.d/adbd.sh
endef
ANDROID_ADBD_POST_INSTALL_TARGET_HOOKS += ANDROID_ADBD_INSTALL_TARGET_TCP_PORT
endif

ifneq ($(BR2_PACKAGE_ANDROID_ADBD_SECURE),)

define ANDROID_ADBD_INSTALL_TARGET_SECURE
	echo "export ADB_SECURE=1" >> $(TARGET_DIR)/etc/profile.d/adbd.sh
endef
ANDROID_ADBD_POST_INSTALL_TARGET_HOOKS += ANDROID_ADBD_INSTALL_TARGET_SECURE

ANDROID_ADBD_PASSWORD = $(call qstrip,$(BR2_PACKAGE_ANDROID_ADBD_PASSWORD))
ifneq ($(ANDROID_ADBD_PASSWORD),)
ANDROID_ADBD_PASSWORD_MD5=$(shell echo $(ANDROID_ADBD_PASSWORD) | md5sum)

define ANDROID_ADBD_INSTALL_TARGET_PASSWORD
	$(INSTALL) -D -m 0755 $(ANDROID_ADBD_PKGDIR)/adbd-auth \
		$(TARGET_DIR)/usr/bin/adbd-auth
	sed -i "s/ADBD_PASSWORD_MD5/${ANDROID_ADBD_PASSWORD_MD5}/g" \
		$(TARGET_DIR)/usr/bin/adbd-auth
endef
ANDROID_ADBD_POST_INSTALL_TARGET_HOOKS += ANDROID_ADBD_INSTALL_TARGET_PASSWORD
endif

ANDROID_ADBD_KEYS = $(call qstrip,$(BR2_PACKAGE_ANDROID_ADBD_KEYS))
ifneq ($(ANDROID_ADBD_KEYS),)
define ANDROID_ADBD_INSTALL_TARGET_KEYS
	cat $(ANDROID_ADBD_KEYS) > $(TARGET_DIR)/adb_keys
endef
ANDROID_ADBD_POST_INSTALL_TARGET_HOOKS += ANDROID_ADBD_INSTALL_TARGET_KEYS
endif

endif # BR2_PACKAGE_ANDROID_ADBD_SECURE

$(eval $(meson-package))
