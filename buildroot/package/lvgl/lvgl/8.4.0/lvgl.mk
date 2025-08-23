################################################################################
#
# LittleVGL
#
################################################################################

LVGL_VERSION = 8.4.0
LVGL_SITE = $(call github,lvgl,lvgl,v$(LVGL_VERSION))
LVGL_INSTALL_STAGING = YES

ifeq ($(BR2_LV_USE_DEMO_WIDGETS), y)
LVGL_CONF_OPTS += -DLV_USE_DEMO_WIDGETS=1
endif

ifeq ($(BR2_LV_USE_DEMO_KEYPAD_AND_ENCODER), y)
LVGL_CONF_OPTS += -DLV_USE_DEMO_KEYPAD_AND_ENCODER=1
endif

ifeq ($(BR2_LV_USE_DEMO_BENCHMARK), y)
LVGL_CONF_OPTS += -DLV_USE_DEMO_BENCHMARK=1
endif

ifeq ($(BR2_LV_USE_DEMO_STRESS), y)
LVGL_CONF_OPTS += -DLV_USE_DEMO_STRESS=1
endif

ifeq ($(BR2_LV_USE_DEMO_MUSIC), y)
LVGL_CONF_OPTS += -DLV_USE_DEMO_MUSIC=1
endif

ifeq ($(BR2_LV_USE_FREETYPE), y)
LVGL_DEPENDENCIES += freetype
endif

ifeq ($(BR2_LV_USE_GPU_SDL), y)
LVGL_CONF_OPTS += -DLV_USE_GPU_SDL=1
LVGL_DEPENDENCIES += sdl2
endif

define LVGL_PRE_HOOK
	echo "#ifndef LV_CONF_H" > $(LVGL_DIR)/lv_conf.h
	echo "#define LV_CONF_H" >> $(LVGL_DIR)/lv_conf.h
	cat $(BR2_CONFIG) | grep "LVGL\|LV_" | grep -v "#" | grep -v "LV_LOG_LEVEL_" | \
		grep -v "LV_STDLIB_" | sed -e "s/BR2_//g" | \
		sed -e "s/LV_FONT_DEFAULT_/CONFIG_LV_FONT_DEFAULT_/g" | \
		sed -e "s/=y/ 1/g" | sed -e "s/=/ /g" | sed -e "s/^/#define /" >> \
		$(LVGL_DIR)/lv_conf.h
	cat $(BR2_CONFIG) | grep "LVGL\|LV_" | grep "is not set" | grep -v "LV_LOG_LEVEL_" | \
		grep -v "LV_STDLIB_" | grep -v "LV_FONT_DEFAULT_" | \
		sed -e "s/# BR2_/#define /g" | sed -e "s/ is not set/ 0/g" >> \
		$(LVGL_DIR)/lv_conf.h
	echo "#endif" >> $(LVGL_DIR)/lv_conf.h
endef
LVGL_PRE_CONFIGURE_HOOKS += LVGL_PRE_HOOK

$(eval $(cmake-package))
