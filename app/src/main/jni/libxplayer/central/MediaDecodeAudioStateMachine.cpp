//
// Created by chris on 10/17/16.
//
// audio decoder Refer to
// https://github.com/scp-fs2open/fs2open.github.com/blob/a410654575f1818871e98b8f687b457322662689/code/cutscene/ffmpeg/AudioDecoder.cpp
//


#define TAG "Media_Decode_Video_State_Machine"

#include "MediaDecodeAudioStateMachine.h"
#include "util/XLog.h"

MediaDecodeAudioStateMachine::MediaDecodeAudioStateMachine(MediaFile *mediaFile)
{
    this->mediaFileHandle = mediaFile;

}


MediaDecodeAudioStateMachine::~MediaDecodeAudioStateMachine()
{

}


void MediaDecodeAudioStateMachine::decode_one_audio_packet(AVPacket *packet )
{

    int send_result;
    AVFrame *decode_frame;
    AVCodecContext *audio_codec_context ;
    FrameQueue *audio_frame_queue;

    audio_codec_context = mediaFileHandle->audio_codec_context;
    audio_frame_queue = mediaFileHandle->audio_frame_queue;

    do {
        send_result = avcodec_send_packet(audio_codec_context, packet);

        while ( avcodec_receive_frame(audio_codec_context, decode_frame) == 0 ) {

          audio_frame_queue->put(decode_frame);
          XLog::d(ANDROID_LOG_INFO ,TAG ,"==>Audio_Frame_QUEUE size=\n" ,audio_frame_queue->size());

        }

    } while (send_result == AVERROR(EAGAIN));
}


/**
 * Main Work Thread ,the corresponding Audio Decode StateMachine
 */
void * MediaDecodeAudioStateMachine::audio_decode_thread(void *arg)
{

}