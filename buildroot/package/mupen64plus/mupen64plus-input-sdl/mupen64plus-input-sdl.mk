################################################################################
#
# mupen64plus-input-sdl
#
################################################################################
MUPEN64PLUS_INPUT_SDL_VERSION = f2ca3839415d45a547f79d21177dfe15a0ce6d8c
MUPEN64PLUS_INPUT_SDL_SITE = $(call github,mupen64plus,mupen64plus-input-sdl,$(MUPEN64PLUS_INPUT_SDL_VERSION))

MUPEN64PLUS_INPUT_SDL_DEPENDENCIES += \
	mupen64plus-core \
	sdl2

MUPEN64PLUS_INPUT_SDL_CONF += UNAME=linux

ifeq ($(BR2_arm),y)
MUPEN64PLUS_INPUT_SDL_CONF += HOST_CPU=arm
else ifeq ($(BR2_aarch64),y)
MUPEN64PLUS_INPUT_SDL_CONF += HOST_CPU=arm64
endif

define MUPEN64PLUS_INPUT_SDL_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		PREFIX=/usr/ APIDIR="$(STAGING_DIR)/usr/include/mupen64plus" \
		$(MAKE) -C $(@D)/projects/unix all \
		CC="$(TARGET_CC)" CXX="$(TARGET_CXX)" LD="$(TARGET_CC)" \
		RANLIB="$(TARGET_RANLIB)" AR="$(TARGET_AR)" \
		SDL_CONFIG="$(STAGING_DIR)/usr/bin/sdl2-config" \
		$(MUPEN64PLUS_INPUT_SDL_CONF)
endef

define MUPEN64PLUS_INPUT_SDL_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/lib/mupen64plus
	$(INSTALL) -m 0644 $(@D)/projects/unix/mupen64plus-input-sdl.so \
		$(TARGET_DIR)/usr/lib/mupen64plus

	mkdir -p $(TARGET_DIR)/usr/share/mupen64plus
	$(INSTALL) -m 0644 $(@D)/data/InputAutoCfg.ini \
		$(TARGET_DIR)/usr/share/mupen64plus
endef

$(eval $(generic-package))
