//
// Created by chris on 9/29/16.
//

#include "PlayerInner.h"
#include "util/XLog.h"
#include "util/XMessageType.h"

#define TAG "PLAYER_INNER"


PlayerInner::PlayerInner()
{
    // TODO
    // mediaFile Handle
    mediaFileHandle = new MediaFile();

    mediaDecodeAudioStateMachineHandle = new MediaDecodeAudioStateMachine(mediaFileHandle);
    mediaDecodeVideoStateMachineHandle = new MediaDecodeVideoStateMachine(mediaFileHandle);

    // central engine state machine
    centralEngineStateMachineHandle = new CentralEngineStateMachine(mediaFileHandle ,
                                                                mediaDecodeAudioStateMachineHandle ,
                                                                mediaDecodeVideoStateMachineHandle);

}

/**
 * player central engine init.
 */
void PlayerInner::player_engine_init()
{

    int ret;

    // media demux thread
    pthread_t media_demux_tid;

    // video decode thread
    pthread_t decode_video_tid;

    // audio decode thread
    pthread_t decode_audio_tid;

    //1. Register all formats and codecs
    av_register_all();
    avformat_network_init();
    //

    // 2.Create player will used threads
    ret = pthread_create(&media_demux_tid, NULL, central_engine_thread, (void*)this);
    if(ret)
    {
        XLog::e(TAG ,"create media demux thread err %d\n",ret);
        goto init_eout;
    }


    ret = pthread_create(&decode_video_tid, NULL, video_decode_thread, (void*)this);
    if(ret)
    {
        XLog::e(TAG ,"create video decode thread err %d\n",ret);
        goto init_eout;
    }

    // start decode thread
    ret = pthread_create(&decode_audio_tid, NULL, audio_decode_thread, (void*)this);
    if(ret)
    {
        XLog::e(TAG ,"create audio decode thread err %d\n",ret);
        goto init_eout;
    }

    centralEngineStateMachineHandle->state_machine_change_state(STATE_INITIALIZED);

init_eout:

    return ;
}

CM_BOOL PlayerInner::player_engine_open()
{
    // 1.initialize message queue for all state machines.
    // demux state machine push EVT_START
    centralEngineStateMachineHandle->message_queue->push(EVT_OPEN);

    XLog::e(TAG ,"create media demux thread size %d\n",centralEngineStateMachineHandle->message_queue->size());

}



//-----------*******************-------------
//          Thread according to function
//-----------*******************-------------

/**
 * Thread 1 the corresponding Central Engine StateMachine
 */
void *central_engine_thread(void *arg)
{
    // first ,get PlayerInner Object Handle
    PlayerInner *playerInner = (PlayerInner *)arg;

    // call media demux thread
    playerInner->centralEngineStateMachineHandle->central_engine_thread(playerInner->mediaFileHandle);
}


/**
 * Thread 2 the corresponding Video Decode StateMachine
 */
void *video_decode_thread(void *arg)
{
    // first ,get PlayerInner Object Handle
    PlayerInner *playerInner = (PlayerInner *)arg;

    // call video decode thread
    playerInner->mediaDecodeVideoStateMachineHandle->video_decode_thread(playerInner->mediaFileHandle);

}

/**
 * Thread 3 the corresponding audio Decode StateMachine
 */
void *audio_decode_thread(void *arg)
{


}

