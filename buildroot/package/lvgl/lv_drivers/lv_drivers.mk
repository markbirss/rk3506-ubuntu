################################################################################
#
# lv_drivers
#
################################################################################

LV_DRIVERS_VERSION = 8.3.0
LV_DRIVERS_SITE = $(call github,lvgl,lv_drivers,v$(LV_DRIVERS_VERSION))
LV_DRIVERS_INSTALL_STAGING = YES

LV_DRIVERS_DEPENDENCIES = lvgl

ifeq ($(BR2_LV_DRIVERS_USE_SDL_GPU), y)
LV_DRIVERS_CONF_OPTS += -DLV_DRV_USE_SDL_GPU=1
LV_DRIVERS_DEPENDENCIES += sdl2
endif

ifeq ($(BR2_LV_DRIVERS_USE_OPENGL), y)
LV_DRIVERS_CONF_OPTS += -DLV_DRV_USE_OPENGL=1
endif

ifeq ($(BR2_LV_DRIVERS_USE_DRM), y)
LV_DRIVERS_CONF_OPTS += -DLV_DRV_USE_DRM=1
LV_DRIVERS_DEPENDENCIES += libdrm
endif

ifeq ($(BR2_LV_DRIVERS_USE_RGA), y)
LV_DRIVERS_CONF_OPTS += -DLV_DRV_USE_RGA=1
LV_DRIVERS_DEPENDENCIES += rockchip-rga
endif

ifeq ($(BR2_LV_DRIVERS_USE_RKADK), y)
LV_DRIVERS_CONF_OPTS += -DLV_DRV_USE_RKADK=1
LV_DRIVERS_DEPENDENCIES += rkadk rockchip-rga
endif

$(eval $(cmake-package))
