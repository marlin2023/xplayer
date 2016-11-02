//
// Created by chris on 10/17/16.
//
// audio decoder Refer to
// https://github.com/scp-fs2open/fs2open.github.com/blob/a410654575f1818871e98b8f687b457322662689/code/cutscene/ffmpeg/AudioDecoder.cpp
//


#define TAG "Media_Decode_Audio_State_Machine"

#include <unistd.h>

#include "MediaDecodeAudioStateMachine.h"
#include "util/XLog.h"

MediaDecodeAudioStateMachine::MediaDecodeAudioStateMachine(MediaFile *mediaFile)
{
    this->mediaFileHandle = mediaFile;

    this->message_queue = new XMessageQueue();

    // initialize the state
    this->state = STATE_DECODER_START;

}


MediaDecodeAudioStateMachine::~MediaDecodeAudioStateMachine()
{

}


void MediaDecodeAudioStateMachine::decode_one_audio_packet(AVPacket *packet )
{

    int send_result;
    AVFrame *frame;
    AVCodecContext *audio_codec_context ;
    FrameQueue *audio_frame_queue;

    frame = av_frame_alloc();

    audio_codec_context = mediaFileHandle->audio_codec_context;
    audio_frame_queue = mediaFileHandle->audio_frame_queue;

    do {
        send_result = avcodec_send_packet(audio_codec_context, packet);
        while ( avcodec_receive_frame(audio_codec_context, frame) == 0 ) {
          audio_frame_queue->put(frame);
        }

    } while (send_result == AVERROR(EAGAIN));

    av_frame_free(&frame);
    av_packet_unref(packet);
}

void MediaDecodeAudioStateMachine::state_machine_change_state(player_state_e new_state)
{
    this->old_state = this->state;
    this->state = new_state;

}

/**
 * Main Work Thread ,the corresponding Audio Decode StateMachine
 * decode audio data thread.
 */
void * MediaDecodeAudioStateMachine::audio_decode_thread()
{


    player_event_e evt;
    while(1){

        evt = this->message_queue->pop();
        //XLog::d(ANDROID_LOG_WARN ,TAG ,"==>MediaDecodeAudioStateMachine msq evt = %d\n" ,evt);

        // Exit thread until receive the EXIT_THREAD EVT
        if(evt == EVT_EXIT_THREAD)
        {
            break;
        }

        // process others evt
        audio_decode_state_machine_process_event(evt);

    }
}

// central engine
void MediaDecodeAudioStateMachine::audio_decode_state_machine_process_event(player_event_e evt)
{
    switch(this->state)
    {
        case STATE_DECODER_WAIT:
        {
            do_process_audio_decode_wait(evt);
            return;
        }
        case STATE_DECODER_START:
        {
            do_process_audio_decode_start(evt);
            return;
        }
        case STATE_DECODER_WORK:
        {
            do_process_audio_decode_work(evt);
            return;
        }

        default:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeAudioStateMachine Invalid state!\n");
            return;
        }
    }
}

void MediaDecodeAudioStateMachine::do_process_audio_decode_wait(player_event_e evt)
{
    switch(evt)
    {
        case EVT_STOP_DECODE_WAIT:
        {
            return;
        }
        case EVT_PLAY:
        {
            return;
        }

        case EVT_RESUME:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeAudioStateMachine thread recv resume evt!!\n");
            this->state_machine_change_state(STATE_DECODER_WORK);

            // step into decode loop logic, to make it loops, we send message selfly
            this->message_queue->push(EVT_DECODE_GO_ON);
            return;
        }

        default:
        {
            return;
        }
    }

}


void MediaDecodeAudioStateMachine::do_process_audio_decode_start(player_event_e evt)
{
    switch(evt)
    {
        case EVT_START:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeAudioStateMachine recv start event,goto work state!\n");
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


void MediaDecodeAudioStateMachine::do_process_audio_decode_work(player_event_e evt)
{
    switch(evt)
    {
        case EVT_DECODE_GO_ON:
        {
            //XLog::d(ANDROID_LOG_WARN ,TAG ,"== MediaDecodeAudioStateMachine recv EVT_DECODE_GO_ON event!\n");
            if(mediaFileHandle->audio_frame_queue->node_count >= mediaFileHandle->audio_frame_queue->max_node_count)
            {
                //audio_frame_q->full_func(audio_frame_q->full_parm);
                usleep(50000);
                this->message_queue->push(EVT_DECODE_GO_ON);
                return;
            }

            //
            AVPacket pkt;
            int ret = mediaFileHandle->audio_queue->get(&pkt ,1);
            int rr = mediaFileHandle->audio_queue->size();
            //XLog::d(ANDROID_LOG_WARN ,TAG ,"== MediaDecodeAudioStateMachine ,pkt.size = %d ,rr=%d ,ret =%d\n" ,pkt.size ,rr ,ret);

            decode_one_audio_packet(&pkt );
            this->message_queue->push(EVT_DECODE_GO_ON);

            return;
        }
        case EVT_PAUSE:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"decoder:mac = %d,audio decode paused!\n");
            this->state_machine_change_state(STATE_DECODER_WAIT);

            return;
        }

        case EVT_STOP:
        {
            this->state_machine_change_state(STATE_DECODER_START);
            XLog::d(ANDROID_LOG_INFO ,TAG ,"===>decoder:decode stoped!\n");
            return;
        }

        default:
        {
            return;
        }
    }

}