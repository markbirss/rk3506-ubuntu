################################################################################
#
# labwc
#
################################################################################

LABWC_VERSION = 0.7.2
LABWC_SITE = https://github.com/labwc/labwc.git
LABWC_SITE_METHOD = git
LABWC_LICENSE = MIT
LABWC_LICENSE_FILES = LICENSE

LABWC_DEPENDENCIES = \
	host-pkgconf wlroots libxml2 cairo pango libpng librsvg xkeyboard-config

LABWC_CONF_OPTS = -Dman-pages=disabled -Dxwayland=disabled

$(eval $(meson-package))
