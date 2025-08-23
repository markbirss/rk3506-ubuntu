################################################################################
#
# mupen64plus-rsp-parallel
#
################################################################################
MUPEN64PLUS_RSP_PARALLEL_VERSION = 4cf680bbbda082b32723ef9f8da6a8120803c19c
MUPEN64PLUS_RSP_PARALLEL_SITE = $(call github,Mastergatto,parallel-rsp,$(MUPEN64PLUS_RSP_PARALLEL_VERSION))

MUPEN64PLUS_RSP_PARALLEL_DEPENDENCIES += \
	mupen64plus-core

MUPEN64PLUS_RSP_PARALLEL_CONF_OPTS += \
	-DAPIDIR=$(STAGING_DIR)/usr/include/mupen64plus

define MUPEN64PLUS_RSP_PARALLEL_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/lib/mupen64plus
        $(INSTALL) -m 0644 $(@D)/mupen64plus-rsp-parallel.so \
                $(TARGET_DIR)/usr/lib/mupen64plus
endef

$(eval $(cmake-package))
