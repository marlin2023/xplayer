//
// Created by chris on 10/17/16.
//


#define TAG "MediaFile_CPP"


#include "MediaFile.h"
#include "util/XLog.h"

MediaFile::MediaFile()
{
    // TODO
    //source_url = "/sdcard/hh.mp4";  // default

    // init packet queue
    audio_queue = new PacketQueue();
    video_queue = new PacketQueue();

    // init frame queue
    audio_frame_queue = new FrameQueue(X_MAX_FRAME_AUDIO_Q_NODE_CNT);
    video_frame_queue = new FrameQueue(X_MAX_FRAME_VIDEO_Q_NODE_CNT);

    audio_frame_queue->empty_param = this;
    video_frame_queue->empty_param = this;
    //
    audio_codec_context = NULL;
    video_codec_context = NULL;

    //
    file_opened = false;
    stop_flag = false;  // not stop

    //
    //
    // 如果是网络文件, 设置Buffer时间以及超时时间
    //if (strstr((const char *)source_url,"http://") ||
    //    strstr((const char *)source_url,"rtsp://") ||
    //    strstr((const char *)source_url,"rtmp://"))
    //{
        start_playing_buffering_time = X_MAX_PKT_Q_NETWORK_FIRST_BUFFERING_TS;    // 3s
        max_buffering_time = X_MAX_PKT_Q_NETWORK_BUFFERING_TS;                    // 6s

        //is_network_media = X_TRUE;
    //}
    //else
    //{
    //    start_playing_buffering_time = X_MAX_PKT_Q_NETWORK_FIRST_BUFFERING_TS;// X_MAX_PKT_Q_TS;    // 0.6s
    //    max_buffering_time = 2 * X_MAX_PKT_Q_TS;          // 1.2s
    //}


    sync_audio_clock_time = 0.f;

    duration_ms = 0;
    current_position_ms = 0;

}


MediaFile::~MediaFile()
{
    // destroy queue
    XLog::e(TAG ,"======>in ~MediaFile start.");
    if(audio_queue){
        delete audio_queue;
        audio_queue = NULL;
    }

    if(video_queue){
        delete video_queue;
        video_queue = NULL;
    }

    if(audio_frame_queue){
        delete audio_frame_queue;
        audio_frame_queue = NULL;
    }

    if(video_frame_queue){
        delete video_frame_queue;
        video_frame_queue = NULL;
    }
    XLog::e(TAG ,"======>in ~MediaFile end.");


}

int MediaFile::setListener(MediaPlayerListener* listener)
{
    mListener = listener;

    return 0;
}

void MediaFile::notify(int msg, int ext1, int ext2)
{

    if (mListener != NULL)
    {
        mListener->notify(msg, ext1, ext2);
    }
}

static int decode_interrupt_cb(void *ctx)
{
    MediaFile *is = (MediaFile *)ctx;

    // 返回1，表示要ffmpeg退出被阻塞的内部函数，是Quit的含义。
    // 如果文件没有打开，则不阻塞。
    // 如果文件已经打开了，则该阻塞的，还是要阻塞
    //int quit_ffmpeg_block = !is->file_opened;

    if(is->stop_flag){
        XLog::d(ANDROID_LOG_INFO ,TAG ,"==>in decode_interrupt_cb ,stop_flag is true. \n");
        return 1; // exit
    }
    return 0;
}


