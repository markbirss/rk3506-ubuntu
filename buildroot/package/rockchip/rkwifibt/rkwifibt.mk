################################################################################
#
# rkwifibt
#
################################################################################

RKWIFIBT_VERSION = 1.0.0
RKWIFIBT_SITE_METHOD = local
RKWIFIBT_SITE = $(TOPDIR)/../external/rkwifibt
RKWIFIBT_LICENSE = ROCKCHIP
RKWIFIBT_LICENSE_FILES = LICENSE

ifeq ($(BR2_PACKAGE_RKWIFIBT_STATIC),y)
RKWIFIBT_CFLAGS = $(TARGET_CFLAGS) -static
RKWIFIBT_LDFLAGS = $(TARGET_LDFLAGS) -static
endif

$(eval $(meson-package))
