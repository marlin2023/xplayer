//
// Created by chris on 10/19/16.
//

#define TAG "Media_Demux_State_Machine"


#include <android/log.h>
#include <unistd.h>
#include <stdint.h>

#include "CentralEngineStateMachine.h"
#include "PlayerState.h"
#include "xplayer_android_def.h"

#include "util/XLog.h"
#include "util/XMessageType.h"

CentralEngineStateMachine::CentralEngineStateMachine(MediaFile *mediaFile)
{
    this->mediaFileHandle = mediaFile;
    this->read_retry_count = 0;

    // init message queue for media demux state machine.
    this->mediaFileHandle->message_queue_central_engine = new XMessageQueue();

    // init the state
    this->state = STATE_IDLE;
    this->hasShowFirstPic = false;
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
    this->mediaFileHandle->message_queue_central_engine = new XMessageQueue();

    // init the state
    this->state = STATE_IDLE;

    this->hasShowFirstPic = false;
}

CentralEngineStateMachine::~CentralEngineStateMachine()
{
    if(this->mediaFileHandle->message_queue_central_engine){
        delete this->mediaFileHandle->message_queue_central_engine;
        this->mediaFileHandle->message_queue_central_engine = NULL;
    }

    this->state = STATE_IDLE;
    this->hasShowFirstPic = false;
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
        //XLog::d(ANDROID_LOG_INFO ,TAG ,"==>VVVVideo packet queue size = %d ,pkt->q_size =%d\n" ,pkt_q->size() ,pkt_q->q_size);

    }
    else if (pkt->stream_index == st_index[AVMEDIA_TYPE_AUDIO])
    {
        // audio packet
        pkt_q = mediaFile->audio_queue;
        //XLog::d(ANDROID_LOG_INFO ,TAG ,"==>AAAAudio packet queue size = %d\n" ,pkt_q->size());
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

        evt = this->mediaFileHandle->message_queue_central_engine->pop();
        //XLog::d(ANDROID_LOG_WARN ,TAG ,"==>MediaDemuxStateMachine msq evt = %d\n" ,evt);

        // Exit thread until receive the EXIT_THREAD EVT
        if(evt == EVT_EXIT_THREAD)
        {
            XLog::d(ANDROID_LOG_WARN ,TAG ,"==>MediaDemuxStateMachine msq evt = EVT_EXIT_THREAD\n");
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
            central_engine_do_process_play_paused(evt);
            return;
        }
        case STATE_PLAY_COMPLETE:
        {
            central_engine_do_process_play_complete(evt);
            return;
        }
        case STATE_STOPPED:
        {
            //el_demux_file_do_process_paused(evt);
            return;
        }

        case STATE_SEEK_WAIT:
        {
            central_engine_do_process_seek_wait(evt);
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
                if(mediaFileHandle->stop_flag){
                    XLog::d(ANDROID_LOG_INFO ,TAG ,"===>initialized state ready EVT_STOP ,change to stopped.\n");
                    this->state_machine_change_state(STATE_STOPPED);
                    return;
                }

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
                if(mediaFileHandle->stop_flag){
                    XLog::d(ANDROID_LOG_INFO ,TAG ,"===>initialized state ready EVT_STOP ,change to stopped.\n");
                    this->state_machine_change_state(STATE_STOPPED);
                    return;
                }
                // open failed,already notify app in open func
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
            this->mediaFileHandle->message_queue_central_engine->push(EVT_GO_ON);
            return;
        }
        case EVT_SEEK:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"===>STATE_PREPARED receive EVT_SEEK.\n");
            this->state_machine_change_state(STATE_SEEK_WAIT);
            this->mediaFileHandle->message_queue_central_engine->push(EVT_READY_TO_SEEK);

            return;
        }

        case EVT_STOP:
        {
            // do stop actions
            this->state_machine_change_state(STATE_STOPPED);
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
            mediaFileHandle->end_of_file = false;
            if (demux_2_packet_queue() == AVERROR_EOF && this->read_retry_count > 5)
            {
                XLog::d(ANDROID_LOG_INFO ,TAG ,"===>AVERROR_EOF....\n");
                break;
            }

            if(mediaFileHandle->is_pkt_q_full(mediaFileHandle->start_playing_buffering_time))
            {
                XLog::d(ANDROID_LOG_INFO ,TAG ,"===>is_pkt_q_full break;.\n");
                this->mediaFileHandle->isBuffering = false;
                // TODO ,here should to notify upper layer
                // TODO
                if( (this->old_state == STATE_SEEK_WAIT ) && (this->hasShowFirstPic == true)){
                    //STATE_SEEK_WAIT
                    // after seek, discard packet that comes from last play
                    XLog::e(TAG ,"===>state_machine: sending seek done evt to decoder 00!\n");
                    this->state_machine_change_state(STATE_PLAY_WAIT);
                    // Notify decoder thread
                    this->mediaFileHandle->message_queue_video_decode->push(EVT_SEEK_DONE);
                    this->mediaFileHandle->message_queue_video_decode->push(EVT_PLAY);
                    this->mediaFileHandle->message_queue_audio_decode->push(EVT_SEEK_DONE);
                    this->mediaFileHandle->message_queue_audio_decode->push(EVT_PLAY);

                    // re set seek_mark
                    this->mediaFileHandle->seeking_mark = 0;
                    usleep(10000);
                    XLog::e(TAG ,"===>state_machine: MEDIA_SEEK_COMPLETE notify\n");
                    this->mediaFileHandle->notify(MEDIA_SEEK_COMPLETE ,0 ,0);
                    return;
                }

                XLog::e(TAG ,"===>state_machine: show first pic 0\n");
                this->mediaFileHandle->notify(MEDIA_BUFFERING_UPDATE ,100 ,100);
                this->state_machine_change_state(STATE_PLAY_WAIT);
                // TODO
                this->mediaFileHandle->notify(MEDIA_INFO ,MEDIA_INFO_FIRST_SHOW_PIC ,0);    // notify first picture.
                //this->mediaFileHandle->notify(MEDIA_INFO ,MEDIA_INFO_BUFFERING_END ,0);
                this->hasShowFirstPic = true;
                this->mediaFileHandle->seeking_mark = 0;

                return;
            }
            this->mediaFileHandle->message_queue_central_engine->push(EVT_GO_ON);
            return;
        }
        case EVT_SEEK:
        {
            this->state_machine_change_state(STATE_SEEK_WAIT);
            this->mediaFileHandle->message_queue_central_engine->push(EVT_READY_TO_SEEK);
            return;
        }

        case EVT_STOP:
        {
            XLog::e(TAG ,"===>STATE_BUFFERING receive EVT_STOP\n" );
            this->state_machine_change_state(STATE_STOPPED);
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
        case EVT_START: // TODO
        case EVT_RESUME:    // after seek
        {XLog::d(ANDROID_LOG_INFO ,TAG ,"===>in PLAY_WAIT STATE ,receive EVT_RESUME Event.\n");}
        case EVT_PLAY:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"===>in PLAY_WAIT STATE ,receive EVT_PLAY Event.\n");
            this->state_machine_change_state(STATE_PLAY_PLAYING);
            //
            this->mediaFileHandle->message_queue_central_engine->push(EVT_GO_ON);
            this->mediaFileHandle->notify(MEDIA_INFO ,MEDIA_INFO_BUFFERING_END ,0);
            this->mediaFileHandle->startRender();   // add
            return;
        }

        case EVT_STOP:
        {
            this->state_machine_change_state(STATE_STOPPED);
            return;
        }
        default:
        {
            XLog::e(TAG ,"===>STATE_PLAY_WAIT receive others EVT:%d\n" ,evt);
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
            mediaFileHandle->end_of_file = false;

            //XLog::d(ANDROID_LOG_INFO ,TAG ,"===>in PLAYING ,receive EVT_GO_ON Event .\n");
            // the eof of file or read error .
            if (demux_2_packet_queue() == AVERROR_EOF && this->read_retry_count > 5)
            {
                //XLog::e(TAG ,"state_machine:PLAYING FILE EOF,vpktq = %d,apktq = %d\n",
                //        mediaFileHandle->video_queue->size(),
                //        mediaFileHandle->audio_queue->size());

                //is_file_eof = EL_TRUE;
                // TODO
                this->state_machine_change_state(STATE_PLAY_FILE_END);
                this->mediaFileHandle->end_of_file = true;
                // send message
                this->mediaFileHandle->message_queue_central_engine->push(EVT_GO_ON);
                //
                return;
            }

            // Video packet q is full
            if(mediaFileHandle->video_queue->q_size >= X_MAX_PKT_VIDEO_Q_MEM_SPACE)
            {
                ret = 1;

                //XLog::d(ANDROID_LOG_INFO ,TAG ,"===> pkt q full, %d,%d,%d\n",
                //mediaFileHandle->video_queue->size(),
                //mediaFileHandle->audio_queue->size(),
                //mediaFileHandle->video_queue->q_size);
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
            this->mediaFileHandle->message_queue_central_engine->push(EVT_GO_ON);
            return;
        }
        case EVT_PAUSE:
        {
            this->state_machine_change_state(STATE_PLAY_PAUSED);
            return;
        }

        case EVT_STOP:
        {
            this->state_machine_change_state(STATE_STOPPED);
            return;
        }

        case EVT_SEEK:
        {
            this->state_machine_change_state(STATE_SEEK_WAIT);
            this->mediaFileHandle->message_queue_central_engine->push(EVT_READY_TO_SEEK);
            return;
        }
        case EVT_BUFFERING:
        {
            XLog::e(TAG ,"===>STATE_PLAYING receive EVT_BUFFERING EVT:\n");
            this->state_machine_change_state(STATE_BUFFERING);
            this->mediaFileHandle->message_queue_central_engine->push(EVT_GO_ON);
            return;
        }

        default:
        {
            XLog::e(TAG ,"===>STATE_PLAYING receive others EVT:%d\n" ,evt);
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
                this->state_machine_change_state(STATE_PLAY_COMPLETE);
                //this->mediaFileHandle->message_queue_central_engine->push(EVT_STOP);
                this->mediaFileHandle->notify(MEDIA_PLAYBACK_COMPLETE ,0 ,0);
            }else{
                usleep(50000);
                this->mediaFileHandle->message_queue_central_engine->push(EVT_GO_ON);
            }
            return;
        }

        case EVT_PAUSE:
        {
            XLog::e(TAG ,"====>state_machine: pause in play file end ...\n");
            this->state_machine_change_state(STATE_PLAY_PAUSED);
            return;
        }

        case EVT_STOP:
        {
            this->state_machine_change_state(STATE_STOPPED);
            return;
        }
        default:
        {
            XLog::e(TAG ,"===>STATE_FILE_END_OF receive others EVT:%d\n" ,evt);
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
        case EVT_SEEK:
        {
            XLog::d(ANDROID_LOG_INFO ,TAG ,"===>STATE_PLAY_COMPLETE receive EVT_SEEK !\n");
            this->state_machine_change_state(STATE_SEEK_WAIT);
            this->mediaFileHandle->message_queue_central_engine->push(EVT_READY_TO_SEEK);
            return;
        }
        case EVT_STOP:
        {
            this->state_machine_change_state(STATE_STOPPED);
            return;
        }
        default:
        {
            XLog::e(TAG ,"===>STATE_PLAY_COMPLETE receive others EVT:%d\n" ,evt);
            return;
        }


    }
    return;

}

