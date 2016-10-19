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

    this->message_queue = new XMessageQueue();

    // initialize the state
    this->state = STATE_DECODER_START;
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

void MediaDecodeVideoStateMachine::state_machine_change_state(player_state_e new_state)
{
    this->old_state = this->state;
    this->state = new_state;

}

/**
 * Main Work Thread ,the corresponding Video Decode StateMachine
 * decode video data thread.
 */
void * MediaDecodeVideoStateMachine::video_decode_thread(MediaFile *mediaFile)
{


    player_event_e evt;

    while(1){
        evt = this->message_queue->pop();
        XLog::d(ANDROID_LOG_WARN ,TAG ,"==>MediaDecodeVideoStateMachine msq evt = %d\n" ,evt);

        // Exit thread until receive the EXIT_THREAD EVT
        if(evt == EVT_EXIT_THREAD)
        {
            break;
        }

        // process others evt
        video_decode_state_machine_process_event(evt);

    }

}

// TODO
void MediaDecodeVideoStateMachine::video_decode_state_machine_process_event(player_event_e evt)
{
    switch(this->state)
    {

        case STATE_DECODER_START:
        {
            //el_demux_file_do_process_playing(evt);
            do_process_video_decode_start(evt);
            return;
        }
        case STATE_DECODER_WORK:
        {
            //el_demux_file_do_process_play_file_end(evt);
            return;
        }
        default:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeVideoStateMachine Invalid state!\n");
            return;
        }
    }
}

void MediaDecodeVideoStateMachine::do_process_video_decode_start(player_event_e evt)
{
    switch(evt)
    {
        case EVT_START:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeVideoStateMachine recv start event,goto work state!\n");
            this->state_machine_change_state(STATE_DECODER_WORK);
            this->message_queue->push(EVT_DECODE_GO_ON);
            return;
        }
        default:
        {
            return;
        }
    }

}


void MediaDecodeVideoStateMachine::do_process_video_decode_work(player_event_e evt)
{
    switch(evt)
    {
        case EVT_DECODE_GO_ON:
        {
            XLog::d(ANDROID_LOG_WARN ,TAG ,"== MediaDecodeVideoStateMachine recv EVT_DECODE_GO_ON event!\n");

            return;
        }
        default:
        {
            return;
        }
    }

}