//
// Created by chris on 9/27/16.
// maintenance the message queue used in xplayer.
// the manager of the all the message queue.
//

#ifndef XPLAYER_XIPC_H
#define XPLAYER_XIPC_H

#include <stdint.h>
#include "XMessageQueue.h"

/**
 * message queue type for Inter-thread communication.
 *
 * This Design use msg_q_max to mark the number of message queues.
 */
typedef enum ipc_msg_q_name_e_
{
    msg_q_demux_file = 0,
    msg_q_decode_video,
    msg_q_decode_audio,
    msg_q_video_render,

    msg_q_max

} ipc_msg_q_name_e;

// TODO
class XIPC
{
public:
    XIPC();
    ~XIPC();

    /**
     * init function ,init all used message queue.
     */
    int init();

    /**
     * send a message to the specified queue
     */
    int ipc_send(int msg ,ipc_msg_q_name_e msg_q_name_e);

    /**
     * read a message from the message queue
     */
    int ipc_wait(ipc_msg_q_name_e msg_q_name_e ,int *msg ,int64_t *tm);

    /**
     * clear the specified ipc queue
     */
    void ipc_clear(ipc_msg_q_name_e msg_q_name_e);

private:

    XMessageQueue *ipc_msg_q[msg_q_max];

};

#endif //XPLAYER_XIPC_H
