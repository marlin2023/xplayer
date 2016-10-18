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
}


MediaDecodeVideoStateMachine::~MediaDecodeVideoStateMachine()
{

}

void MediaDecodeVideoStateMachine::decode_one_video_packet(AVPacket *packet)
{

    int send_result;
    AVFrame *video_decode_frame;
    AVCodecContext *video_codec_context ;
    FrameQueue *video_frame_queue;

    video_codec_context = mediaFileHandle->video_codec_context;
    video_frame_queue = mediaFileHandle->video_frame_queue;

    do {
        send_result = avcodec_send_packet(video_codec_context, packet);

#if 0
        while ( avcodec_receive_frame(video_codec_context, video_decode_frame) == 0 ) {

          video_frame_queue->put(video_decode_frame);
          XLog::d(ANDROID_LOG_INFO ,TAG ,"==>Video_Frame_QUEUE size=%d\n" ,video_frame_queue->size());

        }
#endif

    } while (send_result == AVERROR(EAGAIN));

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
        for(i = 0; i < 100 ; i ++){

            XLog::d(ANDROID_LOG_INFO ,TAG ,"==>MediaDecodeVideoStateMachine LOOP=%d\n" ,i);
            if(media_file_handle->video_queue->get(&pkt ,0) > 0){
                decode_one_video_packet(&pkt);
                j ++;
                if(j > 10){break;}
            }else{
                sleep(1);
            }

        }
        break;

    }

}