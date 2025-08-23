################################################################################
#
# mupen64plus
#
################################################################################
LIBRETRO_MUPEN64PLUS_VERSION = ab8134ac90a567581df6de4fc427dd67bfad1b17
LIBRETRO_MUPEN64PLUS_SITE = $(call github,libretro,mupen64plus-libretro,$(LIBRETRO_MUPEN64PLUS_VERSION))

LIBRETRO_MUPEN64PLUS_CONF = platform=unix

ifeq ($(BR2_arm),y)
LIBRETRO_MUPEN64PLUS_CONF += ARCH=arm
else ifeq ($(BR2_aarch64),y)
LIBRETRO_MUPEN64PLUS_CONF += ARCH=aarch64
endif

ifeq ($(BR2_ARM_CPU_HAS_NEON),y)
LIBRETRO_MUPEN64PLUS_CONF += HAVE_NEON=1
endif

ifeq ($(BR2_PACKAGE_HAS_LIBGLES),y)
LIBRETRO_MUPEN64PLUS_DEPENDENCIES += libgles
LIBRETRO_MUPEN64PLUS_CONF += FORCE_GLES=1
endif

define LIBRETRO_MUPEN64PLUS_BUILD_CMDS
		CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" $(MAKE) -C $(@D) \
		CC="$(TARGET_CC)" CXX="$(TARGET_CXX)" LD="$(TARGET_CC)" \
		RANLIB="$(TARGET_RANLIB)" AR="$(TARGET_AR)" \
		$(LIBRETRO_MUPEN64PLUS_CONF)
endef

define LIBRETRO_MUPEN64PLUS_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mupen64plus_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mupen64plus_libretro.so
endef

$(eval $(generic-package))
