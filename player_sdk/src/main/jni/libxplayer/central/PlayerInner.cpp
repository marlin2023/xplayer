//
// Created by chris on 9/29/16.
//

#include "PlayerInner.h"
#include "util/XLog.h"
#include "util/XMessageType.h"
#include "xplayer_android_def.h"

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

    audioRender = new OpenSLEngine(mediaFileHandle);

}

PlayerInner::~PlayerInner()
{
    XLog::e(TAG ,"======>in ~PlayerInner start.");

    if(mediaFileHandle){
        XLog::e(TAG ,"======>delete mediaFileHandle.");
        delete mediaFileHandle;
        mediaFileHandle = NULL;
    }

    if(mediaDecodeAudioStateMachineHandle){
        XLog::e(TAG ,"======>delete mediaDecodeAudioStateMachineHandle.");
        delete         mediaDecodeAudioStateMachineHandle;
        mediaDecodeAudioStateMachineHandle = NULL;
    }

    if(mediaDecodeVideoStateMachineHandle){
        XLog::e(TAG ,"======>delete mediaDecodeVideoStateMachineHandle.");
        delete         mediaDecodeVideoStateMachineHandle;
        mediaDecodeVideoStateMachineHandle = NULL;
    }

    if(centralEngineStateMachineHandle){
        XLog::e(TAG ,"======>delete centralEngineStateMachineHandle.");
        delete centralEngineStateMachineHandle;
        centralEngineStateMachineHandle = NULL;
    }

    // audio engine
    if(audioRender){
        XLog::e(TAG ,"======>delete audioRender.");
        delete audioRender;
        audioRender = NULL;
    }

    XLog::e(TAG ,"======>in ~PlayerInner end.");
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
        this->mediaFileHandle->notify(MEDIA_ERROR, MEDIA_CREATE_DEMUX_THREAD_ERROR, ret);
        goto init_eout;
    }

    ret = pthread_create(&decode_video_tid, NULL, video_decode_thread, (void*)this);
    if(ret)
    {
        XLog::e(TAG ,"create video decode thread err %d\n",ret);
        this->mediaFileHandle->notify(MEDIA_ERROR, MEDIA_CREATE_VIDEO_THREAD_ERROR, ret);
        goto init_eout;
    }

    // start decode thread
    ret = pthread_create(&decode_audio_tid, NULL, audio_decode_thread, (void*)this);
    if(ret)
    {
        XLog::e(TAG ,"create audio decode thread err %d\n",ret);
        this->mediaFileHandle->notify(MEDIA_ERROR, MEDIA_CREATE_AUDIO_THREAD_ERROR, ret);
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
    mediaFileHandle->message_queue_central_engine->push(EVT_OPEN);
    XLog::e(TAG ,"create media demux thread size %d\n",mediaFileHandle->message_queue_central_engine->size());
}

CM_BOOL PlayerInner::player_start()
{
    //audioRender->InitPlayout();
    audioRender->play();
}

bool PlayerInner::isPlaying()
{
    bool playerState;

    if(mediaFileHandle->isPaused){ // player in paused state.
       return false;
    }

    // TODO
    if (centralEngineStateMachineHandle->state == STATE_PLAY_PLAYING ||
        centralEngineStateMachineHandle->state == STATE_PREPARED ||
        centralEngineStateMachineHandle->state == STATE_PLAY_FILE_END ||
        centralEngineStateMachineHandle->state == STATE_BUFFERING) {
        return true;
    }
    return false;
}

long PlayerInner::getCurrentPosition()
{

    if(mediaFileHandle == NULL){
        return 0.f;
    }
    // TODO need subtract the base time
    XLog::e(TAG ,"====>current_position_ms= %ld\n",this->mediaFileHandle->current_position_ms);
    if(this->mediaFileHandle->seeking_mark){
        return this->mediaFileHandle->seekpos;
    }
    return this->mediaFileHandle->current_position_ms;
}


long PlayerInner::getDuration()
{
    if(mediaFileHandle == NULL){
        return 0.f;
    }

    XLog::e(TAG ,"====>current_duration_ms= %lld\n",this->mediaFileHandle->duration_ms);
    return this->mediaFileHandle->duration_ms;
}


void PlayerInner::seekTo(long msec)
{
    if(mediaFileHandle == NULL){
        return ;
    }

    if(this->mediaFileHandle->seekpos > this->mediaFileHandle->duration_ms){
        this->mediaFileHandle->seekpos = this->mediaFileHandle->duration_ms;
    }

    XLog::e(TAG ,"====>PlayerInner::seekTo position = %ld\n",msec);
    this->mediaFileHandle->seeking_mark = true;
    this->mediaFileHandle->seekpos = msec;
    av_read_pause(this->mediaFileHandle->format_context);

    mediaFileHandle->message_queue_video_decode->push_front(EVT_SEEK); // start to decode video packet data .
    mediaFileHandle->message_queue_audio_decode->push_front(EVT_SEEK);

    // 第一次起播之前如果seek的话 不执行这里
    if(mediaFileHandle->isPlayedBefore){   // first loading end
        mediaFileHandle->notify(MEDIA_INFO ,MEDIA_INFO_BUFFERING_END ,0);   //
        mediaFileHandle->notify(MEDIA_INFO ,MEDIA_INFO_BUFFERING_START ,0); // disappear loading
    }
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

