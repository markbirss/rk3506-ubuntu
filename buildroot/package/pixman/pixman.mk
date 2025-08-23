################################################################################
#
# pixman
#
################################################################################

PIXMAN_VERSION = 0.43.4
PIXMAN_SOURCE = pixman-$(PIXMAN_VERSION).tar.xz
PIXMAN_SITE = https://xorg.freedesktop.org/releases/individual/lib
PIXMAN_LICENSE = MIT
PIXMAN_LICENSE_FILES = COPYING
PIXMAN_CPE_ID_VENDOR = pixman

PIXMAN_INSTALL_STAGING = YES
PIXMAN_DEPENDENCIES = host-pkgconf
HOST_PIXMAN_DEPENDENCIES = host-pkgconf

ifeq ($(BR2_PREFER_ROCKCHIP_RGA),y)
PIXMAN_DEPENDENCIES += rockchip-rga
define PIXMAN_INSTALL_TARGET_ENV
	echo "export PIXMAN_USE_RGA=1" > $(@D)/pixman.sh
	$(INSTALL) -D -m 0644 $(@D)/pixman.sh \
		$(TARGET_DIR)/etc/profile.d/pixman.sh
endef

PIXMAN_POST_INSTALL_TARGET_HOOKS += PIXMAN_INSTALL_TARGET_ENV
endif

# don't build demos and tests
PIXMAN_CONF_OPTS = -Ddemos=disabled -Dtests=disabled

# Affects only tests, and we don't build tests (see
# 0001-Disable-tests.patch). See
# https://gitlab.freedesktop.org/pixman/pixman/-/issues/76, which says
# "not sure why NVD keeps assigning CVEs like this. This is just a
# test executable".
PIXMAN_IGNORE_CVES += CVE-2023-37769

PIXMAN_CFLAGS = $(TARGET_CFLAGS)

# toolchain gets confused about TLS access through GOT (PIC), so disable TLS
# movhi	r4, %got_hiadj(%tls_ldo(fast_path_cache))
# {standard input}:172: Error: bad expression
ifeq ($(BR2_TOOLCHAIN_EXTERNAL_CODESOURCERY_NIOSII),y)
PIXMAN_CFLAGS += -DPIXMAN_NO_TLS
endif

ifeq ($(BR2_TOOLCHAIN_HAS_GCC_BUG_101737),y)
PIXMAN_CFLAGS += -O0
endif

$(eval $(meson-package))
$(eval $(host-meson-package))
