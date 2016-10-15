LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libsysanal

SRC_BASE := ../../../

FILE_LIST := \
	$(wildcard $(LOCAL_PATH)/$(SRC_BASE)/*.cpp) \
	$(wildcard $(LOCAL_PATH)/$(SRC_BASE)/android/*.cpp)

LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../android
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH_)/../../../
LOCAL_CPP_FEATURES := rtti exceptions

#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)
