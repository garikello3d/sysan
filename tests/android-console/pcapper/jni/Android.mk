LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := pcapper_console
LOCAL_SRC_FILES := main.cpp

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../../../../ \
	$(LOCAL_PATH)/../../../ \
	$(LOCAL_PATH)/../../../../android/libpcap

LOCAL_STATIC_LIBRARIES := libsysanal libpcap
LOCAL_CFLAGS := -std=c++11

include $(BUILD_EXECUTABLE)

$(call import-add-path, $(LOCAL_PATH)/../../../../)
$(call import-module, build/android/jni)
$(call import-module, android/libpcap/jni)
