################################################################################
#
# LVGL
#
################################################################################

ifeq ($(BR2_PACKAGE_LVGL_VERSION_8), y)
include $(pkgdir)/8.4.0.inc
else ifeq ($(BR2_PACKAGE_LVGL_VERSION_9), y)
include $(pkgdir)/9.1.inc
endif

