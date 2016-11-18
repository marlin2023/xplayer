//
// Created by chris on 10/14/16.
// refer to
// https://github.com/lu-zero/libbmd/blob/e6c53645bfd6d5e422f12fb56f071806c6f69bb2/src/bmdcapture.c
//

#define TAG "FFMpegPacketQueue"

#include "PacketQueue.h"
#include "MediaFile.h"
#include "xplayer_android_def.h"

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
    //pthread_mutex_lock(&mutex);
    int nb_pkt = nb_packets;
    //pthread_mutex_unlock(&mutex);

    return nb_pkt;
}

void PacketQueue::flush()
{
    AVPacketList *pkt, *pkt1;
    pthread_mutex_lock(&mutex);

    for(pkt = first_packet; pkt != NULL; pkt = pkt1) {
        pkt1 = pkt->next;
        av_packet_unref(&pkt->pkt);
        //XLog::e(TAG ,"==>av_free packet ....\n");
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

    pkt1->pkt = *pkt;
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
            // block
            notify_buffering_start();
            //
            ret = 0;
            break;
        } else {
            notify_buffering_start();
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

int64_t PacketQueue::get_buffer_packet_ts()
{
    int64_t nfirst = 0;
    int64_t nlast = 0;
    nfirst = get_first_pkt_pts();
    nlast = get_last_pkt_pts();
    return ((nlast-nfirst > 0) ? (nlast-nfirst) : 0);
}

int64_t PacketQueue::get_first_pkt_pts()
{
    int64_t pts = 0;
    AVPacketList *pkt1;

    pthread_mutex_lock(&mutex);

    pkt1 = first_packet;
    if (pkt1) {
        //pts = pkt1->pkt.pts;
        //if(pts == AV_NOPTS_VALUE){
        //    pts = pkt1->pkt.dts;
        //}
        // DTS Monotonic Increase ,But PTS not.
        pts = pkt1->pkt.dts;
        if(pts == AV_NOPTS_VALUE){
            pts = pkt1->pkt.pts;
        }
    }else {
      // set pts 0
        pts = 0;
    }

    pthread_mutex_unlock(&mutex);

    return pts;
}

int64_t PacketQueue::get_last_pkt_pts()
{
    int64_t pts = 0;
    AVPacketList *pkt1;

    pthread_mutex_lock(&mutex);

    pkt1 = last_packet;
    if (pkt1) {
        //pts = pkt1->pkt.pts;
        //if(pts == AV_NOPTS_VALUE){
            pts = pkt1->pkt.dts;
        //}
    }else {
      // set pts 0
        pts = 0;
    }

    pthread_mutex_unlock(&mutex);

    return pts;
}

void PacketQueue::notify_buffering_start()
{

    MediaFile *mediaFileHandle = (MediaFile *)empty_param;

    if(mediaFileHandle->isBuffering){
        XLog::e(TAG ,"==>in notify_buffering_start function ,but is in Buffering ,then return.\n");
        return;
    }

    XLog::e(TAG ,"==>in notify_buffering_start function PacketQueue.\n");
    if(mediaFileHandle->end_of_file){
        XLog::e(TAG ,"==>in notify_buffering_start function ,is the end of file..\n");
        return ;
    }

    bool isAudioPacketQueueEmpty = false;
    bool isVideoPacketQueueEmpty = false;
#if 1
    if(mediaFileHandle->audio_queue->size() == 0){
        isAudioPacketQueueEmpty = true;
        XLog::e(TAG ,"==>in notify_buffering_start function PacketQueue ,isAudioPacketQueueEmpty is true.\n");
    }

    if(mediaFileHandle->video_queue->size() == 0){
        isVideoPacketQueueEmpty = true;
        XLog::e(TAG ,"==>in notify_buffering_start function PacketQueue ,isVideoPacketQueueEmpty is true.\n");
    }

    if(isAudioPacketQueueEmpty && isVideoPacketQueueEmpty){
        XLog::e(TAG ,"==>in PacketQueue::notify_buffering_start function ,notify buffering_start.MEDIA_INFO_BUFFERING_START\n");
        // stop render
        mediaFileHandle->stopRender();
        // change state to buffering state.
        mediaFileHandle->jNI2BufferState();

        // notify
        mediaFileHandle->notify(MEDIA_INFO ,MEDIA_INFO_BUFFERING_START ,0);
        mediaFileHandle->isBuffering = true;
        // notify engine to stop playing
        // 回调上层接口更及时？？？？TODO

    }
#endif
}