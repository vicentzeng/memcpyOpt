#APP_STL := c++_shared
APP_STL := c++_static  #none system c++_static c++_shared #remove std
LDFLAGS := -nodefaultlibs -lc -lm -ldl -lc++  #
APP_CPPFLAGS := -frtti -fexceptions  -frtti -DCC_ENABLE_CHIPMUNK_INTEGRATION=1 -std=c++11 -fsigned-char
APP_CFLAGS += -Wno-error=format-security
APP_LDFLAGS := -latomic
APP_ABI := armeabi-v7a arm64-v8a
APP_PLATFORM := android-16
NDK_TOOLCHAIN_VERSION=clang
#LIBCXX_FORCE_REBUILD := true
