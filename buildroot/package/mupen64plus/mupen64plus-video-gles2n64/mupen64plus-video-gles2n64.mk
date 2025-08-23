################################################################################
#
# mupen64plus-video-gles2n64
#
################################################################################
MUPEN64PLUS_VIDEO_GLES2N64_VERSION = 1f53773f9045f5f18b895fe41f166d272175d72f
MUPEN64PLUS_VIDEO_GLES2N64_SITE = $(call github,ricrpi,mupen64plus-video-gles2n64,$(MUPEN64PLUS_VIDEO_GLES2N64_VERSION))

MUPEN64PLUS_VIDEO_GLES2N64_DEPENDENCIES += \
	mupen64plus-core \
	host-pkgconf \
	sdl2 \
	libpng

MUPEN64PLUS_VIDEO_GLES2N64_CONF += UNAME=linux \
	HASHMAP_OPT=1 VEC4_OPT=1 TRIBUFFER_OPT=1 CRC_OPT=1

ifeq ($(BR2_arm),y)
MUPEN64PLUS_VIDEO_GLES2N64_CONF += HOST_CPU=arm
else ifeq ($(BR2_aarch64),y)
MUPEN64PLUS_VIDEO_GLES2N64_CONF += HOST_CPU=arm64
endif

MUPEN64PLUS_VIDEO_GLES2N64_DEPENDENCIES += libgles
MUPEN64PLUS_VIDEO_GLES2N64_CONF += USE_GLES=1

ifeq ($(BR2_arm),y)
ifeq ($(BR2_GCC_TARGET_FLOAT_ABI),"hard")
MUPEN64PLUS_VIDEO_GLES2N64_CONF += VFP=1
endif

ifeq ($(BR2_ARM_CPU_HAS_NEON),y)
MUPEN64PLUS_VIDEO_GLES2N64_CONF += NEON=1
endif
endif

define MUPEN64PLUS_VIDEO_GLES2N64_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		PKG_CONFIG="$(PKG_CONFIG_HOST_BINARY)" \
		PREFIX=/usr/ APIDIR="$(STAGING_DIR)/usr/include/mupen64plus" \
		$(MAKE) -C $(@D)/projects/unix all \
		CC="$(TARGET_CC)" CXX="$(TARGET_CXX)" LD="$(TARGET_CC)" \
		RANLIB="$(TARGET_RANLIB)" AR="$(TARGET_AR)" \
		SDL_CONFIG="$(STAGING_DIR)/usr/bin/sdl2-config" \
		$(MUPEN64PLUS_VIDEO_GLES2N64_CONF)
endef

define MUPEN64PLUS_VIDEO_GLES2N64_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/lib/mupen64plus
	$(INSTALL) -m 0644 $(@D)/projects/unix/mupen64plus-video-n64.so \
		$(TARGET_DIR)/usr/lib/mupen64plus

	mkdir -p $(TARGET_DIR)/usr/share/mupen64plus
	$(INSTALL) -m 0644 $(@D)/data/* \
		$(TARGET_DIR)/usr/share/mupen64plus
endef

$(eval $(generic-package))
