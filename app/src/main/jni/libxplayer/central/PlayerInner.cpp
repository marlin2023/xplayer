//
// Created by chris on 9/29/16.
//

#include "PlayerInner.h"
#include "util/XLog.h"

#define TAG "PLAYER_INNER"


PlayerInner::PlayerInner()
{
    // TODO
    // mediaFile Handle
    mediaFileHandle = new MediaFile();

    // mediaDemuxStateMachine
    mediaDemuxStateMachineHandle = new MediaDemuxStateMachine();

}

/**
 * player central engine init.
 */
void PlayerInner::player_engine_init()
{
    // Register all formats and codecs
    av_register_all();
    avformat_network_init();
    //
}

CM_BOOL PlayerInner::player_engine_open()
{
    mediaFileHandle->open();

    // TODO test
    int i = 0;
    for (i = 0; i < 100 ; i ++)
    {
        mediaDemuxStateMachineHandle->demux_2_packet_queue(mediaFileHandle);
    }

}
