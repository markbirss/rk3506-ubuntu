################################################################################
#
# android-adb
#
################################################################################

ANDROID_ADB_VERSION = 10.0.0+r36-10
ANDROID_ADB_FULL_VERSION = 1%25$(ANDROID_ADB_VERSION)
ANDROID_ADB_SITE = https://salsa.debian.org/android-tools-team/android-platform-system-core/-/archive/debian/$(ANDROID_ADB_FULL_VERSION)
ANDROID_ADB_SOURCE = android-platform-system-core-debian-$(ANDROID_ADB_FULL_VERSION).tar.gz
ANDROID_ADB_LICENSE = Apache-2.0
ANDROID_ADB_LICENSE_FILES = debian/copyright
ANDROID_ADB_DEPENDENCIES = host-pkgconf libusb openssl

ANDROID_ADB_STATIC = $(BR2_PACKAGE_ANDROID_ADB_STATIC)

ifeq ($(ANDROID_ADB_STATIC),y)
ANDROID_ADB_CFLAGS += -static
ANDROID_ADB_CXXFLAGS += -static
ANDROID_ADB_FCFLAGS += -static
ANDROID_ADB_LDFLAGS += -static
endif

# Apply the Debian patches before applying the Buildroot patches
define ANDROID_ADB_DEBIAN_PATCH
	$(APPLY_PATCHES) $(@D) $(@D)/debian/patches \*
endef
ANDROID_ADB_PRE_PATCH_HOOKS += ANDROID_ADB_DEBIAN_PATCH

$(eval $(meson-package))
