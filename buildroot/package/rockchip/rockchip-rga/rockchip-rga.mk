################################################################################
#
# rockchip-rga
#
################################################################################

ROCKCHIP_RGA_SITE = $(TOPDIR)/../external/linux-rga
ROCKCHIP_RGA_VERSION = master
ROCKCHIP_RGA_SITE_METHOD = local

ROCKCHIP_RGA_LICENSE = Apache-2.0
ROCKCHIP_RGA_LICENSE_FILES = COPYING

ROCKCHIP_RGA_INSTALL_STAGING = YES

# Avoid conflict with GCC's fixincl
define ROCKCHIP_RGA_FIX_INCLUDE
	$(SED) 's/ linux / __linux__ /' $(@D)/include/RgaApi.h \
		2>/dev/null || true
endef
ROCKCHIP_RGA_POST_RSYNC_HOOKS += ROCKCHIP_RGA_FIX_INCLUDE

ifeq ($(BR2_PACKAGE_LIBDRM),y)
ROCKCHIP_RGA_DEPENDENCIES += libdrm
ROCKCHIP_RGA_CONF_OPTS += -Dlibdrm=true
endif

$(eval $(meson-package))
