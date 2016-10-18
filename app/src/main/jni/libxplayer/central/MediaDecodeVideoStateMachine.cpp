//
// Created by chris on 10/17/16.
//
// video decoder Refer to
// https://github.com/scp-fs2open/fs2open.github.com/blob/a410654575f1818871e98b8f687b457322662689/code/cutscene/ffmpeg/VideoDecoder.cpp
//

#define TAG "Media_Decode_Video_State_Machine"

#include "MediaDecodeVideoStateMachine.h"

#include "util/XLog.h"

MediaDecodeVideoStateMachine::MediaDecodeVideoStateMachine()
{
    // init queue
    video_frame_queue = new FrameQueue();
}


MediaDecodeVideoStateMachine::~MediaDecodeVideoStateMachine()
{

}

void MediaDecodeVideoStateMachine::decode_one_video_packet(AVPacket *packet ,MediaFile *mediaFile)
{

    int send_result;
    AVFrame *video_decode_frame;
    AVCodecContext *video_codec_context ;
    video_codec_context = mediaFile->video_codec_context;

    do {
        send_result = avcodec_send_packet(video_codec_context, packet);

        while ( avcodec_receive_frame(video_codec_context, video_decode_frame) == 0 ) {

          video_frame_queue->put(video_decode_frame);
          XLog::d(ANDROID_LOG_INFO ,TAG ,"==>Video_Frame_QUEUE size=\n" ,video_frame_queue->size());

        }

    } while (send_result == AVERROR(EAGAIN));

}