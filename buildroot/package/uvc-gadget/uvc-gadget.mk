################################################################################
#
# uvc-gadget
#
################################################################################

UVC_GADGET_VERSION = v0.4.0
UVC_GADGET_SITE = https://gitlab.freedesktop.org/camera/uvc-gadget/-/archive/$(UVC_GADGET_VERSION)
UVC_GADGET_SOURCE = uvc-gadget-$(UVC_GADGET_VERSION).tar.bz2
UVC_GADGET_LICENSE = LGPL 2.1+

UVC_GADGET_DEPENDENCIES = jpeg

UVC_GADGET_STATIC = $(BR2_PACKAGE_UVC_GADGET_STATIC)
ifeq ($(UVC_GADGET_STATIC),y)
UVC_GADGET_CFLAGS += -static
UVC_GADGET_CXXFLAGS += -static
UVC_GADGET_FCFLAGS += -static
UVC_GADGET_LDFLAGS += -static
endif

$(eval $(meson-package))
