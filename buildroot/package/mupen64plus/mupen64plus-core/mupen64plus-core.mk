################################################################################
#
# mupen64plus-core
#
################################################################################
MUPEN64PLUS_CORE_VERSION = 312a5befde1b44db8beee7868b929c23d896991f
MUPEN64PLUS_CORE_SITE = $(call github,mupen64plus,mupen64plus-core,$(MUPEN64PLUS_CORE_VERSION))
MUPEN64PLUS_CORE_INSTALL_STAGING = YES

MUPEN64PLUS_CORE_DEPENDENCIES += \
	host-nasm host-pkgconf \
	sdl2 \
	libpng \
	libzlib

MUPEN64PLUS_CORE_CONF += UNAME=linux VULKAN=0 OSD=0

ifeq ($(BR2_arm),y)
MUPEN64PLUS_CORE_CONF += HOST_CPU=arm
else ifeq ($(BR2_aarch64),y)
MUPEN64PLUS_CORE_CONF += HOST_CPU=arm64
endif

ifeq ($(BR2_PACKAGE_HAS_LIBGLES)$(BR2_PACKAGE_SDL2_OPENGLES),yy)
MUPEN64PLUS_CORE_DEPENDENCIES += libgles
MUPEN64PLUS_CORE_CONF += USE_GLES=1
endif

ifeq ($(BR2_arm),y)
ifeq ($(BR2_GCC_TARGET_FLOAT_ABI),"hard")
MUPEN64PLUS_CORE_CONF += VFP_HARD=1
endif

ifeq ($(BR2_ARM_CPU_HAS_NEON),y)
LIBRETRO_MUPEN64PLUS_NX_CONF += HAVE_NEON=1
endif
endif

define MUPEN64PLUS_CORE_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		PKG_CONFIG="$(PKG_CONFIG_HOST_BINARY)" \
		$(MAKE) -C $(@D)/projects/unix all \
		CC="$(TARGET_CC)" CXX="$(TARGET_CXX)" LD="$(TARGET_CC)" \
		RANLIB="$(TARGET_RANLIB)" AR="$(TARGET_AR)" \
		SDL_CONFIG="$(STAGING_DIR)/usr/bin/sdl2-config" \
		$(MUPEN64PLUS_CORE_CONF)
endef

define MUPEN64PLUS_CORE_INSTALL_STAGING_CMDS
	mkdir -p $(STAGING_DIR)/usr/include/mupen64plus
	$(INSTALL) -m 0644 $(@D)/src/api/m64p_*.h \
		$(STAGING_DIR)/usr/include/mupen64plus
endef

define MUPEN64PLUS_CORE_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/lib/
	$(INSTALL) -m 0644 $(@D)/projects/unix/libmupen64plus.so.2 \
		$(TARGET_DIR)/usr/lib/

	mkdir -p $(TARGET_DIR)/usr/share/mupen64plus
	$(INSTALL) -m 0644 $(@D)/data/* $(TARGET_DIR)/usr/share/mupen64plus/
endef

$(eval $(generic-package))
