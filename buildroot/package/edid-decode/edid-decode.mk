################################################################################
#
# edid-decode
#
################################################################################

EDID_DECODE_VERSION = 2d44e1b01c7ed7d65b20ecdce62d354841832201
EDID_DECODE_SITE = https://git.linuxtv.org/edid-decode.git
EDID_DECODE_SITE_METHOD = git
EDID_DECODE_LICENSE = MIT
EDID_DECODE_LICENSE_FILES = LICENSE

EDID_DECODE_LDFLAGS = $(TARGET_LDFLAGS)

ifeq ($(BR2_PACKAGE_EDID_DECODE_STATIC),y)
EDID_DECODE_LDFLAGS += -static
endif

define EDID_DECODE_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_OPTS) $(MAKE) -C $(@D) \
		CFLAGS="$(TARGET_CXXFLAGS) -std=c++11" WARN_FLAGS= \
		LDFLAGS="$(EDID_DECODE_LDFLAGS)"
endef

define EDID_DECODE_INSTALL_TARGET_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_OPTS) $(MAKE) -C $(@D) \
		DESTDIR="$(TARGET_DIR)" install
endef

$(eval $(generic-package))
