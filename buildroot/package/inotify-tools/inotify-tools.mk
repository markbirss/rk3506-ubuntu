################################################################################
#
# inotify-tools
#
################################################################################

INOTIFY_TOOLS_VERSION = 3.20.2.2
INOTIFY_TOOLS_SITE = https://github.com/inotify-tools/inotify-tools/releases/download/$(INOTIFY_TOOLS_VERSION)
INOTIFY_TOOLS_LICENSE = GPL-2.0+
INOTIFY_TOOLS_LICENSE_FILES = COPYING
INOTIFY_TOOLS_INSTALL_STAGING = YES

ifeq ($(BR2_PACKAGE_INOTIFY_TOOLS_STATIC),y)
INOTIFY_TOOLS_CONF_OPTS += --enable-static --enable-static-binary
INOTIFY_TOOLS_CONF_ENV += LDFLAGS="$(TARGET_LDFLAGS) -static"
endif

$(eval $(autotools-package))
