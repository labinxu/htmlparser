LOCAL_PATHA := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := tinyhtml
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATHA)/../../include
LOCAL_SRC_FILES := $(LOCAL_PATHA)/tinyhtml.cpp
include $(BUILD_STATIC_LIBRARY)