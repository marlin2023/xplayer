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

#include <list>
#include "XMutexLock.h"
#include "XMessageType.h"

using namespace std;


// queue block mode
typedef enum _q_node_mode_e_
{
    el_q_mode_unblock = 0x0,
    el_q_mode_block_empty = 0x1,
    el_q_mode_block_full = 0x2,
    el_q_mode_max
} q_node_mode_e;


#if 0
/**
 * Message Queue Node ADT
 */
typedef struct _ipc_msg_node_t_
{
    /**
     * message content
     */
    unsigned long       msg;

    /**
     * message timestamp
     */
    int64_t             tm;

    //struct list_node    list;   // 消息队列
}ipc_msg_node_t;

#endif


/**
 * Message Queue ADT
 */
class XMessageQueue{

public:

    XMessageQueue();
    XMessageQueue(char *name);
    ~XMessageQueue();

    /**
     * init function
     */
    int init();

    /**
     * add Elements at the front of queue.
     * push_front
     */
    int push_front(player_event_e evt_type);

    /**
     * add Elements at the end of queue.
     * push_back
     */
    int push(player_event_e evt_type);

    /**
     * pop Elements at the front of queue.
     * pop_front
     */
    player_event_e pop();

    /**
     * if the queue has no element return true.
     */
    bool empty();

    /**
     * return the element number in the queue.
     */
    int size();


private:

    /**
     * message queue
     * empty queue.
     */
    list<player_event_e> msg_queue;

    /**
     * message queue name
     */
    char *msg_queue_name;


    /**
     * current node count
     */
    int node_count_current;

    /**
     * upper limit node count
     */
    int node_count_max;

    /**
     * queue block mode.
     */
    q_node_mode_e   mode;   // once this mode value is set, you should never change it during the q's life


    /**
     * mutex for current MessageQueue Object
     */
    pthread_mutex_t mutexLock;

    // if empty, blocked
    pthread_cond_t  has_node;

    // if full, blocked
    pthread_cond_t  has_space;

};



#endif //XPLAYER_XMESSAGEQUEUE_H
