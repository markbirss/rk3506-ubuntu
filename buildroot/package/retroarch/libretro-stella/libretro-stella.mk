################################################################################
#
# STELLA
#
################################################################################
LIBRETRO_STELLA_VERSION = 3cc89f0d316d6c924a5e3f4011d17421df58e615
LIBRETRO_STELLA_SITE = $(call github,libretro,stella-libretro,$(LIBRETRO_STELLA_VERSION))

define LIBRETRO_STELLA_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" \
	       LDFLAGS="$(TARGET_LDFLAGS) -lstdc++" \
	       $(MAKE) -C $(@D) \
	       CC="$(TARGET_CC)" CXX="$(TARGET_CXX)" LD="$(TARGET_CC) -lm" \
	       RANLIB="$(TARGET_RANLIB)" AR="$(TARGET_AR)" \
	       platform="unix"
endef

define LIBRETRO_STELLA_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/stella2014_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/stella2014_libretro.so
endef

$(eval $(generic-package))
