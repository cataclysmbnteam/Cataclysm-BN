LOCAL_PATH := $(call my-dir)

###########################
#
# SDL prebuilt shared library
#
###########################

include $(CLEAR_VARS)

LOCAL_MODULE := SDL2

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libSDL2.so

include $(PREBUILT_SHARED_LIBRARY)

