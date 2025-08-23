AVS_SITE = $(TOPDIR)/../external/avs
AVS_SITE_METHOD = local

AVS_INSTALL_STAGING = YES

AVS_CONF_OPTS += -DCONFIG_RK_AVS=ON

define AVS_INSTALL_TARGET_CMDS
	cp -rfp $(@D)/lib/*.so $(TARGET_DIR)/usr/lib/
	cp -rfp $(@D)/lib/*.so $(HOST_DIR)/aarch64-buildroot-linux-gnu/sysroot/usr/lib/
	cp -rfp $(@D)/lib/*.h $(HOST_DIR)/aarch64-buildroot-linux-gnu/sysroot/usr/include/
	cp -rfp $(@D)/avs_calib/ $(TARGET_DIR)/usr/share/
endef

$(eval $(generic-package))
