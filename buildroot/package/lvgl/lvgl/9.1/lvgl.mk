################################################################################
#
# LVGL - Light and Versatile Graphics Library
#
################################################################################

LVGL_VERSION = 9.1
LVGL_SITE = $(call github,lvgl,lvgl,release/v$(LVGL_VERSION))
LVGL_INSTALL_STAGING = YES

ifeq ($(BR2_PACKAGE_LVGL_DISABLE_THORVG), y)
LVGL_CONF_OPTS += -DLV_CONF_BUILD_DISABLE_THORVG_INTERNAL=y
endif

ifeq ($(BR2_PACKAGE_LVGL_DISABLE_EXAMPLES), y)
LVGL_CONF_OPTS += -DLV_CONF_BUILD_DISABLE_EXAMPLES=y
endif

ifeq ($(BR2_PACKAGE_LVGL_DISABLE_DEMOS), y)
LVGL_CONF_OPTS += -DLV_CONF_BUILD_DISABLE_DEMOS=y
endif

ifeq ($(LV_USE_FREETYPE), y)
LVGL_DEPENDENCIES += freetype
LVGL_CONF_OPTS += -DLV_USE_FREETYPE=y
endif

ifeq ($(LV_USE_SDL), y)
LVGL_DEPENDENCIES += sdl2
LVGL_CONF_OPTS += -DLV_USE_SDL=y
endif

ifeq ($(LV_USE_DRAW_SDL), y)
LVGL_DEPENDENCIES += sdl2 sdl2_image
LVGL_CONF_OPTS += -DLV_USE_DRAW_SDL=y
endif

define LVGL_PRE_HOOK
	echo "#ifndef LV_CONF_H" > $(LVGL_DIR)/lv_conf.h
	echo "#define LV_CONF_H" >> $(LVGL_DIR)/lv_conf.h
	cat $(BR2_CONFIG) | grep "LVGL\|LV_" | grep -v "#" | grep -v "LV_LOG_LEVEL_" | \
		grep -v "LV_OS_" | grep -v "LV_DRAW_SW_ASM_" | grep -v "LV_STDLIB_" | \
		grep -v "LV_TXT_ENC_" | sed -e "s/BR2_//g" | \
		sed -e "s/LV_FONT_DEFAULT_/CONFIG_LV_FONT_DEFAULT_/g" | \
		sed -e "s/=y/ 1/g" | sed -e "s/=/ /g" | sed -e "s/^/#define /" >> \
		$(LVGL_DIR)/lv_conf.h
	cat $(BR2_CONFIG) | grep "LVGL\|LV_" | grep "is not set" | grep -v "LV_LOG_LEVEL_" | \
		grep -v "LV_OS_" | grep -v "LV_DRAW_SW_ASM_" | grep -v "LV_STDLIB_" | \
		grep -v "LV_STDLIB_" | grep -v "LV_FONT_DEFAULT_" | \
		grep -v "LV_TXT_ENC_" | \
		sed -e "s/# BR2_/#define /g" | sed -e "s/ is not set/ 0/g" >> \
		$(LVGL_DIR)/lv_conf.h
	echo "#endif" >> $(LVGL_DIR)/lv_conf.h
endef
LVGL_PRE_CONFIGURE_HOOKS += LVGL_PRE_HOOK

$(eval $(cmake-package))
