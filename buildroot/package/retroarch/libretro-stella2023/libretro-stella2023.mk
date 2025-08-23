################################################################################
#
# libretro-stella2023
#
################################################################################
LIBRETRO_STELLA2023_VERSION = ca34413ccc6bd684013f2befff3d108ca1e51821
LIBRETRO_STELLA2023_SITE = $(call github,libretro,stella2023,$(LIBRETRO_STELLA2023_VERSION))

define LIBRETRO_STELLA2023_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" \
	       LDFLAGS="$(TARGET_LDFLAGS) -lstdc++" \
	       $(MAKE) -C $(@D)/src/os/libretro/ \
	       CC="$(TARGET_CC)" CXX="$(TARGET_CXX)" LD="$(TARGET_CC)" \
	       RANLIB="$(TARGET_RANLIB)" AR="$(TARGET_AR)" \
	       platform="unix"
endef

define LIBRETRO_STELLA2023_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/src/os/libretro/stella2023_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/stella2023_libretro.so
endef

$(eval $(generic-package))
