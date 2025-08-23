################################################################################
#
# rkpartybox
#
################################################################################

RKPARTYBOX_SITE = $(TOPDIR)/../app/rkpartybox
RKPARTYBOX_SITE_METHOD = local

RKPARTYBOX_LICENSE = ROCKCHIP
RKPARTYBOX_LICENSE_FILES = LICENSE

ifeq ($(BR2_PACKAGE_RK3308_RKPARTYBOX_BTMCU),y)
RKPARTYBOX_CONF_OPTS += -DRK3308_PBOX_BTMCU=TRUE
else
RKPARTYBOX_DEPENDENCIES += wpa_supplicant bluez5_utils
endif

ifeq ($(BR2_PACKAGE_PBOX_LCD_DISPLAY),y)
RKPARTYBOX_CONF_OPTS += -DENABLE_LVGL_DISPLAY=1
RKPARTYBOX_DEPENDENCIES += libdrm lvgl lv_drivers
endif

ifeq ($(BR2_PACKAGE_PBOX_LED_EFFECT),y)
RKPARTYBOX_CONF_OPTS += -DENABLE_RK_LED_EFFECT=TRUE
endif

define RKPARTYBOX_POST_INSTALL_TO_TARGET
	$(INSTALL) -D -m 0755 $(@D)/lib64/lib* $(TARGET_DIR)/usr/lib/
endef
#RKPARTYBOX_POST_INSTALL_TARGET_HOOKS += RKPARTYBOX_POST_INSTALL_TO_TARGET
$(info RKPARTYBOX_CONF_OPTS is $(RKPARTYBOX_CONF_OPTS))
$(eval $(cmake-package))
