################################################################################
#
# aml
#
################################################################################

AML_SITE = https://github.com/any1/aml.git
AML_VERSION = v0.3.0
AML_SITE_METHOD = git
AML_LICENSE = ISC
AML_LICENSE_FILES = COPYING
AML_INSTALL_STAGING = YES

$(eval $(meson-package))
