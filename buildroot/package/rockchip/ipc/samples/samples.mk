SAMPLES_SITE = $(TOPDIR)/../external/samples
SAMPLES_SITE_METHOD = local
SAMPLES_LICENSE = ROCKCHIP
SAMPLES_LICENSE_FILES = LICENSE

SAMPLES_DEPENDENCIES = camera-engine-rkaiq wpa_supplicant freetype common_algorithm iva

ifeq ($(BR2_PACKAGE_RK3588),y)
    RK_MEDIA_CHIP=rk3588
endif

ifeq ($(BR2_PACKAGE_RK3576),y)
    RK_MEDIA_CHIP=rk3576
endif

RK_MEDIA_JOBS=65
RK_MEDIA_SAMPLE_STATIC_LINK=n
RK_MEDIA_CROSS=$(patsubst %-gcc, %, $(TARGET_CC))
RK_MEDIA_CROSS_CFLAGS=
RK_MEDIA_OUTPUT= $(STAGING_DIR)/usr/
RK_MEDIA_OPTS=-z noexecstack

export COMPILE_FOR_BUILDROOT=y
export RK_ENABLE_SAMPLE=y
export CONFIG_RK_IVA=y

export RK_MEDIA_JOBS RK_MEDIA_SAMPLE_STATIC_LINK RK_MEDIA_CROSS RK_MEDIA_CHIP RK_MEDIA_OUTPUT RK_MEDIA_OPTS RK_MEDIA_CROSS_CFLAGS CONFIG_RK_IVA

define SAMPLES_CONFIGURE_CMDS
    $(info "configure multi-media samples for $(RK_MEDIA_CHIP)")
endef

define SAMPLES_BUILD_CMDS
    $(info "build multi-media samples for $(RK_MEDIA_CHIP)")
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D)
endef

define SAMPLES_INSTALL_CMDS
    $(info "install multi-media samples for $(RK_MEDIA_CHIP)")
	mkdir -p $(TARGET_DIR)/usr/bin/
	mkdir -p $(TARGET_DIR)/usr/share/samples
	$(INSTALL) -D -m 755 $(@D)/simple_test/out/bin/* $(TARGET_DIR)/usr/bin
	$(INSTALL) -D -m 755 $(@D)/example/out/bin/* $(TARGET_DIR)/usr/bin
	$(INSTALL) -D -m 755 $(@D)/example/out/install_to_userdata/* $(TARGET_DIR)/usr/share/samples
endef

SAMPLES_POST_INSTALL_TARGET_HOOKS += SAMPLES_INSTALL_CMDS

$(eval $(generic-package))