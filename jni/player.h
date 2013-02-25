#ifndef PLAYER_H
#define PLAYER_H

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <pthread.h>

/* audio | video  packet queue*/
typedef struct PacketQueue {
    AVPacketList *first_pkt, *last_pkt;
    int nb_packets;

	pthread_mutex_t mutex;
	pthread_cond_t cond;

	pthread_cond_t full_cond;

	int packet_max_num;
} PacketQueue;


/*big structure for the player*/
typedef struct media_handle_union {

	/*AVFormatContext for input file*/
	AVFormatContext *ptr_format_ctx;

	int file_over;  //mark for read_packet frome file over..


	/*video */
	AVCodecContext *ptr_video_codec_ctx;
	AVCodec *ptr_video_codec;
	int video_stream;

	PacketQueue video_queue;
	struct SwsContext *img_convert_ctx;

	uint8_t *decoded_video_buf;
	AVFrame *decoded_frame;	//rgb565
	AVFrame *yuv_frame;

	int frame_rgb565_size;

	double video_frame_pts;



	/* audio */
	AVCodecContext *ptr_audio_codec_ctx;
	AVCodec *ptr_audio_codec;
	int audio_stream;

	PacketQueue audio_queue;
	int audio_pkt_size;
	AVFrame *audio_decode_frame;	//pcm

	double audio_frame_pts;
	int audio_size;
	int bytes_per_sec;


	//seek
	int seek_mark ;
	int seek_pos;   	//80 == 80%
	AVPacket flush_pkt;
	int stop_mark;

} media_handle_union_t;


#define VIDEO_PACKET_MAX_NUM			80
#define AUDIO_PACKET_MAX_NUM			80



/*
 * 	Function
 *
 * */
typedef int PLAYER_HANDLE;
/*
 * init fuction
 * @param[in] media_src the input file
 * return: Handle for the input file
 * */
PLAYER_HANDLE init_media(char *media_src ,int *src_width ,int *src_height ,int *sample);

/*
 * get video frame
 * @param[in] handle       the return value of the init_media
 * @param[in out] pts      the pts of the video frame
 *
 * return : the data of rgb565
 * */
uint8_t * get_video_frame(PLAYER_HANDLE handle ,double *video_frame_pts ,uint8_t * video_mem_addr ,int back_play_mark);


/*
 * get audio frame
 * @param[in] handle             the return value of the init_media
 * @param[in out] pts            the pts of the audio frame
 * @param[in out] audio_size     the size of the audio frame
 *
 * return : the data of audio ,such as pcm
 * */
uint8_t * get_audio_frame(PLAYER_HANDLE handle  , int *audio_frame_size ,double * audio_frame_pts  ,uint8_t * audio_mem_addr );

/*
 * the main thread to read data and put packet into queue
 * @param[in] handle       the return value of the init_media
 *
 * return : 0 success
 * */
int read_thread(PLAYER_HANDLE handle ) ;


/*
 *
 * @param[in] handle       the return value of the init_media
 *
 * return : 0 success
 * */
int player_stop(PLAYER_HANDLE handle);

/*
 *
 * @param[in] handle       the return value of the init_media
 *
 * return : 0 success
 * */
int player_release(PLAYER_HANDLE handle);

int seek_frame(PLAYER_HANDLE handle ,int seek_pos);




#endif