CM_BOOL MediaFile::open()
{
    int ret = 0;
    int i;
    char *source_url = getSourceUrl();
    XLog::d(ANDROID_LOG_INFO ,TAG ,"==>in open file ,source url=%s" ,source_url);

    format_context = NULL;
    // 1. Open input stream and read the header
    ret = avformat_open_input (&format_context, (const char *)source_url, NULL, NULL);
    if(ret != 0){

        XLog::e(TAG ,"av_open_input_file,ret %d err\n",ret);
        ret = CM_FALSE;
        goto notify_callback;
    }

    // set interrup call back
    // 设置最新版ffmpeg 1.0的回调格式
    // 处理网络堵塞的情况，一直读取不到数据，程序无法退出
    format_context->interrupt_callback.callback = decode_interrupt_cb;
    format_context->interrupt_callback.opaque = this;

    // 2. Read packets of a media file to get stream information.
    ret = avformat_find_stream_info(format_context, NULL);
    if(ret < 0)
    {
        XLog::e(TAG ,"Couldn't find stream information %d\n",ret);
        if(format_context)
        {
            // close format_context, and set it to null
            avformat_close_input(&format_context);
            format_context = NULL;
        }
        ret = CM_FALSE;
        goto notify_callback;
    }

    XLog::d(ANDROID_LOG_INFO ,TAG ,"==>state_machine:av_find_stream_info called!\n");
    // set duration
    if(format_context->duration != AV_NOPTS_VALUE)
    {
        int64_t secs, us;
        secs = format_context->duration / AV_TIME_BASE;
        us = format_context->duration % AV_TIME_BASE;
        duration_ms = secs * 1000 + us / 1000;      // ms
    }
    else
    {
        duration_ms = 0;
    }

    av_dump_format(format_context, 0, (const char *)source_url, 0);

    // Default no subtitles
    stream_index[AVMEDIA_TYPE_SUBTITLE] = -1;
    stream_index[AVMEDIA_TYPE_VIDEO] = -1;
    stream_index[AVMEDIA_TYPE_AUDIO] = -1;
    av_support = HAS_NONE;

    if(this->stop_flag){ return true;}

    XLog::d(ANDROID_LOG_INFO ,TAG ,"==> Traversing streams information in file ,nb_streams=%d\n" ,format_context->nb_streams);
    // Traversing streams information in file.
    for(i = 0; i < format_context->nb_streams; i++)
    {
        // only get the first video stream
        // AVCodecParameters
        if( (format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) &&
          !(av_support & HAS_VIDEO) )
        {
            stream_index[AVMEDIA_TYPE_VIDEO] = i;
            video_stream = format_context->streams[i];
            av_support = av_support_type_e (av_support | HAS_VIDEO);
        }

        // only get the first audio stream
        if(format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO &&
            !(av_support & HAS_AUDIO))
        {
            stream_index[AVMEDIA_TYPE_AUDIO] = i;
            audio_stream = format_context->streams[i];
            av_support = av_support_type_e (av_support | HAS_AUDIO);
        }

        // subtitle support
        if (format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE)
        {
            stream_index[AVMEDIA_TYPE_SUBTITLE] = i;
        }
    }

    XLog::d(ANDROID_LOG_INFO ,TAG ," ==> stream_index[video] = %d,stream_index[audio] =%d ,av_support =%d\n",
        stream_index[AVMEDIA_TYPE_VIDEO],
        stream_index[AVMEDIA_TYPE_AUDIO],
        av_support);

    // Open the codec
    // process audio decoder
    if (stream_index[AVMEDIA_TYPE_AUDIO] >= 0)
    {
        // open audio decoder
        ret = stream_component_open(stream_index[AVMEDIA_TYPE_AUDIO]);
        if(!ret)
        {
            XLog::e(TAG ,"state_machine: open audio codec err, disable audio support\n");
            av_support = av_support_type_e(av_support & ~HAS_AUDIO);
            stream_index[AVMEDIA_TYPE_AUDIO] = -1;
        }
    }


    // process video decoder
    if (stream_index[AVMEDIA_TYPE_VIDEO] >= 0)
    {
        // open video decoder
        ret = stream_component_open(stream_index[AVMEDIA_TYPE_VIDEO]);
        if(!ret)
        {
            XLog::e(TAG ,"state_machine: open video codec err, disable video support\n");
            av_support = av_support_type_e(av_support & ~HAS_VIDEO);
            stream_index[AVMEDIA_TYPE_VIDEO] = -1;
        }
    }

    if(this->video_stream  && this->video_stream->codecpar){
        AVCodecParameters *codecpar = this->video_stream->codecpar;
        // notify
        this->notify(MEDIA_SET_VIDEO_SIZE ,codecpar->width, codecpar->height);
        this->notify(MEDIA_SET_VIDEO_SAR ,codecpar->sample_aspect_ratio.num, codecpar->sample_aspect_ratio.den);
    }

    XLog::d(ANDROID_LOG_INFO ,TAG ," ==> after open_decoder, stream_index[video] = %d,stream_index[audio] =%d ,av_support =%d\n",
        stream_index[AVMEDIA_TYPE_VIDEO],
        stream_index[AVMEDIA_TYPE_AUDIO],
        av_support);


    if(!av_support)
    {
        ret = CM_FALSE;
        goto notify_callback;
    }

    // set
    file_opened = true;

    ret = CM_TRUE;

notify_callback:

    return ret;

}


