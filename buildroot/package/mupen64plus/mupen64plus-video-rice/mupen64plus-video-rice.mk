################################################################################
#
# mupen64plus-video-rice
#
################################################################################
MUPEN64PLUS_VIDEO_RICE_VERSION = fcf00779f08a9503ef30d26422f6b0350684820d
MUPEN64PLUS_VIDEO_RICE_SITE = $(call github,mupen64plus,mupen64plus-video-rice,$(MUPEN64PLUS_VIDEO_RICE_VERSION))

MUPEN64PLUS_VIDEO_RICE_DEPENDENCIES += \
	mupen64plus-core \
	host-pkgconf \
	sdl2 \
	libpng

MUPEN64PLUS_VIDEO_RICE_CONF += UNAME=linux

ifeq ($(BR2_arm),y)
MUPEN64PLUS_VIDEO_RICE_CONF += HOST_CPU=arm
else ifeq ($(BR2_aarch64),y)
MUPEN64PLUS_VIDEO_RICE_CONF += HOST_CPU=arm64
endif

ifeq ($(BR2_PACKAGE_HAS_LIBGLES)$(BR2_PACKAGE_SDL2_OPENGLES),yy)
MUPEN64PLUS_VIDEO_RICE_DEPENDENCIES += libgles
MUPEN64PLUS_VIDEO_RICE_CONF += USE_GLES=1
endif

define MUPEN64PLUS_VIDEO_RICE_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		PKG_CONFIG="$(PKG_CONFIG_HOST_BINARY)" \
		PREFIX=/usr/ APIDIR="$(STAGING_DIR)/usr/include/mupen64plus" \
		$(MAKE) -C $(@D)/projects/unix all \
		CC="$(TARGET_CC)" CXX="$(TARGET_CXX)" LD="$(TARGET_CC)" \
		RANLIB="$(TARGET_RANLIB)" AR="$(TARGET_AR)" \
		SDL_CONFIG="$(STAGING_DIR)/usr/bin/sdl2-config" \
		$(MUPEN64PLUS_VIDEO_RICE_CONF)
endef

define MUPEN64PLUS_VIDEO_RICE_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/lib/mupen64plus
	$(INSTALL) -m 0644 $(@D)/projects/unix/mupen64plus-video-rice.so \
		$(TARGET_DIR)/usr/lib/mupen64plus

	mkdir -p $(TARGET_DIR)/usr/share/mupen64plus
	$(INSTALL) -m 0644 $(@D)/data/RiceVideoLinux.ini \
		$(TARGET_DIR)/usr/share/mupen64plus
endef

$(eval $(generic-package))
