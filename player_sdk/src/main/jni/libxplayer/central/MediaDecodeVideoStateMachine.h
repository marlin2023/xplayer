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
#include "PlayerState.h"

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
    void decode_one_video_packet(AVPacket *packet );

    /**
    * Change the state machine state value
    * state         will be set new_state
    * old_state     will be set previous state
    */
    void state_machine_change_state(player_state_e new_state);

    /**
     * video decode state machine process event
     */
    void video_decode_state_machine_process_event(player_event_e evt);

private:

    /**
     * decode wait state processing work
     */
    void do_process_video_decode_wait(player_event_e evt);

    /**
     * decode start state processing work
     */
    void do_process_video_decode_start(player_event_e evt);

    /**
     * decode work state processing work
     */
    void do_process_video_decode_work(player_event_e evt);

    /**
     * decode seek wait state processing work
     */
    void do_process_video_decode_seek_wait(player_event_e evt);

    /**
     * media file handle
     */
    MediaFile *mediaFileHandle;


public:
    /**
     * message queue for MediaDecodeVideoStateMachine
     */
    //XMessageQueue *message_queue;

    /**
     * video decoder machine Current State
     */
    player_state_e                state;

    /**
     * video decoder machine Previous State
     */
    player_state_e                old_state;


};

#endif //XPLAYER_MEDIADECODEVIDEOSTATEMACHINE_H
