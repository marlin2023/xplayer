//
// Created by chris on 9/26/16.
// native source code entry file.
//
#include <stdlib.h>
#include <jni.h>
#include "util/XLog.h"
#include "central/PlayerInner.h"

static const char *TAG = "JNI_ONLOAD";

// define the target class name .
static const char *className = "com/cmcm/v/cmplayersdk/XPlayer";


static void sayHello(JNIEnv *env, jobject thiz){
    XLog::e(TAG ,"======>say hello .");

    // new PlayerInner
    PlayerInner * playerInner = new PlayerInner();  // create player object

    playerInner->player_engine_init();  // engine init
    playerInner->open_file();           // open file ,get stream info

}

// define the native method mapping .
static JNINativeMethod methods[] = {
    {"sayHello", "()V", (void*)sayHello},
};


JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved)
{

    jint result = JNI_ERR;
    JNIEnv* env = NULL;
    jclass clazz;
    int methodsLenght;

    // get java vm .
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        XLog::e(TAG ,"ERROR: GetEnv failed\n");
        return JNI_ERR;
    }

    // get the class name .
    clazz = env->FindClass(className);
    if (clazz == NULL) {
        XLog::e(TAG ,"Native registration unable to find class '%s'", className);
        return JNI_ERR;
    }

    // get methods length ,and then register native methods
    methodsLenght = sizeof(methods) / sizeof(methods[0]);
    if (env->RegisterNatives(clazz, methods, methodsLenght) < 0) {
        XLog::e(TAG ,"RegisterNatives failed for '%s'", className);
        return JNI_ERR;
    }

    result = JNI_VERSION_1_4;
    return result;

}
