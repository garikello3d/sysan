LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := sysanal_console
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../utility.cpp main.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../ $(LOCAL_PATH)/../../
LOCAL_SHARED_LIBRARIES := libsysanal

include $(BUILD_EXECUTABLE)

$(call import-add-path, $(LOCAL_PATH)/../../../)
$(call import-module, build/android/jni)


