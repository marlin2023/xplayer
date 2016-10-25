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


static void native_setup(JNIEnv *env, jobject thiz)
{
    XLog::e(TAG ,"======>native_setup .");
    // new PlayerInner
    playerInner = new PlayerInner();  // create player object

}


static void native_init(JNIEnv *env, jobject thiz)
{
    XLog::e(TAG ,"======>native_init .");
    playerInner->player_engine_init();  // engine init
}

static void native_setDataSource(JNIEnv *env, jobject thiz ,jstring dataSource)
{
    const char *c_data_source = env->GetStringUTFChars(dataSource, NULL );

    //
    playerInner->set_data_source(c_data_source);

    XLog::e(TAG ,"======>set_data_source,source_url=%s." ,c_data_source);
    //env->ReleaseStringUTFChars(dataSource, c_data_source);    // not release TODO

}

static void native_prepareAsync(JNIEnv *env, jobject thiz){

    playerInner->player_engine_prepare();  // open file ,get stream info
}

static void native_renderFrame(JNIEnv *env, jobject thiz){

    YuvGLRender *yuvGLRender = playerInner->yuvGLRender;
    yuvGLRender->render_frame();

}

static void native_notify(JNIEnv *env, jobject thiz){


}

// define the native method mapping .
static JNINativeMethod methods[] =
{

    {"native_setup", "()V", (void*)native_setup},
    {"init", "()V", (void*)native_init},
    {"setDataSource", "(Ljava/lang/String;)V", (void*)native_setDataSource},
    {"prepareAsync", "()V", (void*)native_prepareAsync},

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
