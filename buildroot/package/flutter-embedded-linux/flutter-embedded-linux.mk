################################################################################
#
# flutter-embedded-linux
#
################################################################################

FLUTTER_EMBEDDED_LINUX_VERSION = e76c956498
FLUTTER_EMBEDDED_LINUX_SITE = https://github.com/sony/flutter-embedded-linux.git
FLUTTER_EMBEDDED_LINUX_SITE_METHOD = git
FLUTTER_EMBEDDED_LINUX_LICENSE = BSD-3-Clause
FLUTTER_EMBEDDED_LINUX_LICENSE_FILES = LICENSE
FLUTTER_EMBEDDED_LINUX_DEPENDENCIES = \
	flutter-engine \
	libxkbcommon

define FLUTTER_EMBEDDED_LINUX_LINK_ENGINE
	mkdir -p $(@D)/build/
	$(INSTALL) -D -m 0755 $(STAGING_DIR)/usr/lib/libflutter_engine.so \
		$(@D)/build/
endef
FLUTTER_EMBEDDED_LINUX_PRE_BUILD_HOOKS += FLUTTER_EMBEDDED_LINUX_LINK_ENGINE

FLUTTER_EMBEDDED_LINUX_CONF_OPTS += \
	-DBACKEND_TYPE=WAYLAND \
	-DCMAKE_BUILD_TYPE=Release \
	-DENABLE_ELINUX_EMBEDDER_LOG=ON \
	-DFLUTTER_RELEASE=ON

ifneq ($(BR2_PACKAGE_FLUTTER_EMBEDDED_LINUX_SO),y)
define FLUTTER_EMBEDDED_LINUX_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/flutter-client $(TARGET_DIR)/usr/bin/
endef
else
FLUTTER_EMBEDDED_LINUX_CONF_OPTS += \
	-DBUILD_ELINUX_SO=ON

define FLUTTER_EMBEDDED_LINUX_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/libflutter_elinux_wayland.so \
		$(TARGET_DIR)/usr/lib/
endef
endif

$(eval $(cmake-package))
