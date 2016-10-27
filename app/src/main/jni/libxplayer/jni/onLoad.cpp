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

struct fields_t {
    jfieldID    context;
    jmethodID   post_event;
};
fields_t fields;

static JavaVM *sVm;
//TODO
static PlayerInner * playerInner;

/*
 * Throw an exception with the specified class and an optional message.
 */
int jniThrowException(JNIEnv* env, const char* className, const char* msg) {
    jclass exceptionClass = env->FindClass(className);
    if (exceptionClass == NULL) {
        XLog::d(ANDROID_LOG_ERROR,
            TAG,
            "Unable to find exception class %s",
                    className);
        return -1;
    }

    if (env->ThrowNew(exceptionClass, msg) != JNI_OK) {
        XLog::d(ANDROID_LOG_ERROR,
            TAG,
            "Failed throwing '%s' '%s'",
            className, msg);
    }
    return 0;
}

JavaVM* getJvm()
{
    return sVm;
}

JNIEnv* getJNIEnv() {
    JNIEnv* env = NULL;
    bool isAttached = false;
    int status = 0;

    if (sVm->GetEnv((void**) &env, JNI_VERSION_1_4) < 0) {
        return NULL;
    }
    return env;
}

// ----------------------------------------------------------------------------
// ref-counted object for callbacks
class JNIMediaPlayerListener: public MediaPlayerListener
{
public:
    JNIMediaPlayerListener(JNIEnv* env, jobject thiz, jobject weak_thiz);
    ~JNIMediaPlayerListener();
    void notify(int msg, int ext1, int ext2);

private:
    JNIMediaPlayerListener();
    jclass      mClass;     // Reference to MediaPlayer class
    jobject     mObject;    // Weak ref to MediaPlayer Java object to call on
};


JNIMediaPlayerListener::JNIMediaPlayerListener(JNIEnv* env, jobject thiz, jobject weak_thiz)
{

    // Hold onto the MediaPlayer class for use in calling the static method
    // that posts events to the application thread.

    jclass clazz = env->GetObjectClass(thiz);
    if (clazz == NULL) {
        XLog::e(TAG ,"Can't find android/media/MediaPlayer");
        jniThrowException(env, "java/lang/Exception", NULL);
        return;
    }
    mClass = (jclass)env->NewGlobalRef(clazz);

    // We use a weak reference so the MediaPlayer object can be garbage collected.
    // The reference is only used as a proxy for callbacks.
    mObject  = env->NewGlobalRef(weak_thiz);
}

JNIMediaPlayerListener::~JNIMediaPlayerListener()
{
    // remove global references
    JNIEnv *env = getJNIEnv();
    if(env){
        env->DeleteGlobalRef(mObject);
        env->DeleteGlobalRef(mClass);
    }
}

void JNIMediaPlayerListener::notify(int msg, int ext1, int ext2)
{
    JNIEnv *env = getJNIEnv();
    JavaVM *svm = getJvm();
    bool isAttached = false;
    if(env == NULL)
    {
        svm->AttachCurrentThread(&env, NULL);
        isAttached = true;
    }
    env->CallStaticVoidMethod(mClass, fields.post_event, mObject, msg, ext1, ext2, 0);

    if(isAttached)
    {
        svm->DetachCurrentThread();
    }
}

// ----------------------------------------------------------------------------

// be called when library be loaded.
static void
jni_native_init(JNIEnv *env , jobject thiz)
{
    jclass clazz;
    clazz = env->FindClass(className);
    if (clazz == NULL) {
        jniThrowException(env, "java/lang/RuntimeException", "Can't find android/media/MediaPlayer");
        return;
    }

    fields.post_event = env->GetStaticMethodID(clazz, "postEventFromNative",
                                                   "(Ljava/lang/Object;IIILjava/lang/Object;)V");
    if (fields.post_event == NULL) {
        jniThrowException(env, "java/lang/RuntimeException", "Can't find FFMpegMediaPlayer.postEventFromNative");
        return;
    }
}
//
static void jni_native_setup(JNIEnv *env, jobject thiz ,jobject weak_this)
{
    XLog::e(TAG ,"======>native_setup .");

    #if 0
    if(mbPlayer)
    {
         Log::d(ANDROID_LOG_INFO, TAG, "=core=[%s,%s:%d] Player Already exists!",
        __FILE__, __FUNCTION__, __LINE__);
        return -2;
    }
    #endif

    playerInner = new PlayerInner();
    if (playerInner == NULL) {
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return;
    }

     // create new listener and give it to MediaPlayer
    JNIMediaPlayerListener *listener = new JNIMediaPlayerListener(env, thiz, weak_this);
    playerInner->mediaFileHandle->setListener(listener);

    // Stow our new C++ MediaPlayer in an opaque field in the Java object.
    //setMediaPlayer(env, thiz, playerInner);

}



static void native_init1(JNIEnv *env, jobject thiz)
{
    XLog::e(TAG ,"======>native_init .");
    playerInner->player_engine_init();  // engine init
}

static void native_initEGLCtx(JNIEnv *env, jobject thiz)
{
    XLog::e(TAG ,"======>native_initEGLCtx .");
    playerInner->yuvGLRender->init();  // engine init
}


static void native_setDataSource(JNIEnv *env, jobject thiz ,jstring dataSource)
{
    const char *c_data_source = env->GetStringUTFChars(dataSource, NULL );

    //
    playerInner->set_data_source(c_data_source);

    XLog::e(TAG ,"======>set_data_source,source_url=%s." ,c_data_source);
    //env->ReleaseStringUTFChars(dataSource, c_data_source);    // not release TODO

}

static void native_prepareAsync(JNIEnv *env, jobject thiz)
{
    playerInner->player_engine_prepare();  // open file ,get stream info
}

static void native_start(JNIEnv *env, jobject thiz)
{
    playerInner->player_engine_prepare();
    playerInner->centralEngineStateMachineHandle->message_queue->push(EVT_START);   //TODO here should be performed in upper layer
}

static void native_play(JNIEnv *env, jobject thiz)
{
    // start audio decoder & video decoder thread.
    // TODO send EVT_START to audio decode state machine & video decode state machine should be put in el_do_start_central_engine
    playerInner->mediaDecodeAudioStateMachineHandle->message_queue->push(EVT_START);
    playerInner->mediaDecodeVideoStateMachineHandle->message_queue->push(EVT_START);
}

static void native_renderFrame(JNIEnv *env, jobject thiz)
{

    YuvGLRender *yuvGLRender = playerInner->yuvGLRender;
    yuvGLRender->render_frame();

}

// define the native method mapping .
static JNINativeMethod methods[] =
{

    {"_native_init",     "()V",                                      (void*)jni_native_init},
    {"_native_setup",    "(Ljava/lang/Object;)V",                    (void*)jni_native_setup},

    {"_init",            "()V",                                      (void*)native_init1},
    {"_initEGLCtx",      "()V",                                      (void*)native_initEGLCtx},

    {"_setDataSource",   "(Ljava/lang/String;)V",                    (void*)native_setDataSource},
    {"_prepareAsync",    "()V",                                      (void*)native_prepareAsync},

    {"_start",           "()V",                                      (void*)native_start},
    {"_play",            "()V",                                      (void*)native_play},

    {"_renderFrame",     "()V",                                      (void*)native_renderFrame},  // render frame.

};


JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved)
{

    jint result = JNI_ERR;
    JNIEnv* env = NULL;
    jclass clazz;
    int methodsLenght;

    //
    sVm = vm;

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
