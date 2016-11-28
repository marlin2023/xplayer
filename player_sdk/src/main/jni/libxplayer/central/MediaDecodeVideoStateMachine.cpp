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
#include "xplayer_android_def.h"

MediaDecodeVideoStateMachine::MediaDecodeVideoStateMachine(MediaFile *mediaFile)
{
    this->mediaFileHandle = mediaFile;

    this->mediaFileHandle->message_queue_video_decode = new XMessageQueue();

    // initialize the state
    this->state = STATE_DECODER_START;


}

MediaDecodeVideoStateMachine::~MediaDecodeVideoStateMachine()
{
    if(this->mediaFileHandle->message_queue_video_decode){
        delete this->mediaFileHandle->message_queue_video_decode;
        this->mediaFileHandle->message_queue_video_decode = NULL;
    }
}


void MediaDecodeVideoStateMachine::decode_one_video_packet(AVPacket *packet1)
{

    int send_result;
    AVFrame *frame;
    AVCodecContext *codec_ctx ;
    FrameQueue *frame_queue;

    frame = av_frame_alloc();
    if(!frame){
        XLog::e(TAG ,"===>decode_one_video_packet, error for av_frame_alloc.\n");
        this->mediaFileHandle->notify(MEDIA_ERROR ,MEDIA_ERROE_MEM, CM_FALSE);
        return;
    }

    codec_ctx = mediaFileHandle->video_codec_context;
    frame_queue = mediaFileHandle->video_frame_queue;

    do {
        send_result = avcodec_send_packet(codec_ctx, packet1);
        while ( avcodec_receive_frame(codec_ctx, frame) == 0 ) {

            frame->pts = av_frame_get_best_effort_timestamp(frame);
            mediaFileHandle->video_frame_queue->put(frame);
        }

    } while (send_result == AVERROR(EAGAIN));

    av_frame_free(&frame);
    av_packet_unref(packet1);
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
        evt = this->mediaFileHandle->message_queue_video_decode->pop();
        //XLog::d(ANDROID_LOG_WARN ,TAG ,"==>MediaDecodeVideoStateMachine msq evt = %d\n" ,evt);

        // Exit thread until receive the EXIT_THREAD EVT
        if(evt == EVT_EXIT_THREAD)
        {
            XLog::d(ANDROID_LOG_WARN ,TAG ,"==>MediaDecodeVideoStateMachine msq evt = EVT_EXIT_THREAD\n" );
            break;
        }

        // process others evt
        video_decode_state_machine_process_event(evt);

    }

}

// central engine .
void MediaDecodeVideoStateMachine::video_decode_state_machine_process_event(player_event_e evt)
{
    switch(this->state)
    {
        case STATE_DECODER_WAIT:
        {
            do_process_video_decode_wait(evt);
            return;
        }

        case STATE_DECODER_START:
        {
            do_process_video_decode_start(evt);
            return;
        }
        case STATE_DECODER_WORK:
        {
            do_process_video_decode_work(evt);
            return;
        }

        case STATE_DECODE_SEEK_WAIT:
        {
            do_process_video_decode_seek_wait(evt);
            return;
        }
        default:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeVideoStateMachine Invalid state!\n");
            return;
        }
    }
}

void MediaDecodeVideoStateMachine::do_process_video_decode_wait(player_event_e evt)
{
    XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeVideoStateMachine thread recv  evt %d!!\n" ,evt);
    switch(evt)
    {
        case EVT_STOP_DECODE_WAIT:
        {
            return;
        }
        case EVT_PLAY:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeVideoStateMachine thread decode_wait state recv EVT_PLAY evt!!\n");
        }
        case EVT_START:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeVideoStateMachine thread decode_wait state recv EVT_START evt!!\n");
            this->state_machine_change_state(STATE_DECODER_WORK);
            this->mediaFileHandle->message_queue_video_decode->push(EVT_DECODE_GO_ON);
            return;
        }
        case EVT_RESUME:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeVideoStateMachine thread decode_wait state recv EVT_RESUME evt!!\n");
            this->state_machine_change_state(STATE_DECODER_WORK);
            // step into decode loop logic, to make it loops, we send message selfly
            this->mediaFileHandle->message_queue_video_decode->push(EVT_DECODE_GO_ON);
            return;
        }
        case EVT_STOP:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeVideoStateMachine thread decode_wait state recv EVT_STOP evt!!\n");
            this->state_machine_change_state(STATE_DECODER_START);
            return;
        }
        case EVT_SEEK:
        {
            mediaFileHandle->message_queue_central_engine->push_front(EVT_SEEK);
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeVideoStateMachine thread decode_wait state recv EVT_SEEK evt!!\n");
            this->state_machine_change_state(STATE_DECODE_SEEK_WAIT);  // change state.
            return;
        }

        default:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeVideoStateMachine thread decode_wait state recv others EVT:%d\n" ,evt);
            return;
        }
    }

}

