################################################################################
#
# rockchip erpc project
#
################################################################################

ROCKCHIP_ERPC_VERSION = rkdev
ROCKCHIP_ERPC_SITE = $(TOPDIR)/../external/rockchip-erpc
ROCKCHIP_ERPC_SITE_METHOD = local
ROCKCHIP_ERPC_INSTALL_STAGING = YES

ROCKCHIP_ERPC_LICENSE = BSD 3 Clause
ROCKCHIP_ERPC_LICENSE_FILES = LICENSE

$(eval $(cmake-package))
