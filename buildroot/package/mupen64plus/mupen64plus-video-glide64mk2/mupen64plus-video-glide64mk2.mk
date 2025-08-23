################################################################################
#
# mupen64plus-video-glide64mk2
#
################################################################################
MUPEN64PLUS_VIDEO_GLIDE64MK2_VERSION = 39a8c11e8a041e16bcc6d67ebc2281b8632ba5ed
MUPEN64PLUS_VIDEO_GLIDE64MK2_SITE = $(call github,mupen64plus,mupen64plus-video-glide64mk2,$(MUPEN64PLUS_VIDEO_GLIDE64MK2_VERSION))

MUPEN64PLUS_VIDEO_GLIDE64MK2_DEPENDENCIES += \
	mupen64plus-core \
	host-pkgconf \
	sdl2

MUPEN64PLUS_VIDEO_GLIDE64MK2_CONF += \
	UNAME=linux HIRES=0 OPTFLAGS=$(TARGET_OPTIMIZATION)

ifeq ($(BR2_arm),y)
MUPEN64PLUS_VIDEO_GLIDE64MK2_CONF += HOST_CPU=arm
else ifeq ($(BR2_aarch64),y)
MUPEN64PLUS_VIDEO_GLIDE64MK2_CONF += HOST_CPU=arm64
endif

ifeq ($(BR2_PACKAGE_HAS_LIBGLES)$(BR2_PACKAGE_SDL2_OPENGLES),yy)
MUPEN64PLUS_VIDEO_GLIDE64MK2_DEPENDENCIES += libgles
MUPEN64PLUS_VIDEO_GLIDE64MK2_CONF += USE_GLES=1
endif

define MUPEN64PLUS_VIDEO_GLIDE64MK2_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		PKG_CONFIG="$(PKG_CONFIG_HOST_BINARY)" \
		PREFIX=/usr/ APIDIR="$(STAGING_DIR)/usr/include/mupen64plus" \
		$(MAKE) -C $(@D)/projects/unix all \
		CC="$(TARGET_CC)" CXX="$(TARGET_CXX)" LD="$(TARGET_CC)" \
		RANLIB="$(TARGET_RANLIB)" AR="$(TARGET_AR)" \
		SDL_CONFIG="$(STAGING_DIR)/usr/bin/sdl2-config" \
		$(MUPEN64PLUS_VIDEO_GLIDE64MK2_CONF)
endef

define MUPEN64PLUS_VIDEO_GLIDE64MK2_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/lib/mupen64plus
	$(INSTALL) -m 0644 $(@D)/projects/unix/mupen64plus-video-glide64mk2.so \
		$(TARGET_DIR)/usr/lib/mupen64plus

	mkdir -p $(TARGET_DIR)/usr/share/mupen64plus
	$(INSTALL) -m 0644 $(@D)/data/Glide64mk2.ini \
		$(TARGET_DIR)/usr/share/mupen64plus
endef

$(eval $(generic-package))