CM_BOOL MediaFile::stream_component_open(int stream_index)
{
    AVFormatContext *ic = format_context;
    AVCodecContext *codec_context;
    AVCodec *codec;

    int ret = 0;

    if (stream_index < 0 || stream_index >= ic->nb_streams)
    {
        XLog::e(TAG ,"===> invalid stream_index %d \n" ,stream_index);
        return CM_FALSE;
    }

    codec_context = avcodec_alloc_context3(NULL);
    if (!codec_context){
        XLog::e(TAG ,"===> avcodec_alloc_context3 failed\n");
        return CM_FALSE;
    }

    ret = avcodec_parameters_to_context(codec_context, ic->streams[stream_index]->codecpar);
    if (ret < 0){
        XLog::e(TAG ,"===> avcodec_parameters_to_context failed\n");
        ret = CM_FALSE;
        goto fail;
    }

    codec = avcodec_find_decoder(codec_context->codec_id);
    if (!codec || avcodec_open2(codec_context, codec, NULL) < 0)
    {
        if (codec_context->codec_type == AVMEDIA_TYPE_AUDIO){
            XLog::e(TAG ,"===> open audio decoder err\n");
        }else if(codec_context->codec_type == AVMEDIA_TYPE_VIDEO){
            XLog::e(TAG ,"===> open video decoder err\n");
        }
        ret = CM_FALSE;
        goto fail;
    }

    // set codec context.
    if (codec_context->codec_type == AVMEDIA_TYPE_AUDIO){
        audio_codec_context = codec_context;
        if(this->audio_stream->start_time != AV_NOPTS_VALUE)
        {
            this->beginning_audio_pts = (int64_t)(this->audio_stream->start_time * av_q2d(this->audio_stream->time_base)*1000);
            XLog::e(TAG ,"==beginning_audio_pts ,set audio begin pts = %lld\n",this->beginning_audio_pts);
        }

    }else if(codec_context->codec_type == AVMEDIA_TYPE_VIDEO){
        video_codec_context = codec_context;
        if(this->video_stream->start_time != AV_NOPTS_VALUE)
        {
            this->beginning_video_pts = (int64_t)(this->video_stream->start_time * av_q2d(this->video_stream->time_base)*1000);
            XLog::e(TAG ,"==beginning_video_pts ,set video begin pts = %lld\n",this->beginning_video_pts);
        }
    }

    ic->streams[stream_index]->discard = AVDISCARD_DEFAULT;
    ret = CM_TRUE;
    goto sucess;    // here not release codec_context.

fail:
    avcodec_free_context(&codec_context);

sucess:
    return ret;
}

void MediaFile::stream_component_close(int stream_index)
{
    AVFormatContext *ic = format_context;
    AVCodecParameters *codecpar;

    if (stream_index < 0 || stream_index >= ic->nb_streams)
    {
        return;
    }

    codecpar = ic->streams[stream_index]->codecpar;
    ic->streams[stream_index]->discard = AVDISCARD_ALL;
}


void MediaFile::setSourceUrl(const char *source_url)
{
    XLog::e(TAG ,"===>in setSourceUrl ,source_url = %s\n",source_url);
    this->source_url = (char *)source_url;
    XLog::e(TAG ,"===>in setSourceUrl ,this->source_url = %s\n",this->source_url);
}

