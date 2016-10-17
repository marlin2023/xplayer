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


class PlayerInner{

public:
    PlayerInner();
    ~PlayerInner();


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

    void player_engine_init();

};


#endif //XPLAYER_PLAYERINNER_H
