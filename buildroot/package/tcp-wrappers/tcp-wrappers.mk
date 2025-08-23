################################################################################
#
# tcp-wrapper
#
################################################################################

TCP_WRAPPERS_VERSION = 7.6.q
TCP_WRAPPERS_SOURCE = tcp-wrappers_$(TCP_WRAPPERS_VERSION).orig.tar.gz
TCP_WRAPPERS_PATCH = tcp-wrappers_7.6.q-32.debian.tar.xz
TCP_WRAPPERS_SITE = https://snapshot.debian.org/archive/debian/20240324T210425Z/pool/main/t/tcp-wrappers
TCP_WRAPPERS_LICENSE = tcp-wrappers-license
TCP_WRAPPERS_LICENSE_FILES = DISCLAIMER
TCP_WRAPPERS_INSTALL_STAGING = YES

define TCP_WRAPPERS_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) CC=$(TARGET_CC) \
		COPTS="-DUSE_GETDOMAIN" -C $(@D) gnu
endef

define TCP_WRAPPERS_INSTALL_STAGING_CMDS
        $(INSTALL) -D -m 0644 $(@D)/tcpd.h $(STAGING_DIR)/usr/include/
        $(INSTALL) -D -m 0644 $(@D)/libwrap.a $(STAGING_DIR)/usr/lib/
        $(INSTALL) -D -m 0644 $(@D)/shared/libwrap.so* $(STAGING_DIR)/usr/lib/
endef

define TCP_WRAPPERS_INSTALL_TARGET_CMDS
        $(INSTALL) -D -m 0644 $(@D)/shared/libwrap.so* $(STAGING_DIR)/usr/lib/
        $(INSTALL) -D -m 0644 $(@D)/safe_finger $(STAGING_DIR)/usr/sbin/
        $(INSTALL) -D -m 0644 $(@D)/tcpd $(STAGING_DIR)/usr/sbin/
        $(INSTALL) -D -m 0644 $(@D)/tcpdchk $(STAGING_DIR)/usr/sbin/
        $(INSTALL) -D -m 0644 $(@D)/tcpdmatch $(STAGING_DIR)/usr/sbin/
        $(INSTALL) -D -m 0644 $(@D)/try-from $(STAGING_DIR)/usr/sbin/
endef

$(eval $(generic-package))
