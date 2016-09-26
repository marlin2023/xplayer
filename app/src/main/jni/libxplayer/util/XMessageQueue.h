//
// Created by chris on 9/26/16.
// the message queue used to Inter-thread communication.
// have Several kinds of message queue:
//                                      1. demux thread corresponding message queue.
//                                      2. video decode thread corresponding message queue.
//                                      3. audio decode thread corresponding message queue.
//                                      4.
//
//

#ifndef XPLAYER_XMESSAGEQUEUE_H
#define XPLAYER_XMESSAGEQUEUE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


// 线程间通讯的几个队列
// 这种设计很巧妙，用enum的最大数目，来作为下面数组的长度
typedef enum _el_ipc_msg_q_name_e_
{
    el_msg_q_demux_file = 0,
    el_msg_q_decode_video,
    el_msg_q_decode_audio,
    el_msg_q_video_sync,

    el_msg_q_max

} el_ipc_msg_q_name_e;

// TODO: ??
typedef struct _el_ipc_msg_node_t_
{
    unsigned long       msg;    // 消息内容
    int64_t             tm;     // 时间戳
    struct list_node    list;   // 消息队列
} el_ipc_msg_node_t;





#ifdef __cplusplus

}
#endif

#endif //XPLAYER_XMESSAGEQUEUE_H
