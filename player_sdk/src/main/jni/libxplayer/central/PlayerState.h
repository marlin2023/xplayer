//
// Created by chris on 10/18/16.
// Video Player State Status
//

#ifndef XPLAYER_PLAYERSTATE_H
#define XPLAYER_PLAYERSTATE_H

/* video player state status*/
typedef enum _player_state_e_
{
    STATE_UNKNOWN = -1,

    STATE_IDLE ,
    STATE_INITIALIZED ,
    STATE_PREPARED ,
    STATE_BUFFERING,
    STATE_PLAY_WAIT,                // Buffering End then go into PLAY_WAIT.
    STATE_PLAY_PLAYING,
    STATE_PLAY_PAUSED,              // puased
    STATE_PLAY_FILE_END ,
    STATE_PLAY_COMPLETE,
    STATE_STOPPED,                  // STOPPED
    STATE_SEEK_WAIT,                // 搜寻暂停



    /* state below is used for decoding state machine*/
    STATE_DECODER_START,
    STATE_DECODER_WAIT,
    STATE_DECODER_WORK,
    STATE_DECODE_SEEK_WAIT ,

    STATE_END,                    // the end
} player_state_e;



#endif //XPLAYER_PLAYERSTATE_H
