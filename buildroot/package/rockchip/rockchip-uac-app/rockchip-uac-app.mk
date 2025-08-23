################################################################################
#
# rockchip_uac_app project
#
################################################################################

ROCKCHIP_UAC_APP_SITE = $(TOPDIR)/../external/uac_app/uac_app

ROCKCHIP_UAC_APP_SITE_METHOD = local

ROCKCHIP_UAC_APP_INSTALL_STAGING = YES

ROCKCHIP_UAC_APP_LICENSE = ROCKCHIP
ROCKCHIP_UAC_APP_LICENSE_FILES = LICENSE

ROCKCHIP_UAC_APP_DEPENDENCIES += rockit

ROCKCHIP_UAC_APP_CONF_OPTS += "-DUAC_BUILDROOT=ON"

$(eval $(cmake-package))
