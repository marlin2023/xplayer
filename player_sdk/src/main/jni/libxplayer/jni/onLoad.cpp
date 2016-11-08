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
static const char *className = "com/cmcm/v/player_sdk/player/XPlayer";

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
    //playerInner->player_engine_prepare();
    playerInner->mediaFileHandle->message_queue_central_engine->push(EVT_START);   //TODO here should be performed in upper layer
    usleep(50* 1000);   // for seek
}

static void native_play(JNIEnv *env, jobject thiz)
{
    // start audio decoder & video decoder thread.
    // TODO send EVT_START to audio decode state machine & video decode state machine should be put in el_do_start_central_engine
    playerInner->mediaFileHandle->message_queue_video_decode->push(EVT_START);
    playerInner->mediaFileHandle->message_queue_audio_decode->push(EVT_START);
    // TODO
    playerInner->mediaFileHandle->message_queue_central_engine->push(EVT_PLAY);
    // audio render thread.
    playerInner->player_start();
    XLog::e(TAG ,"======>playerInner->player_start.SimpleBufferQueueCallback");

}

static void native_pause(JNIEnv *env, jobject thiz)
{
    // playerInner->player_engine_prepare();
    // playerInner->centralEngineStateMachineHandle->message_queue->push(EVT_START);

    // audio opensl es
    playerInner->audioRender->pause();

    // process state
    playerInner->mediaFileHandle->message_queue_video_decode->push(EVT_PAUSE);
    playerInner->mediaFileHandle->message_queue_audio_decode->push(EVT_PAUSE);
    playerInner->mediaFileHandle->message_queue_central_engine->push(EVT_PAUSE);

}

static void native_resume(JNIEnv *env, jobject thiz)
{
    if(playerInner->centralEngineStateMachineHandle->state == STATE_BUFFERING){
        XLog::e(TAG ,"======>call native_resume,but in buffering state ,and return.");
        return;
    }

    // audio opensl es
    playerInner->audioRender->resume();

    // process state
    playerInner->mediaFileHandle->message_queue_video_decode->push(EVT_RESUME);
    playerInner->mediaFileHandle->message_queue_audio_decode->push(EVT_RESUME);
    playerInner->mediaFileHandle->message_queue_central_engine->push(EVT_RESUME);
}

static void native_stop(JNIEnv *env, jobject thiz)
{
    XLog::e(TAG ,"======>in native_stop 1.");
    if(playerInner->audioRender->isInitialized){
        XLog::e(TAG ,"======>in native_stop 1.1.");
        // audio opensl es
        playerInner->audioRender->pause();
        XLog::e(TAG ,"======>in native_stop 1.2.");
    }
    XLog::e(TAG ,"======>in native_stop 2.");

    // set
    playerInner->mediaFileHandle->stop_flag = true;
    XLog::e(TAG ,"======>in native_stop 2.1");

    // process state
    playerInner->mediaFileHandle->message_queue_video_decode->push(EVT_STOP);
    playerInner->mediaFileHandle->message_queue_audio_decode->push(EVT_STOP);
    playerInner->mediaFileHandle->message_queue_central_engine->push(EVT_STOP);

    // clear packet&frame q
    playerInner->mediaFileHandle->audio_queue->flush();
    playerInner->mediaFileHandle->video_queue->flush();
    playerInner->mediaFileHandle->audio_frame_queue->flush();
    playerInner->mediaFileHandle->video_frame_queue->flush();
    XLog::e(TAG ,"======>in native_stop 3.");

}

static void native_release(JNIEnv *env, jobject thiz)
{
    // 关闭文件之前，分别向几个队列发CLOSE消息。
    // 这几个队列现在不处理CLOSE消息了，直接退出线程

    playerInner->mediaFileHandle->message_queue_video_decode->push_front(EVT_CLOSE);
    playerInner->mediaFileHandle->message_queue_audio_decode->push_front(EVT_CLOSE);
    playerInner->mediaFileHandle->message_queue_central_engine->push_front(EVT_CLOSE);


    playerInner->mediaFileHandle->message_queue_video_decode->push_front(EVT_EXIT_THREAD);
    playerInner->mediaFileHandle->message_queue_audio_decode->push_front(EVT_EXIT_THREAD);
    playerInner->mediaFileHandle->message_queue_central_engine->push_front(EVT_EXIT_THREAD);

    XLog::e(TAG ,"======>in native_release 1.");
    pthread_join(playerInner->media_demux_tid, NULL);
    XLog::e(TAG ,"======>in native_release 2.");
    pthread_join(playerInner->decode_video_tid, NULL);
    XLog::e(TAG ,"======>in native_release 3.");
    pthread_join(playerInner->decode_audio_tid, NULL);
    XLog::e(TAG ,"======>in native_release 4.");

    playerInner->mediaFileHandle->message_queue_video_decode->flush();
    XLog::e(TAG ,"======>in native_release 5.");
    playerInner->mediaFileHandle->message_queue_audio_decode->flush();
    XLog::e(TAG ,"======>in native_release 6.");
    playerInner->mediaFileHandle->message_queue_central_engine->flush();
    XLog::e(TAG ,"======>in native_release 7.");
    // TODO
    // Release ffmpeg resources

}


static void native_renderFrame(JNIEnv *env, jobject thiz)
{

    YuvGLRender *yuvGLRender = playerInner->yuvGLRender;
    yuvGLRender->render_frame();

}

static jboolean native_isPlaying(JNIEnv *env, jobject thiz)
{

    jboolean retval = JNI_FALSE;
    retval = playerInner->isPlaying();
    return retval;

}

static jlong native_getCurrentPosition(JNIEnv *env, jobject thiz)
{
     jlong retval = 0;
     retval = playerInner->getCurrentPosition();
     return retval;

}

static jlong native_getDuration(JNIEnv *env, jobject thiz)
{
     jlong retval = 0;
     retval = playerInner->getDuration();
     return retval;
}

static void native_seekTo(JNIEnv *env, jobject thiz ,jlong seekedPositionMsec )
{
    long seek_time = seekedPositionMsec;
    XLog::e(TAG ,"======>native_seekTo :%ld" ,seek_time);
    // set current_position_ms
    playerInner->mediaFileHandle->current_position_ms = seekedPositionMsec;
    playerInner->seekTo(seek_time);

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
    {"_pause",           "()V",                                      (void*)native_pause},
    {"_resume",          "()V",                                      (void*)native_resume},
    {"_stop",            "()V",                                      (void*)native_stop},
    {"_release",         "()V",                                      (void*)native_release},


    {"_isPlaying",                       "()Z",                                      (void*)native_isPlaying},
    {"_getCurrentPosition",              "()J",                                      (void*)native_getCurrentPosition},
    {"_getDuration",                     "()J",                                      (void*)native_getDuration},

    {"_seekTo",                          "(J)V",                                     (void*)native_seekTo},

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
