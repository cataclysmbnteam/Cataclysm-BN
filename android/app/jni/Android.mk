# Reference: https://developer.android.com/ndk/guides/android_mk.html

APP_LDFLAGS += -fuse-ld=gold
ANDROID_LD=deprecated
include $(call all-subdir-makefiles)
