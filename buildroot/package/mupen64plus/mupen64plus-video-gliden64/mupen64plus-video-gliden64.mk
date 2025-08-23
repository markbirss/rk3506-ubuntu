################################################################################
#
# mupen64plus-video-gliden64
#
################################################################################
MUPEN64PLUS_VIDEO_GLIDEN64_VERSION = f4b0755e2722d59fe1c20d7e470f7b8929dcae85
MUPEN64PLUS_VIDEO_GLIDEN64_SITE = $(call github,gonetz,GLideN64,$(MUPEN64PLUS_VIDEO_GLIDEN64_VERSION))

MUPEN64PLUS_VIDEO_GLIDEN64_SUBDIR = src

MUPEN64PLUS_VIDEO_GLIDEN64_DEPENDENCIES += libpng libzlib

MUPEN64PLUS_VIDEO_GLIDEN64_CONF_OPTS += \
	-DMUPENPLUSAPI=ON \
	-DUSE_SYSTEM_LIBS=ON \
	-DNOHQ=1

ifeq ($(BR2_PACKAGE_HAS_LIBEGL),y)
MUPEN64PLUS_VIDEO_GLIDEN64_DEPENDENCIES += libegl
MUPEN64PLUS_VIDEO_GLIDEN64_CONF_OPTS += -DEGL=ON
endif

$(eval $(cmake-package))
