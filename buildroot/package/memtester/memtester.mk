################################################################################
#
# memtester
#
################################################################################

MEMTESTER_VERSION = 4.5.1
MEMTESTER_SITE = http://pyropus.ca/software/memtester/old-versions
MEMTESTER_LICENSE = GPL-2.0
MEMTESTER_LICENSE_FILES = COPYING
MEMTESTER_CPE_ID_VENDOR = pryopus

MEMTESTER_TARGET_INSTALL_OPTS = INSTALLPATH=$(TARGET_DIR)/usr

MEMTESTER_CFLAGS = $(TARGET_CFLAGS)
MEMTESTER_LDFLAGS = $(TARGET_LDFLAGS)

ifeq ($(BR2_PACKAGE_MEMTESTER_STATIC),y)
MEMTESTER_CFLAGS += -static
MEMTESTER_LDFLAGS += -static
endif

define MEMTESTER_BUILD_CMDS
	$(SED) "s%^cc%$(TARGET_CC) $(MEMTESTER_CFLAGS)%" $(@D)/conf-cc
	$(SED) "s%^cc%$(TARGET_CC) $(MEMTESTER_LDFLAGS)%" $(@D)/conf-ld
	$(MAKE) -C $(@D)
endef

define MEMTESTER_INSTALL_TARGET_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) $(MEMTESTER_TARGET_INSTALL_OPTS) -C $(@D) install
endef

$(eval $(generic-package))
