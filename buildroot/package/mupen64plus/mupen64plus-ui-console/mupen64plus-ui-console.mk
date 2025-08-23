################################################################################
#
# mupen64plus-ui-console
#
################################################################################
MUPEN64PLUS_UI_CONSOLE_VERSION = beddd15785663f1c3b4e9476998defea36223cf3
MUPEN64PLUS_UI_CONSOLE_SITE = $(call github,mupen64plus,mupen64plus-ui-console,$(MUPEN64PLUS_UI_CONSOLE_VERSION))

MUPEN64PLUS_UI_CONSOLE_DEPENDENCIES += mupen64plus-core sdl2

MUPEN64PLUS_UI_CONSOLE_CONF += UNAME=linux

ifeq ($(BR2_arm),y)
MUPEN64PLUS_UI_CONSOLE_CONF += HOST_CPU=arm
else ifeq ($(BR2_aarch64),y)
MUPEN64PLUS_UI_CONSOLE_CONF += HOST_CPU=arm64
endif

define MUPEN64PLUS_UI_CONSOLE_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		PREFIX=/usr/ APIDIR="$(STAGING_DIR)/usr/include/mupen64plus" \
		$(MAKE) -C $(@D)/projects/unix all \
		CC="$(TARGET_CC)" CXX="$(TARGET_CXX)" LD="$(TARGET_CC)" \
		RANLIB="$(TARGET_RANLIB)" AR="$(TARGET_AR)" \
		SDL_CONFIG="$(STAGING_DIR)/usr/bin/sdl2-config" \
		$(MUPEN64PLUS_UI_CONSOLE_CONF)
endef

define MUPEN64PLUS_UI_CONSOLE_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 0755 $(@D)/projects/unix/mupen64plus \
		$(TARGET_DIR)/usr/bin

	mkdir -p $(TARGET_DIR)/usr/share/applications
	$(INSTALL) -m 0644 $(@D)/data/mupen64plus.desktop \
		$(TARGET_DIR)/usr/share/applications

	mkdir -p $(TARGET_DIR)/usr/share/icons/hicolor
	cp -r $(@D)/data/icons/* $(TARGET_DIR)/usr/share/icons/hicolor
endef

$(eval $(generic-package))
