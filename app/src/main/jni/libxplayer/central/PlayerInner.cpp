//
// Created by chris on 9/29/16.
//

#include "PlayerInner.h"
#include "util/XLog.h"

#define TAG "PLAYER_INNER"


PlayerInner::PlayerInner()
{
    // TODO
    mediaFileHandle = new MediaFile();

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
}
