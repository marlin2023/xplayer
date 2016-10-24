//
// Created by chris on 9/29/16.
//

#ifndef XPLAYER_PLAYERINNER_H
#define XPLAYER_PLAYERINNER_H

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"

} // end of extern C

#include "util/cm_std.h"

#include "MediaFile.h"
#include "CentralEngineStateMachine.h"
#include "MediaDecodeAudioStateMachine.h"
#include "MediaDecodeVideoStateMachine.h"
#include "YuvGLRender.h"
#include "OpenSLEngine.h"

/**
 * Thread 1 the corresponding Central Engine StateMachine
 */
void *central_engine_thread(void *arg);

/**
 * Thread 2 the corresponding Video Decode StateMachine
 */
void *video_decode_thread(void *arg);

/**
 * Thread 3 the corresponding audio Decode StateMachine
 */
void *audio_decode_thread(void *arg);

/**
 * Thread 4 the corresponding video Render StateMachine
 */
void *video_render_thread(void *arg);



class PlayerInner{

public:
    PlayerInner();
    ~PlayerInner();

    /**
     * Create object then player engine initialize
     * 1. register ffmpeg formats & codecs
     * 2. create pthreads
     */
    void player_engine_init();


    /**
     * player engine open
     * open file and get file streams information
     */
    CM_BOOL player_engine_open();

    /**
     * player start play.
     */
    CM_BOOL player_start();

    //-----------*******************-------------
    //          public member variable
    //-----------*******************-------------

    /**
     * Media File Handler contain file all information.
     */
    MediaFile *mediaFileHandle;

    /**
     * Central Engine State Machine Handle.
     */
    CentralEngineStateMachine *centralEngineStateMachineHandle;

    /**
     * Media Decode Audio State Machine Handle.
     */
    MediaDecodeAudioStateMachine *mediaDecodeAudioStateMachineHandle;

    /**
     * Media Decode Video State Machine Handle..
     */
    MediaDecodeVideoStateMachine *mediaDecodeVideoStateMachineHandle;

    /**
     * video view Render
     */
     YuvGLRender *yuvGLRender;

    /**
     * audio output use opensl es engine.
     */
     OpenSLEngine *audioRender;
private:

    //-----------*******************-------------
    //          private member function
    //-----------*******************-------------




};


#endif //XPLAYER_PLAYERINNER_H
