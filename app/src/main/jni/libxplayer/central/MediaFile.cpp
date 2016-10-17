//
// Created by chris on 10/17/16.
//


#define TAG "MediaFile_CPP"


#include "MediaFile.h"
#include "util/XLog.h"

MediaFile::MediaFile()
{
    // TODO
    source_url = "/sdcard/hh.mp4";

    // init queue
    audio_queue = new PacketQueue();
    video_queue = new PacketQueue();

    //

}


MediaFile::~MediaFile()
{

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
    av_dump_format(format_context, 0, (const char *)source_url, 0);

    // Default no subtitles
    stream_index[AVMEDIA_TYPE_SUBTITLE] = -1;
    stream_index[AVMEDIA_TYPE_VIDEO] = -1;
    stream_index[AVMEDIA_TYPE_AUDIO] = -1;
    av_support = HAS_NONE;

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

    XLog::d(ANDROID_LOG_INFO ,TAG ," ==> after open_decoder, stream_index[video] = %d,stream_index[audio] =%d ,av_support =%d\n",
        stream_index[AVMEDIA_TYPE_VIDEO],
        stream_index[AVMEDIA_TYPE_AUDIO],
        av_support);


    if(!av_support)
    {
        ret = CM_FALSE;
        goto notify_callback;
    }

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

    ic->streams[stream_index]->discard = AVDISCARD_DEFAULT;
    ret = CM_TRUE;
fail:
    avcodec_free_context(&codec_context);

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


void MediaFile::setSourceUrl(char *source_url)
{
    this->source_url = source_url;
}

char * MediaFile::getSourceUrl()
{
    return this->source_url;
}





