LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

# library name
LOCAL_MODULE := cmxplayer

# set include path
LOCAL_C_INCLUDES += $(realpath $(LOCAL_PATH)/..)
LOCAL_C_INCLUDES += $(realpath $(LOCAL_PATH)/../third-party/ffmpeg/ffmpeg-3.2/)

# source files
LOCAL_SRC_FILES := jni/onLoad.cpp   \
                jni/XPlayerJni.cpp  \
                \
                util/XIPC.cpp   \
                util/XLog.cpp   \
                util/XMessageQueue.cpp  \
                util/XMutexlock.cpp \
                util/XTimeUtil.cpp \
                \
                central/PacketQueue.cpp \
                central/FrameQueue.cpp \
                central/MediaFile.cpp    \
                central/CentralEngineStateMachine.cpp    \
                central/MediaDecodeVideoStateMachine.cpp    \
                central/MediaDecodeAudioStateMachine.cpp    \
                central/YuvGLRender.cpp    \
                central/OpenSLEngine.cpp    \
                central/PlayerInner.cpp

# compile flags
LOCAL_CFLAGS := -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS

# linked library
LOCAL_LDLIBS :=  -llog -lffmpeg -lGLESv2 -lOpenSLES

# linked library search path.
LOCAL_LDFLAGS += \
    -L$(realpath $(LOCAL_PATH)/../third-party/build/ffmpeg-armv7a/output)

include $(BUILD_SHARED_LIBRARY)