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
    //
    yuvGLRender = new YuvGLRender(mediaFileHandle);
    audioRender = new OpenSLEngine(mediaFileHandle);

}

/**
 * player central engine init.
 */
void PlayerInner::player_engine_init()
{

    int ret;



    //1. Register all formats and codecs
    av_register_all();
    avformat_network_init();

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

void PlayerInner::set_data_source(const char *source_url)
{
    this->mediaFileHandle->setSourceUrl(source_url);
}

CM_BOOL PlayerInner::player_engine_prepare()
{
    // 1.initialize message queue for all state machines.
    // demux state machine push EVT_START
    centralEngineStateMachineHandle->message_queue->push(EVT_OPEN);
    XLog::e(TAG ,"create media demux thread size %d\n",centralEngineStateMachineHandle->message_queue->size());
}

CM_BOOL PlayerInner::player_start()
{
    audioRender->InitPlayout();
    audioRender->play();
    //TODO
}

bool PlayerInner::isPlaying()
{
    bool playerState;
    // TODO
    if (centralEngineStateMachineHandle->state == STATE_PLAY_PLAYING ||
        centralEngineStateMachineHandle->state == STATE_PREPARED ||
        centralEngineStateMachineHandle->state == STATE_PLAY_FILE_END ||
        centralEngineStateMachineHandle->state == STATE_BUFFERING) {
        return true;
    }

}

long PlayerInner::getCurrentPosition()
{
    // TODO need subtract the base time
    XLog::e(TAG ,"====>current_position_ms= %lld\n",this->mediaFileHandle->current_position_ms);
    return this->mediaFileHandle->current_position_ms;
}


long PlayerInner::getDuration()
{
    XLog::e(TAG ,"====>current_duration_ms= %lld\n",this->mediaFileHandle->current_position_ms);
    return this->mediaFileHandle->duration_ms;
}


void PlayerInner::seekTo(long msec)
{
    XLog::e(TAG ,"====>seekTo position = %ld\n",msec);
    this->mediaFileHandle->seekpos = msec;
    //
    av_read_pause(this->mediaFileHandle->format_context);
    centralEngineStateMachineHandle->message_queue->push(EVT_SEEK);
    mediaDecodeAudioStateMachineHandle->message_queue->push(EVT_SEEK);
    mediaDecodeVideoStateMachineHandle->message_queue->push(EVT_SEEK);

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
    // first ,get PlayerInner Object Handle
    PlayerInner *playerInner = (PlayerInner *)arg;

    // call audio decode thread
    playerInner->mediaDecodeAudioStateMachineHandle->audio_decode_thread();

}

