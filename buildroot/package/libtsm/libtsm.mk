################################################################################
#
# libtsm
#
################################################################################

LIBTSM_VERSION = 4.0.2
LIBTSM_SITE = $(call github,Aetf,libtsm,v$(LIBTSM_VERSION))
LIBTSM_INSTALL_STAGING = YES
LIBTSM_SUPPORTS_IN_SOURCE_BUILD = NO
LIBTSM_LICENSE = BSD-2-Clause, MIT, LGPL-2.1+, ISC
LIBTSM_LICENSE_FILES = COPYING LICENSE_htable external/wcwidth/LICENSE.txt

ifeq ($(BR2_PACKAGE_LIBTSM_STATIC),y)
LIBTSM_CONF_OPTS += -DBUILD_SHARED_LIBS=OFF
endif

$(eval $(cmake-package))
