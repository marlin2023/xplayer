//
// Created by chris on 10/17/16.
//

#define TAG "Media_Demux_State_Machine"

#include "MediaDemuxStateMachine.h"
#include "util/XLog.h"

MediaDemuxStateMachine::MediaDemuxStateMachine()
{
    read_retry_count = 0;

    // init message queue for media demux state machine.
    message_queue = new XMessageQueue();

}

MediaDemuxStateMachine::~MediaDemuxStateMachine()
{

}

int MediaDemuxStateMachine::demux_2_packet_queue(MediaFile *mediaFile)
{
    AVPacket packet;
    int ret;
    AVFormatContext *format_context = mediaFile->format_context;

    // read packet from stream
    if((ret = av_read_frame(format_context, &packet)) < 0)
    {
        read_retry_count++;
        XLog::e(TAG ,"av_read_frame return: %d\n", ret);
        return ret;
    }

    read_retry_count = 0;

    // here ,audio packet will be put into audio packet queue ;
    // video packet will be put into video packet queue.
    return add_packet_to_q(&packet ,mediaFile);

}

int MediaDemuxStateMachine::add_packet_to_q(AVPacket *pkt ,MediaFile *mediaFile)
{


    AVFormatContext *format_context;
    int *st_index;
    PacketQueue *pkt_q;

    format_context = mediaFile->format_context;
    st_index = &mediaFile->stream_index[0];



    if (pkt->stream_index == st_index[AVMEDIA_TYPE_VIDEO])
    {
        // video packet
        pkt_q = mediaFile->video_queue;
        XLog::d(ANDROID_LOG_INFO ,TAG ,"==>VVVVideo packet queue size = %d\n" ,pkt_q->size());
    }
    else if (pkt->stream_index == st_index[AVMEDIA_TYPE_AUDIO])
    {
        // audio packet
        pkt_q = mediaFile->audio_queue;
        XLog::d(ANDROID_LOG_INFO ,TAG ,"==>AAAAudio packet queue size = %d\n" ,pkt_q->size());
    }
    else
    {
        // no used packet ,release directly.
        av_packet_unref(pkt);   //av_free_packet(pkt);
        return -1;
    }

    // Entry queue
    int ret = pkt_q->put(pkt);
    return ret;
}


void MediaDemuxStateMachine::media_demux_thread(MediaFile *mediaFile)
{

}