void MediaDecodeVideoStateMachine::do_process_video_decode_start(player_event_e evt)
{
    switch(evt)
    {
        case EVT_RESUME:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeVideoStateMachine thread decode_start state recv EVT_RESUME evt!!\n");
        }
        case EVT_START:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeVideoStateMachine thread decode_start state recv EVT_START evt!!\n");
            this->state_machine_change_state(STATE_DECODER_WORK);
            this->mediaFileHandle->message_queue_video_decode->push(EVT_DECODE_GO_ON);

            return;
        }
        case EVT_SEEK:
        {
            mediaFileHandle->message_queue_central_engine->push_front(EVT_SEEK);
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeVideoStateMachine thread decode_start state recv EVT_SEEK evt!!\n");
            this->state_machine_change_state(STATE_DECODE_SEEK_WAIT);  // change state.
            return;
        }

        default:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeVideoStateMachine thread decode_start state receive others EVT:%d\n" ,evt);
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
            //
            if(mediaFileHandle->video_frame_queue->node_count >= X_MAX_FRAME_VIDEO_Q_NODE_CNT)
            {
                //XLog::d(ANDROID_LOG_WARN ,TAG ,"== MediaDecodeVideoStateMachine video_frame_queue is full!\n");
                usleep(30000);
                this->mediaFileHandle->message_queue_video_decode->push(EVT_DECODE_GO_ON);
                return;
            }

            AVPacket pkt;
            int ret = mediaFileHandle->video_queue->get(&pkt ,0);   // non block
            if(ret == 0){   // get no packet.
                if(mediaFileHandle->end_of_file){ // end of file
                    XLog::d(ANDROID_LOG_INFO ,TAG ,"video decoder statemachine ,in the end of file and change state to STATE_DECODER_START!\n");
                    this->state_machine_change_state(STATE_DECODER_START);  // change state.

                }else{
                    XLog::d(ANDROID_LOG_INFO ,TAG ,"video decoder statemachine , not in the end of file ,may will go in Buffering! ,video packet q size = %d\n" ,mediaFileHandle->video_queue->size());
                    usleep(50000);
                    this->mediaFileHandle->message_queue_video_decode->push(EVT_DECODE_GO_ON);
                }
                return;
            }
            int rr = mediaFileHandle->video_queue->size();
            //XLog::d(ANDROID_LOG_WARN ,TAG ,"== MediaDecodeVideoStateMachine ,pkt.size = %d ,rr=%d ,ret =%d\n" ,pkt.size ,rr ,ret);

            // TODO
            decode_one_video_packet(&pkt );
            this->mediaFileHandle->message_queue_video_decode->push(EVT_DECODE_GO_ON);

            return;
        }

        case EVT_PAUSE:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeVideoStateMachine thread decode_work state recv EVT_PAUSE evt!!\n");
            this->state_machine_change_state(STATE_DECODER_WAIT);

            return;
        }

        case EVT_STOP:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeVideoStateMachine thread decode_work state recv EVT_STOP evt!!\n");
            this->state_machine_change_state(STATE_DECODER_START);  // change state.

            // TODO set mark

            return;
        }
        case EVT_SEEK:
        {
            mediaFileHandle->message_queue_central_engine->push_front(EVT_SEEK);
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeVideoStateMachine thread decode_work state recv EVT_SEEK evt!!\n");
            this->state_machine_change_state(STATE_DECODE_SEEK_WAIT);  // change state.
            return;
        }
        default:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeVideoStateMachine thread decode_start state receive others EVT:%d\n" ,evt);
            return;
        }
    }

}

void MediaDecodeVideoStateMachine::do_process_video_decode_seek_wait(player_event_e evt)
{
    switch(evt)
    {
        case EVT_SEEK_DONE:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeVideoStateMachine thread seek_wait state recv EVT_SEEK_DONE evt!!\n");
            this->state_machine_change_state(STATE_DECODER_WAIT);
            return;
        }
        case EVT_SEEK_PAUSE:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeVideoStateMachine thread seek_wait state recv EVT_SEEK_PAUSE evt!!\n");
            return;
        }
        case EVT_START:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeVideoStateMachine thread seek_wait state recv EVT_START evt!!\n");
            this->state_machine_change_state(STATE_DECODER_WORK);
            this->mediaFileHandle->message_queue_video_decode->push(EVT_DECODE_GO_ON);
            return;
        }

        default:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"== MediaDecodeVideoStateMachine thread seek_wait state receive others EVT:%d\n" ,evt);
            return;
        }
    }

}