char * MediaFile::getSourceUrl()
{
    XLog::e(TAG ,"===>in getSourceUrl ,this->source_url = %s\n",this->source_url);
    return this->source_url;
}


CM_BOOL MediaFile::is_pkt_q_full(int64_t max_buffer_ts)
{
    int64_t q_v_buffer_ts = -1;
    int64_t q_a_buffer_ts = -1;

    double buffering_percent = 0.1;

    AVFormatContext *fc = this->format_context;

    // 得到当前视频流的dts
    if(this->stream_index[AVMEDIA_TYPE_VIDEO] >= 0)
    {
        AVStream *vst = fc->streams[stream_index[AVMEDIA_TYPE_VIDEO]];
        q_v_buffer_ts = video_queue->get_buffer_packet_ts() * av_q2d(vst->time_base) * 1000;
    }

    // 得到当前音频流的dts
    if(stream_index[AVMEDIA_TYPE_AUDIO] >= 0)
    {
        AVStream *vst = fc->streams[stream_index[AVMEDIA_TYPE_AUDIO]];
        q_a_buffer_ts = audio_queue->get_buffer_packet_ts() * av_q2d(vst->time_base)*1000;
    }

    //XLog::d(ANDROID_LOG_WARN ,TAG ,"PKT Q, q_v_ts = %lld, q_a_ts = %lld, v count = %d, a count = %d, base_time = %lld\n",
    //           q_v_buffer_ts, q_a_buffer_ts, video_queue->size(), audio_queue->size(), max_buffer_ts);

    switch(this->av_support)
    {
        case HAS_BOTH:
        {
            // 视频包数据大小，超过配置值
            if(video_queue->q_size >= X_MAX_PKT_VIDEO_Q_MEM_SPACE)
            {
                XLog::d(ANDROID_LOG_WARN ,TAG ,"===>pkt video q exceed limit space, q full\n");

                goto is_full;
            }

            // 音频、视频，缓冲均已经达到最大缓冲的时间值
            if(q_v_buffer_ts > max_buffer_ts && q_a_buffer_ts > max_buffer_ts)
            {
                XLog::d(ANDROID_LOG_WARN ,TAG ,"queue is full: v ts = %lld, a ts = %lld, both > max buffer ts: %lld, is full\n",
                           q_v_buffer_ts,
                           q_a_buffer_ts,
                           max_buffer_ts);

                goto is_full;
            }

            buffering_percent = x_min(q_v_buffer_ts, q_a_buffer_ts) / (double)max_buffer_ts;

            goto is_not_full;
        }
        case HAS_AUDIO:   //TODO
        case HAS_VIDEO:   //TODO
        default:
        {
            return CM_FALSE;
        }
    }

is_full:

    XLog::d(ANDROID_LOG_WARN ,TAG ,"===>buffering is full");

    this->buffering_percent = 0;

    return CM_TRUE;

is_not_full:

    XLog::d(ANDROID_LOG_WARN ,TAG  ,"buffering is not full, buffering_percent = %f%%, max_buffer_ts = %lld", buffering_percent * 100, max_buffer_ts);
    // TODO notify buffering percent to upper layer.
    if((int)(buffering_percent* 100) == 100){
        buffering_percent = 99;
    }
    this->notify(MEDIA_BUFFERING_UPDATE ,buffering_percent* 100 ,100);

    return CM_FALSE;

}

void MediaFile::close_file()
{

    XLog::d(ANDROID_LOG_WARN ,TAG ,"===>close_file..,free ffmpeg resource .");
    // Close the codec
    if (audio_stream)
    {
        audio_stream->discard = AVDISCARD_ALL;
        avcodec_close(audio_codec_context);
        avcodec_free_context(&audio_codec_context);
    }

    if (video_stream)
    {
        video_stream->discard = AVDISCARD_ALL;
        avcodec_close(video_codec_context);
        avcodec_free_context(&video_codec_context);
    }

    if(format_context)
    {
        avformat_close_input(&format_context);
        format_context = NULL;
    }
    XLog::d(ANDROID_LOG_WARN ,TAG ,"===>close_file..,free ffmpeg resource end .");
}
