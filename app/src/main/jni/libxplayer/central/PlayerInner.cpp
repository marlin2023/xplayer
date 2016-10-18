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

    // mediaDemuxStateMachine
    mediaDemuxStateMachineHandle = new MediaDemuxStateMachine();

    mediaDecodeAudioStateMachineHandle = new MediaDecodeAudioStateMachine(mediaFileHandle);
    mediaDecodeVideoStateMachineHandle = new MediaDecodeVideoStateMachine(mediaFileHandle);
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
    ret = pthread_create(&media_demux_tid, NULL, media_demux_thread, (void*)this);
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

init_eout:

    return ;
}

CM_BOOL PlayerInner::player_engine_open()
{
    mediaFileHandle->open();

    // demux state machine push EVT_START
    //mediaDemuxStateMachineHandle->message_queue->




#if 0
    // TODO test
    int i = 0;
    for (i = 0; i < 100 ; i ++)
    {
        mediaDemuxStateMachineHandle->demux_2_packet_queue(mediaFileHandle);
    }
#endif

}



//-----------*******************-------------
//          Thread according to function
//-----------*******************-------------

/**
 * Thread 1 the corresponding Demux StateMachine
 */
void *media_demux_thread(void *arg)
{
    // first ,get PlayerInner Object Handle
    PlayerInner *playerInner = (PlayerInner *)arg;

    // call media demux thread
    playerInner->mediaDemuxStateMachineHandle->media_demux_thread(playerInner->mediaFileHandle);
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

