LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := sysanal
LOCAL_SRC_FILES := ../../utility.cpp
LOCAL_C_INCLUDES := ../../
LOCAL_SHARED_LIBRARIES := libsysanal

include $(BUILD_EXECUTABLE)

$(call import-add-path, ../../../../)
$(call import-module, sysanal/build/android/jni)


