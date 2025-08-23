################################################################################
#
# Build the ubifs root filesystem image
#
################################################################################

ifeq ($(BR2_TARGET_ROOTFS_UBIFS_MAX_SIZE),0)
UBIFS_MAXLEBCNT = $(BR2_TARGET_ROOTFS_UBIFS_MAXLEBCNT)
else
UBIFS_LEBSIZE = $(shell printf "%d" $(BR2_TARGET_ROOTFS_UBIFS_LEBSIZE))
UBIFS_MAX_SIZE = $(shell echo "$(BR2_TARGET_ROOTFS_UBIFS_MAX_SIZE)*1024" | bc)
UBIFS_MAXLEBCNT = $(shell echo "$(UBIFS_MAX_SIZE)*1024/$(UBIFS_LEBSIZE)" | bc)
endif

UBIFS_OPTS = \
	-e $(BR2_TARGET_ROOTFS_UBIFS_LEBSIZE) \
	-c $(UBIFS_MAXLEBCNT) \
	-m $(BR2_TARGET_ROOTFS_UBIFS_MINIOSIZE)

ifeq ($(BR2_TARGET_ROOTFS_UBIFS_RT_ZLIB),y)
UBIFS_OPTS += -x zlib
endif
ifeq ($(BR2_TARGET_ROOTFS_UBIFS_RT_LZO),y)
UBIFS_OPTS += -x lzo
endif
ifeq ($(BR2_TARGET_ROOTFS_UBIFS_RT_NONE),y)
UBIFS_OPTS += -x none
endif

UBIFS_OPTS += $(call qstrip,$(BR2_TARGET_ROOTFS_UBIFS_OPTS))

ROOTFS_UBIFS_DEPENDENCIES = host-mtd

define ROOTFS_UBIFS_CMD
	$(HOST_DIR)/sbin/mkfs.ubifs -d $(TARGET_DIR) $(UBIFS_OPTS) -o $@
endef

$(eval $(rootfs))