void CentralEngineStateMachine::central_engine_do_process_play_paused(player_event_e evt)
{
    switch(evt)
    {
        case EVT_RESUME:
        {
            this->state_machine_change_state(STATE_PLAY_PLAYING);
            // send read next packet message
            this->mediaFileHandle->message_queue_central_engine->push(EVT_GO_ON);
            return;
        }
        case EVT_STOP:
        {
            this->state_machine_change_state(STATE_STOPPED);
            return;
        }

        case EVT_SEEK:
        {
            this->state_machine_change_state(STATE_SEEK_WAIT);
            this->mediaFileHandle->message_queue_central_engine->push(EVT_READY_TO_SEEK);

            return;
        }
        default:
        {
            XLog::e(TAG ,"===>STATE_PLAY_PAUSED receive others EVT:%d\n" ,evt);
            return;
        }


    }
    return;

}

void CentralEngineStateMachine::central_engine_do_process_play_stopped(player_event_e evt)
{
    switch(evt)
    {
        case EVT_CLOSE:
        {
            this->state_machine_change_state(STATE_IDLE);
            return;
        }
        default:
        {
            XLog::e(TAG ,"===>STATE_PLAY_STOPPED receive others EVT:%d\n" ,evt);
            return;
        }

    }
    return;

}

void CentralEngineStateMachine::central_engine_do_process_seek_wait(player_event_e evt)
{
    switch(evt)
    {
        case EVT_READY_TO_SEEK:
        {
            XLog::e(TAG ,"===>STATE_SEEK_WAIT receive EVT_READY_TO_SEEK\n");
            // do seek work
            AVFormatContext *fc = this->mediaFileHandle->format_context;

            if(this->hasShowFirstPic){
                // Notify UI and engine to send buffering start message
                this->mediaFileHandle->notify(MEDIA_INFO ,MEDIA_INFO_BUFFERING_START ,0);
            }
            this->state_machine_change_state(STATE_BUFFERING);
            // Note that this event is sent by decode thread when it is in its wait state
            ffmpeg_do_seek();

            // do resume actions
            av_read_play(fc);

            //this->state_machine_change_state(STATE_BUFFERING);
            // send read next packet message
            this->mediaFileHandle->message_queue_central_engine->push(EVT_GO_ON);
            // after seek, discard packet that comes from last play
            //XLog::e(TAG ,"===>state_machine: sending seek done evt to decoder!\n");
            // Notify decoder thread
            //this->mediaDecodeAudioStateMachineHandle->message_queue->push(EVT_SEEK_DONE);
            //this->mediaDecodeVideoStateMachineHandle->message_queue->push(EVT_SEEK_DONE);
            return;
        }
        default:
        {
            XLog::e(TAG ,"===>STATE_SEEK_WAIT receive others EVT:%d\n" ,evt);
            return;
        }


    }
    return;

}

