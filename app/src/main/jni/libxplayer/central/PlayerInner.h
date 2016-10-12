//
// Created by chris on 9/29/16.
//

#ifndef XPLAYER_PLAYERINNER_H
#define XPLAYER_PLAYERINNER_H

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"

} // end of extern C


class PlayerInner{

public:
    PlayerInner();
    ~PlayerInner();

    /**
     * set source url
     */
    void setSourceUrl(char *source_url);

    /**
     * get source url
     */
    char * getSourceUrl();

private:

    //-----------*******************-------------
    //          private member function
    //-----------*******************-------------

    /**
     * Thread 1 the corresponding Central Engine StateMachine
     */
    void *central_engine_thread(void *arg);

    /**
     * Thread 2 the corresponding Video Decode StateMachine
     */
    void *video_decode_thread(void *arg);

    /**
     * Thread 3 the corresponding audio Decode StateMachine
     */
    void *audio_decode_thread(void *arg);

    /**
     * Thread 4 the corresponding video Render StateMachine
     */
     void *video_render_thread(void *arg);



    /**
     * open input file
     *
     * if open file success ,and get media info then return true.
     */
     bool open_file();



    //-----------*******************-------------
    //          private member variable
    //-----------*******************-------------

    /**
     * source url
     */
    char *source_url;

    /**
     * duration
     */
    unsigned long *duration;


public:
    //*************   input file information  *****************
    /**
     * format context for input file
     */
    AVFormatContext *format_context;

    /**
     * audio stream
     */
    AVStream *audio_stream;

    /**
     * video stream
     */
    AVStream *video_stream;

    /**
     * duration
     */
    int stream_index[AVMEDIA_TYPE_NB];

    /**
     * audio channels
     */
    int audio_channels_cnt;

    //el_player_engine_AV_support_e av_support;   // 本文件类型: 音频、视频、音视频


};


#endif //XPLAYER_PLAYERINNER_H
