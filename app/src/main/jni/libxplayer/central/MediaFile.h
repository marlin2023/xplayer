//
// Created by chris on 10/17/16.
// Media File Object
// The inner object , all according to media information can get from here.
//

#ifndef XPLAYER_MEDIAFILE_H
#define XPLAYER_MEDIAFILE_H

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"

} // end of extern C

#include "util/cm_std.h"

#include "MediaFile.h"
#include "PacketQueue.h"

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



class MediaFile
{

public:

    MediaFile();
    ~MediaFile();

    /**
     * set source url
     */
    void setSourceUrl(char *source_url);

    /**
     * get source url
     */
    char * getSourceUrl();

    /**
    * OPEN input file
    *
    * if open file success ,and get media info then return CM_TRUE.
    */
    CM_BOOL open();


private:
    /**
    * OPEN given stream by stream_index
    * refer to function from ffplay.c
    */
    CM_BOOL stream_component_open(int stream_index);

    /**
    * Close given stream by stream_index
    * refer to function from ffplay.c
    */
    void stream_component_close(int stream_index);



private:
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

    /**
     * audio packet queue
     */
    PacketQueue *audio_queue;

    /**
    * video packet queue
    */
    PacketQueue *video_queue;




};

#endif //XPLAYER_MEDIAFILE_H
