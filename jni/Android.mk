LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:=com_chris_yanlong_PlayerView.c \
				player.c

LOCAL_C_INCLUDES := $(JNI_H_INCLUDE)

LOCAL_C_INCLUDES += \
/home/chris/work/ffmpeg/refs/android-104-neon/include
#/home/chris/work/ffmpeg/refs/android-neon/include

#LOCAL_C_INCLUDES += \
#/home/chris/work/ffmpeg/refs/android/include

LOCAL_MODULE := libplayer

LOCAL_SHARED_LIBRARIES := libutils

LOCAL_LDLIBS := -fpic -g -D_CHRIS -llog -lz -L/home/chris/work/ffmpeg/refs/android-104-neon/lib  -lavformat -lavcodec -lswscale -lavutil 
#LOCAL_LDLIBS := -fpic -D_CHRIS -llog -lstagefright -lz   -lbinder -lstdc++ -lstlport -lutils -L/home/chris/work/android/toolchain/sysroot/usr/lib  -lavformat -lavcodec -lswscale -lavutil 
# -L/home/chris/work/android/android-ndk-r8/sources/cxx-stl/gnu-libstdc++/libs/armeabi-v7a -lgnustl_static


LOCAL_PRELINK_MODULE := false

LOCAL_PROGUARD_ENABLED := disabled

#APP_STL := gnustl_static
#LOCAL_ALLOW_UNDEFINED_SYMBOLS := true

include $(BUILD_SHARED_LIBRARY)
