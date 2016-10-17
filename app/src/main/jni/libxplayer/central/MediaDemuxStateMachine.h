//
// Created by chris on 10/17/16.
//

#ifndef XPLAYER_MEDIADEMUXSTATEMACHINE_H
#define XPLAYER_MEDIADEMUXSTATEMACHINE_H

#include "MediaFile.h"

class MediaDemuxStateMachine
{
public :

    MediaDemuxStateMachine();
    ~MediaDemuxStateMachine();

    /**
    * Demux inputfile streams to packet queue
    * audio packet put into audio packet queue
    * video packet put into video packet queue
    */
    int demux_2_packet_queue(MediaFile *mediaFile);

private:

    /**
    * put packet into queue.
    * audio packet put into audio packet queue
    * video packet put into video packet queue
    */
    int add_packet_to_q(AVPacket *pkt ,MediaFile *mediaFile);

private:

    /**
     * av_read_frame- retry count
     */
    int read_retry_count;


};

#endif //XPLAYER_MEDIADEMUXSTATEMACHINE_H
