//
// Created by chris on 10/19/16.
//
// Central Engine State Machine
//
//

#ifndef XPLAYER_CENTRALENGINESTATEMACHINE_H
#define XPLAYER_CENTRALENGINESTATEMACHINE_H

#include "MediaFile.h"
#include "PlayerState.h"
#include "MediaDecodeAudioStateMachine.h"
#include "MediaDecodeVideoStateMachine.h"

#include "util/XMessageQueue.h"


class CentralEngineStateMachine
{
public :

    CentralEngineStateMachine(MediaFile *mediaFile);
    CentralEngineStateMachine(MediaFile *mediaFile ,
                            MediaDecodeAudioStateMachine *mediaDecodeAudioStateMachine ,
                            MediaDecodeVideoStateMachine *mediaDecodeVideoStateMachine
    );
    ~CentralEngineStateMachine();

    /**
     * Main Work Thread ,the corresponding Central Engine StateMachine
     */
    void central_engine_thread(MediaFile *mediaFile);

    /**
     * central engine state machine process event
     */
    void central_engine_state_machine_process_event(player_event_e evt);

    /**
    * Demux inputfile streams to packet queue
    * audio packet put into audio packet queue
    * video packet put into video packet queue
    */
    int demux_2_packet_queue();

    /**
    * Change the state machine state value
    * state         will be set new_state
    * old_state     will be set previous state
    */
    void state_machine_change_state(player_state_e new_state);

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

private:

    /**
    * put packet into queue.
    * audio packet put into audio packet queue
    * video packet put into video packet queue
    */
    int add_packet_to_q(AVPacket *pkt ,MediaFile *mediaFile);


    /**
     * idle state processing work
     */
    void central_engine_do_process_idle(player_event_e evt);

    /**
     * initialized state processing work
     */
    void central_engine_do_process_initialized(player_event_e evt);

    /**
     * prepared state processing work
     */
    void central_engine_do_process_prepared(player_event_e evt);

    /**
     * buffering state processing work
     */
    void central_engine_do_process_buffering(player_event_e evt);

    /**
     * play_wait state processing work ,will process PLAY_EVT from JAVA Layer.
     */
    void central_engine_do_process_play_wait(player_event_e evt);

    /**
     * started state processing work
     */
    void central_engine_do_process_playing(player_event_e evt);

    /**
     * demux file end  state processing work
     */
    void central_engine_do_process_play_file_end(player_event_e evt);

    /**
     * play complete state processing work.
     */
    void central_engine_do_process_play_complete(player_event_e evt);

    /**
     * play paused state processing work.
     */
    void central_engine_do_process_play_paused(player_event_e evt);

    /**
     * play stopped state processing work.
     */
    void central_engine_do_process_play_stopped(player_event_e evt);

    /**
     * av_read_frame- retry count
     */
    int read_retry_count;

    /**
     * media file handle
     */
    MediaFile *mediaFileHandle;

    /**
     * Media Decode Audio State Machine Handle.
     */
    MediaDecodeAudioStateMachine *mediaDecodeAudioStateMachineHandle;

    /**
     * Media Decode Video State Machine Handle..
     */
    MediaDecodeVideoStateMachine *mediaDecodeVideoStateMachineHandle;

};




#endif //XPLAYER_CENTRALENGINESTATEMACHINE_H
