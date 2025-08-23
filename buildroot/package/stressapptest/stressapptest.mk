################################################################################
#
# stressapptest
#
################################################################################

STRESSAPPTEST_SITE = https://github.com/stressapptest/stressapptest.git
STRESSAPPTEST_SITE_METHOD = git
STRESSAPPTEST_VERSION = 6714c57d0d67f5a2a7a9987791af6729289bf64e

STRESSAPPTEST_LICENSE = Apache-2.0
STRESSAPPTEST_LICENSE_FILES = NOTICE

ifeq ($(BR2_PACKAGE_STRESSAPPTEST_STATIC),y)
STRESSAPPTEST_CONF_OPTS += --enable-static
STRESSAPPTEST_CONF_ENV += LDFLAGS="$(TARGET_LDFLAGS) -static"
endif

$(eval $(autotools-package))
