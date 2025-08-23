SOURCE_HAN_SANS_VERSION = 2.004R
SOURCE_HAN_SANS_SITE = https://github.com/adobe-fonts/source-han-sans/releases/download/$(SOURCE_HAN_SANS_VERSION)

include $(sort $(wildcard package/source-han-sans/*/*.mk))
