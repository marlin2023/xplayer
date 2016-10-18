//
// Created by chris on 10/17/16.
//
// audio decoder Refer to
// https://github.com/scp-fs2open/fs2open.github.com/blob/a410654575f1818871e98b8f687b457322662689/code/cutscene/ffmpeg/AudioDecoder.cpp
//


#define TAG "Media_Decode_Video_State_Machine"

#include "MediaDecodeAudioStateMachine.h"
#include "util/XLog.h"

MediaDecodeAudioStateMachine::MediaDecodeAudioStateMachine()
{
    // init queue
    audio_frame_queue = new FrameQueue();
}


MediaDecodeAudioStateMachine::~MediaDecodeAudioStateMachine()
{

}


void MediaDecodeAudioStateMachine::decode_one_audio_packet(AVPacket *packet ,MediaFile *mediaFile)
{

    int send_result;
    AVFrame *decode_frame;
    AVCodecContext *audio_codec_context ;
    audio_codec_context = mediaFile->audio_codec_context;

    do {
        send_result = avcodec_send_packet(audio_codec_context, packet);

        while ( avcodec_receive_frame(audio_codec_context, decode_frame) == 0 ) {

          audio_frame_queue->put(decode_frame);
          XLog::d(ANDROID_LOG_INFO ,TAG ,"==>Audio_Frame_QUEUE size=\n" ,audio_frame_queue->size());

        }

    } while (send_result == AVERROR(EAGAIN));
}