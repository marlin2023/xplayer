//
// Created by chris on 10/17/16.
//

#ifndef XPLAYER_MEDIADEMUXSTATEMACHINE_H
#define XPLAYER_MEDIADEMUXSTATEMACHINE_H

#include "MediaFile.h"
#include "PlayerState.h"

#include "util/XMessageQueue.h"

class MediaDemuxStateMachine
{
public :

    MediaDemuxStateMachine();
    ~MediaDemuxStateMachine();

    /**
     * Main Work Thread ,the corresponding Media Demux StateMachine
     */
    void media_demux_thread(MediaFile *mediaFile);

    /**
     * media demux state machine process event
     */
    void media_demux_state_machine_process_event(player_event_e evt);

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



    /**
     * av_read_frame- retry count
     */
    int read_retry_count;

public:

    /**
     * message queue for MediaDecodeVideoStateMachine
     */
    XMessageQueue *message_queue;

    /**
     * demux state machine Current State
     */
    player_state_e                state;

    /**
     * demux state machine Previous State
     */
    player_state_e                old_state;

};

#endif //XPLAYER_MEDIADEMUXSTATEMACHINE_H
