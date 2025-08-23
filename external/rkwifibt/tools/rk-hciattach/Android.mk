LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

ifeq (1,$(strip $(shell expr $(PLATFORM_VERSION) \>= 5.0)))
LOCAL_CFLAGS += -DMMAP64
endif

bluedroidhci_conlyflags := -std=c99
bluedroidhci_cflags := -v -g -Wall -Wno-unused-function -Wno-unused-variable -Wno-unused-parameter

LOCAL_SHARED_LIBRARIES := \
         libcutils \
         liblog
LOCAL_LDLIBS :=-llog
LOCAL_SYSTEM_SHARED_LIBRARIES := libc
LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE_TAGS := debug
LOCAL_SRC_FILES := hciclient.c
LOCAL_CFLAGS := $(bluedroidhci_cflags)
LOCAL_CFLAGS += -DANDROID
LOCAL_CONLYFLAGS := $(bluedroidhci_conlyflags)
LOCAL_MODULE := bluedroidhci

include $(BUILD_EXECUTABLE)

###############################################
include $(CLEAR_VARS)

ifeq (1,$(strip $(shell expr $(PLATFORM_VERSION) \>= 5.0)))
LOCAL_CFLAGS += -DMMAP64
endif

rk_hciattach_conlyflags := -std=c99
rk_hciattach_cflags := -v -g -Wall -Wno-unused-function -Wno-unused-variable -Wno-unused-parameter

LOCAL_SHARED_LIBRARIES := \
         libcutils \
         liblog
LOCAL_LDLIBS :=-llog
LOCAL_SYSTEM_SHARED_LIBRARIES := libc
LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE_TAGS := debug
LOCAL_SRC_FILES := hciattach.c hciattach_rk.c
LOCAL_CFLAGS := $(rk_hciattach_cflags)
LOCAL_CFLAGS += -DANDROID
LOCAL_CONLYFLAGS := $(rk_hciattach_conlyflags)
LOCAL_MODULE := rk_hciattach

include $(BUILD_EXECUTABLE)

###############################################
include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
		tools/hcitool.c \
		lib/hci.c \
		lib/bluetooth.c \
		src/oui.c

LOCAL_C_INCLUDES += \
		$(LOCAL_PATH)/lib \
		$(LOCAL_PATH)/src
	
rk_hcitool_conlyflags = -std=c99
rk_hcitool_cflags = -v -g -Wall -Wno-unused-function -Wno-unused-variable -Wno-unused-parameter

#LOCAL_SYSTEM_SHARED_LIBRARIES := libc
LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE_TAGS := debug
LOCAL_CFLAGS := $(rk_hcitool_cflags)
LOCAL_CFLAGS += -DANDROID -DHAVE_CONFIG_H
LOCAL_CONLYFLAGS := $(rk_hcitool_conlyflags)
LOCAL_MODULE := hcitool

include $(BUILD_EXECUTABLE)

###############################################
include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
		tools/hciconfig.c \
		tools/csr.c \
		lib/hci.c \
		lib/bluetooth.c \
		src/textfile.c \
		src/shared/util.c


LOCAL_C_INCLUDES += \
		$(LOCAL_PATH)/tools \
		$(LOCAL_PATH)/lib \
		$(LOCAL_PATH)/src \
		$(LOCAL_PATH)/src/shared	
	
rk_hciconfig_conlyflags = -std=c99
rk_hciconfig_cflags = -v -g -Wall -Wno-unused-function -Wno-unused-variable -Wno-unused-parameter

#LOCAL_SYSTEM_SHARED_LIBRARIES := libc
LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE_TAGS := debug
LOCAL_CFLAGS := $(rk_hciconfig_cflags)
LOCAL_CFLAGS += -DANDROID -DHAVE_CONFIG_H
LOCAL_CONLYFLAGS := $(rk_hciconfig_conlyflags)
LOCAL_MODULE := hciconfig

include $(BUILD_EXECUTABLE)

###############################################
include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
		tools/hcidump.c \
		tools/parser/l2cap.c \
		tools/parser/rfcomm.c \
		tools/parser/sdp.c \
		tools/parser/parser.c \
		tools/parser/hci.c \
		tools/parser/amp.c \
		tools/parser/csr.c \
		tools/parser/bpa.c \
		tools/parser/smp.c \
		tools/parser/hidp.c \
		tools/parser/avctp.c \
		tools/parser/cmtp.c \
		tools/parser/bnep.c \
		tools/parser/avdtp.c \
		tools/parser/att.c \
		tools/parser/obex.c \
		tools/parser/ppp.c \
		tools/parser/sap.c \
		tools/parser/lmp.c \
		tools/parser/avrcp.c \
		tools/parser/ericsson.c \
		tools/parser/capi.c \
		tools/parser/hcrp.c \
		lib/hci.c \
		lib/bluetooth.c
#		tools/parser/tcpip.c \

LOCAL_C_INCLUDES += \
		$(LOCAL_PATH)/tools \
		$(LOCAL_PATH)/tools/parser \
		$(LOCAL_PATH)/lib

rk_hcidump_conlyflags = -std=c99
rk_hcidump_cflags = -v -g -Wall -Wno-unused-function -Wno-unused-variable -Wno-unused-parameter

LOCAL_SYSTEM_SHARED_LIBRARIES := libc
LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE_TAGS := debug
LOCAL_CFLAGS := $(rk_hcidump_cflags)
LOCAL_CFLAGS += -DANDROID -DHAVE_CONFIG_H
LOCAL_CONLYFLAGS := $(rk_hcidump_conlyflags)
LOCAL_MODULE := hcidump

include $(BUILD_EXECUTABLE)
