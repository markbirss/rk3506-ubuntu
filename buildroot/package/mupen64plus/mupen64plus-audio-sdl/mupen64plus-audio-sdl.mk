################################################################################
#
# mupen64plus-audio-sdl
#
################################################################################
MUPEN64PLUS_AUDIO_SDL_VERSION = 6c2c3f8ae10b7f0f6dfe06f45ca7ca598a6b659a
MUPEN64PLUS_AUDIO_SDL_SITE = $(call github,mupen64plus,mupen64plus-audio-sdl,$(MUPEN64PLUS_AUDIO_SDL_VERSION))

MUPEN64PLUS_AUDIO_SDL_DEPENDENCIES += \
	host-pkgconf \
	mupen64plus-core \
	sdl2

MUPEN64PLUS_AUDIO_SDL_CONF += UNAME=linux NO_OSS=1

ifeq ($(BR2_arm),y)
MUPEN64PLUS_AUDIO_SDL_CONF += HOST_CPU=arm
else ifeq ($(BR2_aarch64),y)
MUPEN64PLUS_AUDIO_SDL_CONF += HOST_CPU=arm64
endif

ifeq ($(BR2_PACKAGE_LIBSAMPLERATE),y)
MUPEN64PLUS_AUDIO_SDL_DEPENDENCIES += libsamplerate
endif

ifeq ($(BR2_PACKAGE_SPEEX),y)
MUPEN64PLUS_AUDIO_SDL_DEPENDENCIES += speex
endif

define MUPEN64PLUS_AUDIO_SDL_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
                PKG_CONFIG="$(PKG_CONFIG_HOST_BINARY)" \
		PREFIX=/usr/ APIDIR="$(STAGING_DIR)/usr/include/mupen64plus" \
		$(MAKE) -C $(@D)/projects/unix all \
		CC="$(TARGET_CC)" CXX="$(TARGET_CXX)" LD="$(TARGET_CC)" \
		RANLIB="$(TARGET_RANLIB)" AR="$(TARGET_AR)" \
		SDL_CONFIG="$(STAGING_DIR)/usr/bin/sdl2-config" \
		$(MUPEN64PLUS_AUDIO_SDL_CONF)
endef

define MUPEN64PLUS_AUDIO_SDL_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/lib/mupen64plus
	$(INSTALL) -m 0644 $(@D)/projects/unix/mupen64plus-audio-sdl.so \
		$(TARGET_DIR)/usr/lib/mupen64plus
endef

$(eval $(generic-package))
