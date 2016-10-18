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


class MediaDecodeAudioStateMachine
{
public :

    MediaDecodeAudioStateMachine(MediaFile *mediaFile);
    ~MediaDecodeAudioStateMachine();

    /**
     * Main Work Thread ,the corresponding Audio Decode StateMachine
     */
    void *audio_decode_thread(void *arg);


    /**
    * decode one packet to frame ,and then put decoded frame into frame queue.
    *
    * @param packet[in]
    *
    * @param mediaFile[in]
    *
    */
    void decode_one_audio_packet(AVPacket *packet );


private:

    /**
     * media file handle
     */
    MediaFile *mediaFileHandle;

};



#endif //XPLAYER_MEDIADECODEAUDIOSTATEMACHINE_H
