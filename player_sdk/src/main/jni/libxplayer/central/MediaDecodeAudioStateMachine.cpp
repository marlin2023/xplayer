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

    this->mediaFileHandle->message_queue_audio_decode = new XMessageQueue();

    // initialize the state
    this->state = STATE_DECODER_START;

}


MediaDecodeAudioStateMachine::~MediaDecodeAudioStateMachine()
{
    if(this->mediaFileHandle->message_queue_audio_decode){
        delete this->mediaFileHandle->message_queue_audio_decode;
        this->mediaFileHandle->message_queue_audio_decode   = NULL;
    }

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

        evt = this->mediaFileHandle->message_queue_audio_decode->pop();
        //XLog::d(ANDROID_LOG_WARN ,TAG ,"==>MediaDecodeAudioStateMachine msq evt = %d\n" ,evt);

        // Exit thread until receive the EXIT_THREAD EVT
        if(evt == EVT_EXIT_THREAD)
        {
            XLog::d(ANDROID_LOG_WARN ,TAG ,"==>MediaDecodeAudioStateMachine msq evt = EVT_EXIT_THREAD\n" );
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
        case STATE_DECODE_SEEK_WAIT:
        {
            do_process_audio_decode_seek_wait(evt);
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
        //{
        //    return;
        //}
        case EVT_START:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeAudioStateMachine thread recv EVT_START evt!!\n");
            this->state_machine_change_state(STATE_DECODER_WORK);
            this->mediaFileHandle->message_queue_audio_decode->push(EVT_DECODE_GO_ON);
            return;
        }
        case EVT_RESUME:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeAudioStateMachine thread recv resume evt!!\n");
            this->state_machine_change_state(STATE_DECODER_WORK);

            // step into decode loop logic, to make it loops, we send message selfly
            this->mediaFileHandle->message_queue_audio_decode->push(EVT_DECODE_GO_ON);
            return;
        }
        case EVT_STOP:
        {
            this->state_machine_change_state(STATE_DECODER_START);
            return;
        }

        case EVT_SEEK:
        {

            this->state_machine_change_state(STATE_DECODE_SEEK_WAIT);  // change state.
            return;
        }

        default:
        {
            XLog::e(TAG ,"===>AUDIO_DECODE_WAIT receive others EVT:%d\n" ,evt);
            return;
        }
    }

}


void MediaDecodeAudioStateMachine::do_process_audio_decode_start(player_event_e evt)
{
    switch(evt)
    {
        case EVT_RESUME:
        case EVT_START:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeAudioStateMachine recv start event,goto work state!\n");
            this->state_machine_change_state(STATE_DECODER_WORK);
            this->mediaFileHandle->message_queue_audio_decode->push(EVT_DECODE_GO_ON);
            return;
        }
        case EVT_SEEK:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"===>decoder audio start state: receive EVT_SEEK\n");
            this->state_machine_change_state(STATE_DECODE_SEEK_WAIT);  // change state.
            return;
        }

        default:
        {
            XLog::e(TAG ,"===>AUDIO_DECODE_START receive others EVT:%d\n" ,evt);
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
            if(mediaFileHandle->audio_frame_queue->node_count >= mediaFileHandle->audio_frame_queue->max_node_count)
            {
                //XLog::d(ANDROID_LOG_WARN ,TAG ,"== MediaDecodeAudioStateMachine recv EVT_DECODE_GO_ON event! 11\n");
                usleep(50000);
                this->mediaFileHandle->message_queue_audio_decode->push(EVT_DECODE_GO_ON);
                return;
            }

            //
            AVPacket pkt;
            int ret = mediaFileHandle->audio_queue->get(&pkt ,0);   // non block
            if(ret == 0){   // get no packet.
                if(mediaFileHandle->end_of_file){ // end of file
                    XLog::d(ANDROID_LOG_INFO ,TAG ,"audio decoder statemachine ,in the end of file and change state to STATE_DECODER_START!\n");
                    this->state_machine_change_state(STATE_DECODER_START);  // change state.

                }else{
                    XLog::d(ANDROID_LOG_INFO ,TAG ,"audio decoder statemachine , not in the end of file ,may will be in buffering,Audio packet q size =%d\n" ,mediaFileHandle->audio_queue->size());
                    usleep(50000);
                    this->mediaFileHandle->message_queue_audio_decode->push(EVT_DECODE_GO_ON);
                }
                return;
            }

            decode_one_audio_packet(&pkt );
            this->mediaFileHandle->message_queue_audio_decode->push(EVT_DECODE_GO_ON);

            return;
        }
        case EVT_PAUSE:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"audio decoder statemachine:mac = %d,audio decode paused!\n");
            this->state_machine_change_state(STATE_DECODER_WAIT);

            return;
        }

        case EVT_STOP:
        {
            this->state_machine_change_state(STATE_DECODER_START);
            XLog::d(ANDROID_LOG_INFO ,TAG ,"===>decoder:decode stoped!\n");
            return;
        }

        case EVT_SEEK:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"===>decoder audio: receive EVT_SEEK\n");
            this->state_machine_change_state(STATE_DECODE_SEEK_WAIT);  // change state.
            return;
        }
        default:
        {
            XLog::e(TAG ,"===>AUDIO_DECODE_WORK receive others EVT:%d\n" ,evt);
            return;
        }
    }

}

void MediaDecodeAudioStateMachine::do_process_audio_decode_seek_wait(player_event_e evt)
{
    switch(evt)
    {
        case EVT_SEEK_DONE:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeAudioStateMachine recv EVT_SEEK_DONE!\n");
            this->state_machine_change_state(STATE_DECODER_WAIT);
            return;
        }
        case EVT_SEEK_PAUSE:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeVideoStateMachine recv EVT_SEEK_PAUSE!\n");

            return;
        }
        case EVT_START:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeAudioStateMachine recv start event,goto work state!\n");
            this->state_machine_change_state(STATE_DECODER_WORK);
            this->mediaFileHandle->message_queue_audio_decode->push(EVT_DECODE_GO_ON);

            return;
        }
        default:
        {
            XLog::e(TAG ,"===>AUDIO_DECODE_SEEK_WAIT receive others EVT:%d\n" ,evt);
            return;
        }
    }

}