################################################################################
#
# rockit-uvc project
#
################################################################################

ROCKIT_UVC_SITE = $(TOPDIR)/../external/rockit_uvc

ROCKIT_UVC_SITE_METHOD = local

ROCKIT_UVC_INSTALL_STAGING = YES

ROCKIT_UVC_LICENSE = ROCKCHIP
ROCKIT_UVC_LICENSE_FILES = LICENSE

ROCKIT_UVC_DEPENDENCIES = rockit rockchip-uvc-app

$(eval $(cmake-package))
