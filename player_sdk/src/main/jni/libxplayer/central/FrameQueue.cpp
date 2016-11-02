//
// Created by chris on 10/14/16.
//

#define TAG "FFMpegFrameQueue"

#include "FrameQueue.h"
#include "util/XLog.h"

FrameQueue::FrameQueue()
{
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    first_frame = NULL;
    last_frame = NULL;
    node_count = 0;
}

FrameQueue::FrameQueue(int max_node_count)
{
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    first_frame = NULL;
    last_frame = NULL;
    node_count = 0;
    //
    this->max_node_count = max_node_count;
}

FrameQueue::~FrameQueue()
{
    flush();
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

}

void FrameQueue::flush()
{
    AVFrameList *frame, *frame1;
    pthread_mutex_lock(&mutex);

    for(frame = first_frame; frame != NULL; frame = frame1) {
        frame1 = frame->next;
        av_frame_free(&frame->frame);
        av_free(frame);
    }

    last_frame = NULL;
    first_frame = NULL;
    node_count = 0;

    pthread_mutex_unlock(&mutex);
}


int FrameQueue::put(AVFrame *frame)
{
   AVFrameList *frame1;
    int ret;

    frame1 = (AVFrameList *) av_malloc(sizeof(AVFrameList));
    frame1->frame = av_frame_alloc();
    if (!frame1 || !frame1->frame){
        XLog::e(TAG ,"==>av_malloc failed.\n");
        return AVERROR(ENOMEM);
    }

    if ( (ret = av_frame_ref(frame1->frame, frame) ) < 0) { //// copy frame data (only copy meta data .//TODO)
        av_free(frame1);
        XLog::e(TAG ,"==>av_frame_ref failed.\n");
        return ret;
    }

    av_frame_unref(frame);
    //*(frame1->frame) = *frame;
    frame1->next = NULL;

    pthread_mutex_lock(&mutex);

    if (!last_frame) {
        first_frame = frame1;
    }
    else {
        last_frame->next = frame1;
    }

    last_frame = frame1;
    node_count ++;

    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    return 0;
}


int FrameQueue::get(AVFrame *frame)
{

    AVFrameList *frame1;
    int ret;

    pthread_mutex_lock(&mutex);
    for(;;) {

        frame1 = first_frame;
        if (frame1) {
            first_frame = frame1->next;
            if (!first_frame){
                last_frame = NULL;
            }

            node_count --;
            av_frame_ref(frame, frame1->frame); // set frame
            av_frame_unref(frame1->frame);
            av_frame_free(&frame1->frame);
            av_free(frame1);

            ret = 1;
            break;
        } else {
            pthread_cond_wait(&cond, &mutex);
        }

    }
    pthread_mutex_unlock(&mutex);

    return ret;

}

int FrameQueue::size()
{
    //pthread_mutex_lock(&mutex);
    int nb_frame = node_count;
    //pthread_mutex_unlock(&mutex);

    return nb_frame;
}