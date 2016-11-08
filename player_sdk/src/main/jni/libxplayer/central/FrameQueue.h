//
// Created by chris on 10/14/16.
// AVFrame Queue .
// Refer to https://github.com/cmkyec/videoIO/blob/3af7c26ece6e4230c4470bbc21195e15f193390e/gentech_queue.h
//


#ifndef XPLAYER_FRAMEQUEUE_H
#define XPLAYER_FRAMEQUEUE_H

#include <pthread.h>

extern "C" {

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"

} // end of extern C


/**
* FrameQueue node ADT.
*/
typedef struct AVFrameList{

    AVFrame *frame;
    struct AVFrameList *next;
} AVFrameList;


class FrameQueue
{
public:
    FrameQueue();
    FrameQueue(int max_node_count);
    ~FrameQueue();

    /**
    * cleanup the frame queue.
    */
    void flush();

    /**
    * put frame into the packet queue.
    */
    int put(AVFrame *frame);

    /**
    * get avframe from the frame queue.
    */
    int get(AVFrame *frame);

    /**
    * get avframe from the frame queue.
    * return < 0 if aborted, 0 if no packet and > 0 if packet.
    */
    int get(AVFrame *frame , bool block);

    int size(); //nb_frames

    /**
    * current node count.
    */
    int node_count;

    /**
    * max node count.
    */
    int max_node_count; // upper limit node count

    /**
    * empty function parameter handle.
    */
    void *empty_param; //


private:

    void notify_buffering_start();

    /**
    * first frame in frame queue.
    */
    AVFrameList*   first_frame;

    /**
    * last frame in frame queue.
    */
    AVFrameList*   last_frame;

    unsigned long long q_size;

    int abort_request;

    pthread_mutex_t mutex;
    pthread_cond_t cond;
};
#endif //XPLAYER_FRAMEQUEUE_H
