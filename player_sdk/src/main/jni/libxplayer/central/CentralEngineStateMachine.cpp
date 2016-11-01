//
// Created by chris on 10/19/16.
//

#define TAG "Media_Demux_State_Machine"


#include <android/log.h>
#include <unistd.h>

#include "CentralEngineStateMachine.h"
#include "PlayerState.h"

#include "util/XLog.h"
#include "util/XMessageType.h"

CentralEngineStateMachine::CentralEngineStateMachine(MediaFile *mediaFile)
{
    this->mediaFileHandle = mediaFile;
    this->read_retry_count = 0;

    // init message queue for media demux state machine.
    this->message_queue = new XMessageQueue();

    // init the state
    this->state = STATE_IDLE;
}

CentralEngineStateMachine::CentralEngineStateMachine(MediaFile *mediaFile ,
                                                    MediaDecodeAudioStateMachine *mediaDecodeAudioStateMachine ,
                                                    MediaDecodeVideoStateMachine *mediaDecodeVideoStateMachine)
{
    this->mediaFileHandle = mediaFile;
    this->mediaDecodeAudioStateMachineHandle = mediaDecodeAudioStateMachine;
    this->mediaDecodeVideoStateMachineHandle = mediaDecodeVideoStateMachine;

    this->read_retry_count = 0;

    // init message queue for media demux state machine.
    this->message_queue = new XMessageQueue();

    // init the state
    this->state = STATE_IDLE;
}

CentralEngineStateMachine::~CentralEngineStateMachine()
{

}

int CentralEngineStateMachine::demux_2_packet_queue()
{
    AVPacket packet;
    int ret;
    AVFormatContext *format_context = mediaFileHandle->format_context;

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
    return add_packet_to_q(&packet ,mediaFileHandle);

}

int CentralEngineStateMachine::add_packet_to_q(AVPacket *pkt ,MediaFile *mediaFile)
{

    AVFormatContext *format_context;
    int *st_index;
    PacketQueue *pkt_q;

    format_context = mediaFile->format_context;
    st_index = &mediaFile->stream_index[0];



    if (pkt->stream_index == st_index[AVMEDIA_TYPE_VIDEO])
    {
        // video packet
        pkt_q = mediaFile->video_queue;
        XLog::d(ANDROID_LOG_INFO ,TAG ,"==>VVVVideo packet queue size = %d ,pkt->q_size =%d\n" ,pkt_q->size() ,pkt_q->q_size);

    }
    else if (pkt->stream_index == st_index[AVMEDIA_TYPE_AUDIO])
    {
        // audio packet
        pkt_q = mediaFile->audio_queue;
        XLog::d(ANDROID_LOG_INFO ,TAG ,"==>AAAAudio packet queue size = %d\n" ,pkt_q->size());
    }
    else
    {
        // no used packet ,release directly.
        av_packet_unref(pkt);   //av_free_packet(pkt);
        return -1;
    }

    // Entry queue
    int ret = pkt_q->put(pkt);

    //
    return ret;
}


void CentralEngineStateMachine::state_machine_change_state(player_state_e new_state)
{
    this->old_state = this->state;
    this->state = new_state;

}

void CentralEngineStateMachine::central_engine_thread(MediaFile *mediaFile)
{
    player_event_e evt;

    while(1){

        evt = message_queue->pop();
        //XLog::d(ANDROID_LOG_WARN ,TAG ,"==>MediaDemuxStateMachine msq evt = %d\n" ,evt);

        // Exit thread until receive the EXIT_THREAD EVT
        if(evt == EVT_EXIT_THREAD)
        {
            break;
        }

        // process others evt
        central_engine_state_machine_process_event(evt);

    }
}

// TODO
void CentralEngineStateMachine::central_engine_state_machine_process_event(player_event_e evt)
{

    switch(this->state)
    {
        case STATE_IDLE:
        {
            central_engine_do_process_idle(evt);
            return;
        }
        case STATE_INITIALIZED:
        {
            central_engine_do_process_initialized(evt);
            return;
        }
        case STATE_PREPARED:
        {
            central_engine_do_process_prepared(evt);
            return;
        }
        case STATE_BUFFERING:
        {
            central_engine_do_process_buffering(evt);
            return;
        }
        case STATE_PLAY_WAIT:
        {
            central_engine_do_process_play_wait(evt);
            return;
        }
        case STATE_PLAY_PLAYING:
        {
            central_engine_do_process_playing(evt);
            return;
        }

        case STATE_PLAY_FILE_END:
        {
            central_engine_do_process_play_file_end(evt);
            return;
        }

        case STATE_PLAY_PAUSED:
        {
            //el_demux_file_do_process_playing(evt);
            return;
        }
        case STATE_PLAY_COMPLETE:
        {
            central_engine_do_process_play_complete(evt);
            //el_demux_file_do_process_play_complete(evt);
            return;
        }
        case STATE_STOPPED:
        {
            //el_demux_file_do_process_paused(evt);
            return;
        }
        default:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"Invalid state!\n");
            return;
        }
    }
}

void CentralEngineStateMachine::central_engine_do_process_idle(player_event_e evt)
{
    switch(evt)
    {
        case EVT_OPEN:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"state_machine: idle state recv EVT_OPEN\n");
            //el_state_machine_change_state(demux_file_state_machine, EL_ENGINE_CENTRAL_STATE_PREPARE);

            // 通过ipc，使得状态机和thread关联起来
            // 在一个线程中，驱动另一个线程。
            //el_ipc_send(EVT_GO_ON, el_msg_q_demux_file);
            return;
        }
        default:
        {
            return;
        }
    }
    return;

}


