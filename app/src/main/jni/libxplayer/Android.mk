LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)


LOCAL_MODULE := cmxplayer

LOCAL_SRC_FILES := jni/onLoad.cpp   \
                jni/XPlayerJni.cpp  \
                \
                util/XIPC.cpp   \
                util/XLog.cpp   \
                util/XMessageQueue.cpp  \
                util/XMutexlock.cpp

LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)