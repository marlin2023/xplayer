//
// Created by chris on 9/29/16.
//

#ifndef XPLAYER_PLAYERINNER_H
#define XPLAYER_PLAYERINNER_H

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"

} // end of extern C
#include "util/cm_std.h"


/**
 * streams type in file media.
 *
 */
typedef enum av_support_type_e_
{
    HAS_NONE = 0,
    HAS_AUDIO,      // has audio
    HAS_VIDEO,    // has video
    HAS_BOTH,     // has all
    HAS_END
} av_support_type_e;


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


public:

    void player_engine_init();

    /**
    * OPEN input file
    *
    * if open file success ,and get media info then return CM_TRUE.
    */
    CM_BOOL open_file();

    /**
    * OPEN given stream by stream_index
    *
    */
    CM_BOOL stream_component_open(int stream_index);

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

    /**
     * media file contain streams type
     */
    av_support_type_e av_support;


};


#endif //XPLAYER_PLAYERINNER_H
