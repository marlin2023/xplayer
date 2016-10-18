//
// Created by chris on 10/17/16.
//

#ifndef XPLAYER_MEDIADECODEAUDIOSTATEMACHINE_H
#define XPLAYER_MEDIADECODEAUDIOSTATEMACHINE_H


extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"

} // end of extern C

#include "MediaFile.h"
#include "FrameQueue.h"

class MediaDecodeAudioStateMachine
{
public :

    MediaDecodeAudioStateMachine();
    ~MediaDecodeAudioStateMachine();


    /**
    * decode one packet to frame ,and then put decoded frame into frame queue.
    *
    * @param packet[in]
    *
    * @param mediaFile[in]
    *
    */
    void decode_one_audio_packet(AVPacket *packet ,MediaFile *mediaFile);


    /**
     * audio frame queue
     */
    FrameQueue *audio_frame_queue;


};



#endif //XPLAYER_MEDIADECODEAUDIOSTATEMACHINE_H
