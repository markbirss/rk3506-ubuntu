################################################################################
#
# play
#
################################################################################
PLAY_VERSION = a045334a966799990e42f579e3fc5dff39ef4617
PLAY_SITE = https://github.com/jpd002/Play-.git
PLAY_SITE_METHOD = git
PLAY_GIT_SUBMODULES = yes

PLAY_DEPENDENCIES += openal libgles qt5base

define PLAY_POST_TARGET_INSTALL
	find $(@D) -type f -name '*.so' \
		-exec $(INSTALL) -D -m 0755 {} $(TARGET_DIR)/usr/lib/ \;
endef
PLAY_POST_INSTALL_TARGET_HOOKS += PLAY_POST_TARGET_INSTALL

$(eval $(cmake-package))
