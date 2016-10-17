//
// Created by chris on 10/17/16.
//

#define TAG "Media_Demux_State_Machine"

#include "MediaDemuxStateMachine.h"
#include "util/XLog.h"

MediaDemuxStateMachine::MediaDemuxStateMachine()
{
    read_retry_count = 0;

}

MediaDemuxStateMachine::~MediaDemuxStateMachine()
{

}

int MediaDemuxStateMachine::demux_2_packet_queue(AVFormatContext *format_ctx)
{
    AVPacket packet;
    int ret;
    AVFormatContext *format_context = format_ctx;

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
    return add_packet_to_q(&packet);

}

int MediaDemuxStateMachine::add_packet_to_q(AVPacket *pkt)
{
    AVFormatContext *pFormatCtx;


        //struct data_queue *pq;
#if 0
        el_av_packet_qnode_t *pkt_node;
        int *st_index;

        pFormatCtx = g_media_file_obj.format_context;
        st_index = &g_media_file_obj.stream_index[0];


        // 视频包
        if (pkt->stream_index == st_index[AVMEDIA_TYPE_VIDEO])
        {

            pq = &g_media_file_obj.video_pkt_q;
        } // 音频包
        else if (pkt->stream_index == st_index[AVMEDIA_TYPE_AUDIO])
        {

            pq = &g_media_file_obj.audio_pkt_q;
        }
        else
        {
            // 没有用的packet，直接释放
            av_free_packet(pkt);

            return -1;
        }

        // 把数据放到pkt_node中
        pkt_node = el_allocate_packet_node();
        if(!pkt_node)
        {
            EL_DBG_LOG("decoder: read pkt alloc err!\n");
            return -1000;
        }

        pkt_node->packet = *pkt;

        // 为了后期free，先增加一个引用
        av_dup_packet(&pkt_node->packet);

        q_push(&(pkt_node->list), pq);
#endif
        return 0;


}