LOCAL_PATH:=$(call my-dir)

################################ bin #################################
include $(CLEAR_VARS)
LOCAL_MODULE := mem_i_test
TARGET_ARCH_ABI := armeabi-v7a arm64-v8a
LOCAL_SRC_FILES := memcpy.cpp
LOCAL_LDLIBS +=  -lm -llog
LDFLAGS := -nodefaultlibs -S -lc -lm -ldl -lc++ -landroid -lpthread -llog -lz# --std=libc++

#ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    LOCAL_ARM_NEON := true
    LOCAL_CFLAGS := -DHAVE_NEON=1
#endif
#LOCAL_ALLOW_UNDEFINED_SYMBOLS := true
include $(BUILD_EXECUTABLE)
