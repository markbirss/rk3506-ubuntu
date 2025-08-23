################################################################################
#
# pcre2
#
################################################################################

PCRE2_VERSION = 10.42
PCRE2_SITE = https://github.com/PCRE2Project/pcre2/releases/download/pcre2-$(PCRE2_VERSION)
PCRE2_SOURCE = pcre2-$(PCRE2_VERSION).tar.bz2
PCRE2_LICENSE = BSD-3-Clause
PCRE2_LICENSE_FILES = LICENCE
PCRE2_CPE_ID_VENDOR = pcre
PCRE2_INSTALL_STAGING = YES
PCRE2_CONFIG_SCRIPTS = pcre2-config

PCRE2_CONF_OPTS += --enable-pcre2-8
PCRE2_CONF_OPTS += $(if $(BR2_PACKAGE_PCRE2_16),--enable-pcre2-16,--disable-pcre2-16)
PCRE2_CONF_OPTS += $(if $(BR2_PACKAGE_PCRE2_32),--enable-pcre2-32,--disable-pcre2-32)

ifeq ($(BR2_PACKAGE_PCRE2_JIT),y)
PCRE2_CONF_OPTS += --enable-jit
else
PCRE2_CONF_OPTS += --disable-jit
endif

# disable fork usage if not available
ifeq ($(BR2_USE_MMU),)
PCRE2_CONF_OPTS += --disable-pcre2grep-callout
endif

# needed for qt6base
HOST_PCRE2_CONF_OPTS = --enable-pcre2-16

ifeq ($(BR2_PACKAGE_PCRE2_STATIC),y)
PCRE2_CONF_OPTS += --enable-static
endif

define PCRE2_TARGET_INSTALL_REMOVE_TOOLS
	rm -f $(TARGET_DIR)/usr/bin/pcre2*
endef
PCRE2_POST_INSTALL_TARGET_HOOKS += PCRE2_TARGET_INSTALL_REMOVE_TOOLS

$(eval $(autotools-package))
$(eval $(host-autotools-package))
