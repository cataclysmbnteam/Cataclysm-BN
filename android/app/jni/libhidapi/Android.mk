LOCAL_PATH := $(call my-dir)

###########################
#
# libhidapi prebuilt shared library
#
###########################

include $(CLEAR_VARS)

LOCAL_MODULE := libhidapi

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libhidapi.so

include $(PREBUILT_SHARED_LIBRARY)

