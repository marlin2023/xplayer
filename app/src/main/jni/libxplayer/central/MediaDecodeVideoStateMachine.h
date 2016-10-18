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

#include "util/XMessageQueue.h"

class MediaDecodeVideoStateMachine
{
public :

    MediaDecodeVideoStateMachine(MediaFile *mediaFile);
    ~MediaDecodeVideoStateMachine();

    /**
     * Main Work Thread ,the corresponding Video Decode StateMachine
     */
    void *video_decode_thread(MediaFile *mediaFile);

    /**
    * decode one packet to frame ,and then put decoded frame into video frame queue.
    *
    * @param packet[in]
    *
    * @param mediaFile[in]
    *
    */
    void decode_one_video_packet(AVPacket *packet);



private:

    /**
     * media file handle
     */
    MediaFile *mediaFileHandle;

public:

    /**
     * message queue for MediaDecodeVideoStateMachine
     */
    XMessageQueue *message_queue;


};

#endif //XPLAYER_MEDIADECODEVIDEOSTATEMACHINE_H
