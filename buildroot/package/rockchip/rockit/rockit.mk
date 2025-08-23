################################################################################
#
# rockit project
#
################################################################################

ROCKIT_SITE = $(TOPDIR)/../external/rockit

ROCKIT_SITE_METHOD = local

ROCKIT_INSTALL_STAGING = YES

ifneq ($(BR2_PACKAGE_RK3308),)
ROCKIT_CONF_OPTS += -DRK3308=TRUE
ROCKIT_DEPENDENCIES = alsa-lib

else ifneq ($(BR2_PACKAGE_RK3506),)
ROCKIT_CONF_OPTS += -DRK3506=TRUE
ROCKIT_DEPENDENCIES = rockchip-rga alsa-lib

else
ROCKIT_DEPENDENCIES = rockchip-mpp rockchip-rga alsa-lib
endif

$(eval $(cmake-package))
