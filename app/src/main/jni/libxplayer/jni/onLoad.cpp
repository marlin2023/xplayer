//
// Created by chris on 9/26/16.
// native source code entry file.
//
#include <stdlib.h>
#include <unistd.h>

#include <jni.h>
#include "util/XLog.h"
#include "central/PlayerInner.h"
#include "central/YuvGLRender.h"

static const char *TAG = "JNI_ONLOAD";

// define the target class name .
static const char *className = "com/cmcm/v/cmplayersdk/XPlayer";

//TODO
static PlayerInner * playerInner;

static void sayHello(JNIEnv *env, jobject thiz){
    XLog::e(TAG ,"======>say hello .");

    // new PlayerInner
    //PlayerInner * playerInner = new PlayerInner();  // create player object

    playerInner = new PlayerInner();  // create player object

    playerInner->player_engine_init();  // engine init
    playerInner->player_engine_open();  // open file ,get stream info

}

static void native_init(JNIEnv *env, jobject thiz){
    XLog::e(TAG ,"======>native_init .");

    // new PlayerInner
    playerInner = new PlayerInner();  // create player object

    playerInner->player_engine_init();  // engine init
    playerInner->player_engine_open();  // open file ,get stream info

}

static void native_renderFrame(JNIEnv *env, jobject thiz){

    YuvGLRender *yuvGLRender = playerInner->yuvGLRender;
    yuvGLRender->render_frame();

}



// define the native method mapping .
static JNINativeMethod methods[] = {
    {"sayHello", "()V", (void*)sayHello},

    {"init", "()V", (void*)native_init},

    {"renderFrame", "()V", (void*)native_renderFrame},  // render frame.

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
