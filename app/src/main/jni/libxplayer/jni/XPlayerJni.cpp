//
// Created by chris on 9/26/16.
// JNI File for XPlayer.cpp and XPlayer.java
//

#define TAG "XPlayer-JNI"

#include <jni.h>
#include "../util/XLog.h"

static const char* const kClassPathName = "com/cmcm/v/cmplayersdk/XPlayer";


//TODO


// define the native method mapping .
static JNINativeMethod methods[] = {
//        {"sayHello", "()V", (void*)sayHello},
};

/**
 * get methods length ,and register native method.
 */
int register_android_media_FFMpegPlayerAndroid(JNIEnv *env) {
    //return jniRegisterNativeMethods(env, kClassPathName, gMethods, sizeof(gMethods) / sizeof(gMethods[0]));
    return 0;
}