void CentralEngineStateMachine::central_engine_do_process_initialized(player_event_e evt)
{
    switch(evt)
    {
        case EVT_OPEN:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"state_machine: initialized state recv EVT_OPEN\n");

            if(mediaFileHandle->open())
            {
                // open success.
                this->state_machine_change_state(STATE_PREPARED);
                XLog::d(ANDROID_LOG_INFO ,TAG ,"state_machine: open file:-%s- success !!!\n" ,mediaFileHandle->getSourceUrl());

                // TODO notify upper open success ,and upper will send EVT_START and go into Buffering state.
                // TODO
                //this->message_queue->push(EVT_START);   //TODO here should be performed in upper layer
                //
                this->mediaFileHandle->notify(MEDIA_PREPARED ,0 ,0);
            }
            else
            {
                // open failed
                XLog::d(ANDROID_LOG_INFO ,TAG ,"state_machine: open file:-%s- failed !!!\n" ,mediaFileHandle->getSourceUrl());
                this->state_machine_change_state(STATE_INITIALIZED);
            }

            return;
        }
        default:
        {
            return;
        }

    }
    return;

}

void CentralEngineStateMachine::central_engine_do_process_prepared(player_event_e evt)
{
    switch(evt)
    {
        case EVT_START:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"===>STATE_PREPARED receive EVT_START.\n");
            this->state_machine_change_state(STATE_BUFFERING);
            this->message_queue->push(EVT_GO_ON);


            // TODO
            // TODO here ,send EVT_START to audio decode state machine & video decode state machine
            //
            //el_do_start_central_engine();

            return;
        }
        default:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"===>STATE_PREPARED receive others EVT.\n");
            return;
        }
    }
    return;

}


void CentralEngineStateMachine::central_engine_do_process_buffering(player_event_e evt)
{
    switch(evt)
    {
        case EVT_GO_ON:
        {

            if (demux_2_packet_queue() == AVERROR_EOF && this->read_retry_count > 5)
            {
                XLog::d(ANDROID_LOG_INFO ,TAG ,"===>AVERROR_EOF....\n");
                break;
            }

            if(mediaFileHandle->is_pkt_q_full(mediaFileHandle->start_playing_buffering_time))
            {
                XLog::d(ANDROID_LOG_INFO ,TAG ,"===>is_pkt_q_full break;.\n");
                // TODO ,here should to notify upper layer
                // TODO
                this->state_machine_change_state(STATE_PLAY_WAIT);
                this->mediaFileHandle->notify(MEDIA_BUFFERING_UPDATE ,100 ,100);

                return;
            }
            this->message_queue->push(EVT_GO_ON);
            return;
        }
        default:
        {
            XLog::e(TAG ,"===>STATE_BUFFERING receive others EVT:%d\n" ,evt);
            return;
        }

    }
    return;

}

void CentralEngineStateMachine::central_engine_do_process_play_wait(player_event_e evt)
{
    switch(evt)
    {
        case EVT_PLAY:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"===>in PLAY_WAIT STATE ,receive EVT_PLAY Event.\n");
            this->state_machine_change_state(STATE_PLAY_PLAYING);
            this->message_queue->push(EVT_GO_ON);
            return;
        }

    }
    return;

}


void CentralEngineStateMachine::central_engine_do_process_playing(player_event_e evt)
{
    switch(evt)
    {
        case EVT_GO_ON:
        {
            int ret = 0;
            //XLog::d(ANDROID_LOG_INFO ,TAG ,"===>in PLAYING ,receive EVT_GO_ON Event .\n");
            // the eof of file or read error .
            if (demux_2_packet_queue() == AVERROR_EOF && this->read_retry_count > 5)
            {
                XLog::e(TAG ,"state_machine:PLAYING FILE EOF,vpktq = %d,apktq = %d\n",
                        mediaFileHandle->video_queue->size(),
                        mediaFileHandle->audio_queue->size());

                //is_file_eof = EL_TRUE;
                // TODO
                this->state_machine_change_state(STATE_PLAY_FILE_END);
                // send message
                this->message_queue->push(EVT_GO_ON);
                //
                return;
            }

            // Video packet q is full
            if(mediaFileHandle->video_queue->q_size >= X_MAX_PKT_VIDEO_Q_MEM_SPACE)
            {
                ret = 1;

                XLog::d(ANDROID_LOG_INFO ,TAG ,"===> pkt q full, %d,%d,%d\n",
                mediaFileHandle->video_queue->size(),
                mediaFileHandle->audio_queue->size(),
                mediaFileHandle->video_queue->q_size);
            }

            if(ret)
            {
                usleep(10000);
            }
            else
            {
                usleep(1000);
            }

            //
            this->message_queue->push(EVT_GO_ON);
            return;
        }
    }
    return;

}

void CentralEngineStateMachine::central_engine_do_process_play_file_end(player_event_e evt)
{
    switch(evt)
    {
        case EVT_GO_ON:
        {
            // notify TODO
            if( (mediaFileHandle->video_queue->size() == 0) &&
                (mediaFileHandle->audio_queue->size() == 0) &&
                (mediaFileHandle->video_frame_queue->size() == 0) &&
                (mediaFileHandle->audio_frame_queue->size() == 0)){

                XLog::e(TAG ,"====>state_machine: notify eof and should quit ...\n");
                this->mediaFileHandle->notify(MEDIA_PLAYBACK_COMPLETE ,0 ,0);
            }else{
                usleep(50000);
                this->message_queue->push(EVT_GO_ON);
            }
            return;
        }
    }
    return;

}

void CentralEngineStateMachine::central_engine_do_process_play_complete(player_event_e evt)
{
    switch(evt)
    {
        case EVT_GO_ON:
        {

            return;
        }
    }
    return;

}
