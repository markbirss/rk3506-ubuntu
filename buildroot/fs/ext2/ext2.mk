################################################################################
#
# Build the ext2 root filesystem image
#
################################################################################

ROOTFS_EXT2_SIZE = $(call qstrip,$(BR2_TARGET_ROOTFS_EXT2_SIZE))

ROOTFS_EXT2_MKFS_OPTS = $(call qstrip,$(BR2_TARGET_ROOTFS_EXT2_MKFS_OPTIONS))

# qstrip results in stripping consecutive spaces into a single one. So the
# variable is not qstrip-ed to preserve the integrity of the string value.
ROOTFS_EXT2_LABEL = $(subst ",,$(BR2_TARGET_ROOTFS_EXT2_LABEL))
#" Syntax highlighting... :-/ )

ROOTFS_EXT2_OPTS = \
	-d $(TARGET_DIR) \
	-r $(BR2_TARGET_ROOTFS_EXT2_REV) \
	-b $(BR2_TARGET_ROOTFS_EXT2_BLKSZ) \
	-N $(BR2_TARGET_ROOTFS_EXT2_INODES) \
	-m $(BR2_TARGET_ROOTFS_EXT2_RESBLKS) \
	-L "$(ROOTFS_EXT2_LABEL)" \
	-I $(BR2_TARGET_ROOTFS_EXT2_INODE_SIZE) \
	$(ROOTFS_EXT2_MKFS_OPTS)

ROOTFS_EXT2_DEPENDENCIES = host-e2fsprogs

ifneq ($(BR2_TARGET_ROOTFS_EXT2_SIZE_AUTO),y)
ifeq ($(BR2_TARGET_ROOTFS_EXT2)-$(ROOTFS_EXT2_SIZE),y-)
$(error BR2_TARGET_ROOTFS_EXT2_SIZE cannot be empty)
endif

define ROOTFS_EXT2_CMD
	rm -f $@
	$(HOST_DIR)/sbin/mkfs.ext$(BR2_TARGET_ROOTFS_EXT2_GEN) $(ROOTFS_EXT2_OPTS) $@ \
		"$(ROOTFS_EXT2_SIZE)" \
	|| { ret=$$?; \
	     echo "*** Maybe you need to increase the filesystem size (BR2_TARGET_ROOTFS_EXT2_SIZE)" 1>&2; \
	     exit $$ret; \
	}
endef
else
define ROOTFS_EXT2_CMD
	rm -f $@
	FILE_SIZE="$$(du --apparent-size -sm $(TARGET_DIR) | cut -f1)"
	ALIGN_SIZE="$$(($$(find $(TARGET_DIR) | wc -l) * \
		   $(BR2_TARGET_ROOTFS_EXT2_BLKSZ) / 1024 / 1024))"
	ROOTFS_SIZE="$$(( ($$FILE_SIZE + $$ALIGN_SIZE) * 110 / 100 + 64 ))"
	$(HOST_DIR)/sbin/mkfs.ext$(BR2_TARGET_ROOTFS_EXT2_GEN) \
		$(ROOTFS_EXT2_OPTS) $@ "$${ROOTFS_SIZE}M"
	$(HOST_DIR)/sbin/resize2fs -M $@
	$(HOST_DIR)/sbin/e2fsck -fy $@
endef
endif

ifneq ($(BR2_TARGET_ROOTFS_EXT2_GEN),2)
define ROOTFS_EXT2_SYMLINK
	ln -sf rootfs.ext2$(ROOTFS_EXT2_COMPRESS_EXT) $(BINARIES_DIR)/rootfs.ext$(BR2_TARGET_ROOTFS_EXT2_GEN)$(ROOTFS_EXT2_COMPRESS_EXT)
endef
ROOTFS_EXT2_POST_GEN_HOOKS += ROOTFS_EXT2_SYMLINK
endif

$(eval $(rootfs))
