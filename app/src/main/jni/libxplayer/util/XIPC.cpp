//
// Created by chris on 9/27/16.
//

#include "XIPC.h"

//TODO
XIPC::XIPC()
{

}

//TODO
XIPC::~XIPC()
{


}

//TODO
int XIPC::init()
{
    // init all ipc message queue used in xplayer.
    ipc_msg_q[msg_q_demux_file] = new XMessageQueue("msg_q_demux_file");
    ipc_msg_q[msg_q_decode_video] = new XMessageQueue("msg_q_decode_video");
    ipc_msg_q[msg_q_decode_audio] = new XMessageQueue("msg_q_decode_audio");
    ipc_msg_q[msg_q_video_render] = new XMessageQueue("msg_q_video_render");

    //

    return 0;
}

//TODO
int XIPC::ipc_wait(ipc_msg_q_name_e msg_q_name_e, int *msg, int64_t *tm)
{
    // first ,get according XMessageQueue object
    XMessageQueue *xMessageQueue = ipc_msg_q[msg_q_name_e];

    // then do others TODO



    return 0;
}
//TODO
int XIPC::ipc_send(int msg, ipc_msg_q_name_e msg_q_name_e)
{
    // first , new msg node


    // second ,get according XMessageQueue object
    XMessageQueue *xMessageQueue = ipc_msg_q[msg_q_name_e];


    // third ,set value and push into queue.


    return 0;
}

//TODO
void XIPC::ipc_clear(ipc_msg_q_name_e msg_q_name_e)
{

}