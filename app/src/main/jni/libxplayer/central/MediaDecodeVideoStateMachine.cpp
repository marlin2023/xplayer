//
// Created by chris on 10/17/16.
//
// video decoder Refer to
// https://github.com/scp-fs2open/fs2open.github.com/blob/a410654575f1818871e98b8f687b457322662689/code/cutscene/ffmpeg/VideoDecoder.cpp
//

#define TAG "Media_Decode_Video_State_Machine"

#include <unistd.h>

#include "MediaDecodeVideoStateMachine.h"

#include "util/XLog.h"

MediaDecodeVideoStateMachine::MediaDecodeVideoStateMachine(MediaFile *mediaFile)
{
    this->mediaFileHandle = mediaFile;

    message_queue = new XMessageQueue();
}


MediaDecodeVideoStateMachine::~MediaDecodeVideoStateMachine()
{

}

void MediaDecodeVideoStateMachine::decode_one_video_packet(AVPacket *packet)
{

    int send_result;
    AVFrame *frame = av_frame_alloc();;
    AVCodecContext *video_codec_context ;
    FrameQueue *video_frame_queue;
    int ret ;
    video_codec_context = mediaFileHandle->video_codec_context;
    video_frame_queue = mediaFileHandle->video_frame_queue;

    do {
        XLog::d(ANDROID_LOG_INFO ,TAG ,"==>1\n" );
        send_result = avcodec_send_packet(video_codec_context, packet);
        av_packet_unref(packet);
        XLog::d(ANDROID_LOG_INFO ,TAG ,"==>2\n" );

        while ( (ret =avcodec_receive_frame(video_codec_context, frame)) == 0 ) {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"==>3\n" );
            video_frame_queue->put(frame);
            av_frame_unref(frame);
            XLog::d(ANDROID_LOG_INFO ,TAG ,"==>Video_Frame_QUEUE size=%d\n" ,video_frame_queue->size());

        }
        XLog::d(ANDROID_LOG_INFO ,TAG ,"==>ret = %d\n" ,ret );


    } while (send_result == AVERROR(EAGAIN));

    av_frame_free(&frame);
}


/**
 * Main Work Thread ,the corresponding Video Decode StateMachine
 */
void * MediaDecodeVideoStateMachine::video_decode_thread(MediaFile *mediaFile)
{

    MediaFile *media_file_handle = mediaFile;
    AVPacket pkt;
    //TODO
    while(1)
    {

        int i ;
        int j = 0;
        for(i = 0; i < 10 ; i ++){

            XLog::d(ANDROID_LOG_INFO ,TAG ,"==>MediaDecodeVideoStateMachine LOOP=%d\n" ,i);
        }
        break;

    }

}