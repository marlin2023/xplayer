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
#include "MediaDemuxStateMachine.h"

class PlayerInner{

public:
    PlayerInner();
    ~PlayerInner();

    /**
     * Media File Handler contain file all information.
     */
    MediaFile *mediaFileHandle;

    /**
     * Media Demux State Machine Handle.
     */
    MediaDemuxStateMachine *mediaDemuxStateMachineHandle;

private:

    //-----------*******************-------------
    //          private member function
    //-----------*******************-------------

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


public:

    /**
     * player engine initialize
     * 1. register ffmpeg formats & codecs
     */
    void player_engine_init();

    /**
     * player engine open
     * open file and get file streams information
     */
    CM_BOOL player_engine_open();

};


#endif //XPLAYER_PLAYERINNER_H
