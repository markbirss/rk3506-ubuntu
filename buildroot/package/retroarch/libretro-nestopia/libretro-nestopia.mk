################################################################################
#
# NESTOPIA
#
################################################################################
LIBRETRO_NESTOPIA_VERSION = a9ee6ca84f04990e209880fe47144e62b14253db
LIBRETRO_NESTOPIA_SITE = $(call github,libretro,nestopia,$(LIBRETRO_NESTOPIA_VERSION))

LIBRETRO_NESTOPIA_PLATFORM=$(LIBRETRO_PLATFORM)

# Reusing RPI configs
ifeq ($(BR2_arm),y)
	LIBRETRO_NESTOPIA_PLATFORM += rpi
endif
ifeq ($(BR2_ARM_CPU_ARMV6),y)
	LIBRETRO_NESTOPIA_PLATFORM += rpi1
else ifeq ($(BR2_ARM_CPU_ARMV7A),y)
	LIBRETRO_NESTOPIA_PLATFORM += rpi2
else ifeq ($(BR2_ARM_CPU_ARMV8A),y)
	LIBRETRO_NESTOPIA_PLATFORM += rpi3
endif

define LIBRETRO_NESTOPIA_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" \
	       LDFLAGS="$(TARGET_LDFLAGS) -lstdc++ -lm" \
	       $(MAKE) -C $(@D)/libretro \
	       CC="$(TARGET_CC)" CXX="$(TARGET_CXX)" LD="$(TARGET_CC)" \
	       RANLIB="$(TARGET_RANLIB)" AR="$(TARGET_AR)" \
	       platform="$(LIBRETRO_NESTOPIA_PLATFORM)"
endef

define LIBRETRO_NESTOPIA_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/libretro/nestopia_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/nestopia_libretro.so
endef

$(eval $(generic-package))
