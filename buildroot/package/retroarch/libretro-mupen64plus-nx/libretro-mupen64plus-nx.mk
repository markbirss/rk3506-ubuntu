################################################################################
#
# mupen64plus-next
#
################################################################################
LIBRETRO_MUPEN64PLUS_NX_VERSION = c2f6acfe3b7b07ab86c3e4cd89f61a9911191793
LIBRETRO_MUPEN64PLUS_NX_SITE = $(call github,libretro,mupen64plus-libretro-nx,$(LIBRETRO_MUPEN64PLUS_NX_VERSION))

LIBRETRO_MUPEN64PLUS_NX_CONF = platform=unix

ifeq ($(BR2_arm),y)
LIBRETRO_MUPEN64PLUS_NX_CONF += ARCH=arm
else ifeq ($(BR2_aarch64),y)
LIBRETRO_MUPEN64PLUS_NX_CONF += ARCH=aarch64
endif

ifeq ($(BR2_ARM_CPU_HAS_NEON),y)
LIBRETRO_MUPEN64PLUS_NX_CONF += HAVE_NEON=1
endif

ifeq ($(BR2_PACKAGE_HAS_LIBGLES),y)
LIBRETRO_MUPEN64PLUS_NX_DEPENDENCIES += libgles
LIBRETRO_MUPEN64PLUS_NX_CONF += FORCE_GLES=1
endif

define LIBRETRO_MUPEN64PLUS_NX_BUILD_CMDS
		CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" $(MAKE) -C $(@D) \
		CC="$(TARGET_CC)" CXX="$(TARGET_CXX)" LD="$(TARGET_CC)" \
		RANLIB="$(TARGET_RANLIB)" AR="$(TARGET_AR)" \
		$(LIBRETRO_MUPEN64PLUS_NX_CONF)
endef

define LIBRETRO_MUPEN64PLUS_NX_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mupen64plus_next_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mupen64plus_next_libretro.so
endef

$(eval $(generic-package))