void CentralEngineStateMachine::ffmpeg_do_seek(void)
{
    AVStream *stream;
    int st_index;
    int64_t abs_seek_pos,pts;

    // do seek actions
    int ret,i;
    AVPacket packet;
    XLog::e(TAG ,"===>in ffmpeg do work function\n");
    AVFormatContext *fc = this->mediaFileHandle->format_context;

    mediaFileHandle->stopRender();  // stop render
    // clear packet & frame queue
    this->mediaFileHandle->audio_queue->flush();
    this->mediaFileHandle->video_queue->flush();
    this->mediaFileHandle->audio_frame_queue->flush();
    this->mediaFileHandle->video_frame_queue->flush();

    switch(this->mediaFileHandle->av_support)
    {
        case HAS_BOTH:
        case HAS_VIDEO:
        {
            stream = this->mediaFileHandle->video_stream;
            st_index = this->mediaFileHandle->stream_index[AVMEDIA_TYPE_VIDEO];
            break;
        }
        case HAS_AUDIO:
        {
            break;
        }
        default:
        {
            return;
        }
    }

    int64_t seek_pos = av_rescale(this->mediaFileHandle->seekpos, AV_TIME_BASE, 1000); //milliseconds_to_fftime(msec);
    if(seek_pos >= this->mediaFileHandle->format_context->duration){
        // oncomplete
        XLog::e(TAG ,"====>state_machine: =====>ffmpeg_do_seek  onComplete.\n");
        XLog::e(TAG ,"====>state_machine: =====>ffmpeg_do_seek notify eof and should quit ...\n");
        this->state_machine_change_state(STATE_PLAY_COMPLETE);
        this->mediaFileHandle->message_queue_video_decode->push(EVT_SEEK_DONE);
        this->mediaFileHandle->message_queue_audio_decode->push(EVT_SEEK_DONE);
        this->mediaFileHandle->notify(MEDIA_PLAYBACK_COMPLETE ,0 ,0);
        return;
    }
    int64_t start_time = this->mediaFileHandle->format_context->start_time;
    if (start_time > 0 && start_time != AV_NOPTS_VALUE){
        seek_pos += start_time;
    }

    XLog::e(TAG ,"state_machine: =====>before avcodec_flush_buffers 00\n");
    avcodec_flush_buffers(this->mediaFileHandle->video_codec_context);
    XLog::e(TAG ,"state_machine: =====>before avcodec_flush_buffers 01\n");
    avcodec_flush_buffers(this->mediaFileHandle->audio_codec_context);

    // TODO need to judge the seek_pos value .
    XLog::e(TAG ,"state_machine: seek start(in msec):%lld, pts = %lld\n", (int64_t)this->mediaFileHandle->seekpos,(int64_t)seek_pos);
    ret = avformat_seek_file(fc, -1, INT64_MIN, seek_pos, INT64_MAX, 0);

    XLog::e(TAG ,"-state_machine: after avformat_seek_file, ret = %d\n",ret);
    return;
}