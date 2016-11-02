//
// Created by chris on 10/18/16.
// message type stored in MessageQueue.
//
//

#ifndef XPLAYER_XMESSAGETYPE_H
#define XPLAYER_XMESSAGETYPE_H

typedef enum _player_event_e_
{

    EVT_GO_ON = 0,              // not external event

    EVT_DECODE_GO_ON ,
    EVT_VIDEO_SYNC_GO_ON ,

    EVT_OPEN ,
    EVT_CLOSE,
    EVT_START,
    EVT_PLAY ,                  // play
    EVT_PAUSE,
    EVT_RESUME ,                  // resume after pause
    EVT_SEEK_PAUSE,
    EVT_SEEK,
    EVT_READY_TO_SEEK,
    EVT_SEEK_DONE,
    EVT_STOP,
    EVT_BUFFERING,
    EVT_STOP_DECODE_WAIT,
    EVT_RESTART,
    EVT_PLAY_COMPLETE,
    EVT_EXIT_THREAD,
    EVT_END

} player_event_e;



#endif //XPLAYER_XMESSAGETYPE_H
