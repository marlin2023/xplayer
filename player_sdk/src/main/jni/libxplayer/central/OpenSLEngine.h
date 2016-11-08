//
// Created by chris on 10/24/16.
//
// play pcm data us opensl es.
// [This Object will create a thread to perform audio data callback .]
//


#ifndef XPLAYER_OPENSLENGINE_H
#define XPLAYER_OPENSLENGINE_H


// for native audio
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

extern "C" {
#include "libswresample/swresample.h"
} // end of extern C


#include "MediaFile.h"

/**
 * SimpleBufferQueueCallback
 * These callback methods are called when data is required for playout.
 * They are both called from an internal "OpenSL ES thread" which is not attached to the Dalvik VM.
 */
void SimpleBufferQueueCallback(SLAndroidSimpleBufferQueueItf bq,
                                        void* context);

class OpenSLEngine
{

public:
    OpenSLEngine(MediaFile *mediaFile);
    ~OpenSLEngine();

    /**
     * start play audio.
     */
    void play();

    /**
     * pause audio play.
     */
    void pause();

    /**
     * resume audio play.
     */
    void resume();

    /**
     * stop and release resources.
     */
    void stop();

    /**
     * InitPlayout
     */
    int InitPlayout();

    /**
     * create audio player
     */
    int createAudioPlayer();



    /**
     * media file handle
     */
    MediaFile *mediaFileHandle;

private:

    /**
     * create opensl es object engine
     */
    void createEngine();

    /**
     * create output mix.
     */
    void createOutputMix();

    /**
     * engine object interfaces
     */
    SLObjectItf engineObject;

    /**
     * engine object interfaces
     */
    SLEngineItf engineEngine;

    /**
     * output mix interfaces
     */
    SLObjectItf outputMixObject;


    /**
     *  audio player object
     */
    SLObjectItf bqPlayerObject;

    /**
     * player play
     */
    SLPlayItf bqPlayerPlay;

    /**
     * buffer queue
     */
    SLAndroidSimpleBufferQueueItf    bqPlayerBufferQueue;

    enum
    {
        BUFFER_COUNT = 2,   // 5
    };


    bool m_paused;

    bool m_running;

public:
    uint8_t * resampled_buf;

    SwrContext *swr_ctx;

    /**
     * mark if created success or not
     */
    bool isInitialized;

};


#endif //XPLAYER_OPENSLENGINE_H
