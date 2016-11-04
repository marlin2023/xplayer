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
#include "PlayerState.h"

#include "util/XMessageQueue.h"


class MediaDecodeAudioStateMachine
{
public :

    MediaDecodeAudioStateMachine(MediaFile *mediaFile);
    ~MediaDecodeAudioStateMachine();

    /**
     * Main Work Thread ,the corresponding Audio Decode StateMachine
     */
    void *audio_decode_thread();


    /**
    * decode one packet to frame ,and then put decoded frame into frame queue.
    *
    * @param packet[in]
    *
    * @param mediaFile[in]
    *
    */
    void decode_one_audio_packet(AVPacket *packet );

    /**
    * Change the state machine state value
    * state         will be set new_state
    * old_state     will be set previous state
    */
    void state_machine_change_state(player_state_e new_state);

    /**
     * audio decode state machine process event
     */
    void audio_decode_state_machine_process_event(player_event_e evt);

private:

    /**
     * decode wait state processing work
     */
    void do_process_audio_decode_wait(player_event_e evt);

    /**
     * decode start state processing work
     */
    void do_process_audio_decode_start(player_event_e evt);

    /**
     * decode work state processing work
     */
    void do_process_audio_decode_work(player_event_e evt);

    /**
     * decode seek wait state processing work
     */
    void do_process_audio_decode_seek_wait(player_event_e evt);

    /**
     * media file handle
     */
    MediaFile *mediaFileHandle;

public:

    /**
     * message queue for MediaDecodeAudioStateMachine
     */
    XMessageQueue *message_queue;

    /**
     * audio decoder machine Current State
     */
    player_state_e                state;

    /**
     * audio decoder machine Previous State
     */
    player_state_e                old_state;

};



#endif //XPLAYER_MEDIADECODEAUDIOSTATEMACHINE_H
