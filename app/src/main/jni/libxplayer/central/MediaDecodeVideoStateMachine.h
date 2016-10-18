//
// Created by chris on 10/17/16.
//

#ifndef XPLAYER_MEDIADECODEVIDEOSTATEMACHINE_H
#define XPLAYER_MEDIADECODEVIDEOSTATEMACHINE_H

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"

} // end of extern C

#include "MediaFile.h"
#include "FrameQueue.h"

class MediaDecodeVideoStateMachine
{
public :

    MediaDecodeVideoStateMachine();
    ~MediaDecodeVideoStateMachine();

    /**
    * decode one packet to frame ,and then put decoded frame into video frame queue.
    *
    * @param packet[in]
    *
    * @param mediaFile[in]
    *
    */
    void decode_one_video_packet(AVPacket *packet ,MediaFile *mediaFile);



    /**
    * video frame queue
    */
    FrameQueue *video_frame_queue;

};

#endif //XPLAYER_MEDIADECODEVIDEOSTATEMACHINE_H
