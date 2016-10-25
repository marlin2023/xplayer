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
#include "FrameQueue.h"

#define x_min(a,b) (((a) < (b)) ? (a):(b))
#define x_max(a,b) (((a) > (b)) ? (a):(b))
#define x_abs(a) (((a) >= 0) ? (a) : -(a))


#define X_S_2_MS(sec) ((sec) * 1000)
#define X_MS_2_US(msec) ((msec) * 1000)

#define X_MAX_PKT_Q_NETWORK_FIRST_BUFFERING_TS X_S_2_MS(3)        // in ms unit
#define X_MAX_PKT_Q_NETWORK_BUFFERING_TS X_S_2_MS(6)              // in ms unit

#define X_MAX_PKT_Q_TS         X_S_2_MS(0.6)                  // in ms unit
#define X_MAX_PKT_Q_NETWORK_TS X_S_2_MS(40)                   // in ms unit

#define X_MAX_FRAME_VIDEO_Q_NODE_CNT 20                         // max frm q video frame count

#define X_MAX_FRAME_Q_TIME_LIMIT (0.7)                          // s

#define X_MAX_FRAME_AUDIO_Q_NODE_CNT 40                         // max frm q audio frame count

#define X_MEGA_SIZE(t) ((t) * 1024 * 1024)
#define X_MAX_FRAME_VIDEO_Q_MEM_SPACE  X_MEGA_SIZE(40)              // 40M space size

#define X_MAX_PKT_VIDEO_Q_MEM_SPACE  X_MEGA_SIZE(10)                // 10M space size
#define X_MAX_PKT_AUDIO_Q_MEM_SPACE  X_MEGA_SIZE(5)                 // 5M space size


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
    void setSourceUrl(const char *source_url);

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

    /**
    * judge if the packet queue is full by the input parameter ts.
    */
    CM_BOOL is_pkt_q_full(int64_t buffer_ts_duration);

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
     * audio codec context
     */
    AVCodecContext  *audio_codec_context ;

    /**
     * video codec context
     */
    AVCodecContext  *video_codec_context ;


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

    /**
     * audio frame queue
     */
    FrameQueue *audio_frame_queue;

    /**
    * video frame queue
    */
    FrameQueue *video_frame_queue;


    //
    /**
    * first play ,only need to buffer start_playing_buffering_time data.
    */
    long long start_playing_buffering_time;

    /**
    * max buffering time.
    */
    long long max_buffering_time;

    /**
    * buffering percent.
    */
    int buffering_percent;


};

#endif //XPLAYER_MEDIAFILE_H
