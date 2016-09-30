LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := collector_backend
LOCAL_SRC_FILES := main.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../
LOCAL_CFLAGS := -std=c++11
LOCAL_CPP_FEATURES := rtti exceptions
LOCAL_LDLIBS := -llog
LOCAL_SHARED_LIBRARIES := libsysanal

include $(BUILD_EXECUTABLE)

$(call import-add-path, $(LOCAL_PATH)/../../../)
$(call import-module, build/android/jni)
