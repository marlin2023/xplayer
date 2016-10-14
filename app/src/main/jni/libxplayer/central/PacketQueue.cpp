//
// Created by chris on 10/14/16.
// refer to
// https://github.com/lu-zero/libbmd/blob/e6c53645bfd6d5e422f12fb56f071806c6f69bb2/src/bmdcapture.c
//

#define TAG "FFMpegPacketQueue"


#include <android/log.h>
#include "PacketQueue.h"

#include "util/XLog.h"

PacketQueue::PacketQueue()
{
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    first_packet = NULL;
    last_packet = NULL;
    nb_packets = 0;;
    q_size = 0;
    abort_request = false;
}

PacketQueue::~PacketQueue()
{
    flush();
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}

int PacketQueue::size()
{
    pthread_mutex_lock(&mutex);
    int nb_pkt = nb_packets;
    pthread_mutex_unlock(&mutex);

    return nb_pkt;
}

void PacketQueue::flush()
{
    AVPacketList *pkt, *pkt1;
    pthread_mutex_lock(&mutex);

    for(pkt = first_packet; pkt != NULL; pkt = pkt1) {
        pkt1 = pkt->next;
        av_packet_unref(&pkt->pkt);
        av_freep(&pkt);
    }

    last_packet = NULL;
    first_packet = NULL;
    nb_packets = 0;
    q_size = 0;

    pthread_mutex_unlock(&mutex);
}

int PacketQueue::put(AVPacket* pkt)
{
    AVPacketList *pkt1;
    int ret;

    pkt1 = (AVPacketList *) av_malloc(sizeof(AVPacketList));
    if (!pkt1){
        XLog::e(TAG ,"==>av_malloc failed.\n");
        return AVERROR(ENOMEM);
    }

    if ( (ret = av_packet_ref(&pkt1->pkt, pkt) ) < 0) {
        av_free(pkt1);
        XLog::e(TAG ,"==>av_packet_ref failed.\n");
        return ret;
    }
    pkt1->next = NULL;

    pthread_mutex_lock(&mutex);

    if (!last_packet) {
        first_packet = pkt1;
    }
    else {
        last_packet->next = pkt1;
    }

    last_packet = pkt1;
    nb_packets ++;
    q_size += pkt1->pkt.size + sizeof(*pkt1);

    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    return 0;

}

/* return < 0 if aborted, 0 if no packet and > 0 if packet.  */
int PacketQueue::get(AVPacket *pkt, bool block)
{
    AVPacketList *pkt1;
    int ret;

    pthread_mutex_lock(&mutex);

    for(;;) {
        if (abort_request) {
            XLog::e(TAG ,"==>abort.\n");
            ret = -1;
            break;
        }

        pkt1 = first_packet;
        if (pkt1) {
            first_packet = pkt1->next;
            if (!first_packet){
                last_packet = NULL;
            }

            nb_packets --;
            q_size -= pkt1->pkt.size + sizeof(*pkt1);
            *pkt = pkt1->pkt;
            av_free(pkt1);
            ret = 1;
            break;
        } else if (!block) {
            ret = 0;
            break;
        } else {
            pthread_cond_wait(&cond, &mutex);
        }

    }
    pthread_mutex_unlock(&mutex);
    return ret;

}

void PacketQueue::abort()
{
    pthread_mutex_lock(&mutex);
    abort_request = true;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}