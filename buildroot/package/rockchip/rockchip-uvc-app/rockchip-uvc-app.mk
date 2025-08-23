################################################################################
#
# rockchip-uvc-app project
#
################################################################################

ROCKCHIP_UVC_APP_SITE = $(TOPDIR)/../external/uvc_app

ROCKCHIP_UVC_APP_SITE_METHOD = local

ROCKCHIP_UVC_APP_INSTALL_STAGING = YES

ROCKCHIP_UVC_APP_DEPENDENCIES = libdrm

ROCKCHIP_UVC_APP_LICENSE = ROCKCHIP
ROCKCHIP_UVC_APP_LICENSE_FILES = LICENSE

ifeq ($(BR2_PACKAGE_RK3588),y)
ROCKCHIP_UVC_APP_CONF_OPTS += "-DRK3588=ON"
else ifeq ($(BR2_PACKAGE_RK3576),y)
ROCKCHIP_UVC_APP_CONF_OPTS += "-DRK3576=ON"
endif

$(eval $(cmake-package))
