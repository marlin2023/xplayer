#include <stdio.h>

#include <unistd.h>

#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <android/log.h>

#include "player.h"
#include "chris_def.h"

/*淇濆瓨涓�抚涓簆pm*/
void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame) {
	FILE *pFile;
	char szFilename[32];
	int  y;

	// 鎵撳紑ppm鏂囦欢
	sprintf(szFilename, "frame%d.ppm", iFrame);
	pFile=fopen(szFilename, "wb");
	if(pFile==NULL)
		return;

	//浜沺pm澶撮儴淇℃伅<3琛�
	fprintf(pFile, "P6\n%d %d\n255\n", width, height);

	//鍐檖pm 姝ｆ枃
	for(y=0; y<height; y++)
		fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);

	// Close file
	fclose(pFile);
}

static int malloc_rgb565_mem( media_handle_union_t * media_handle1 ){

	media_handle_union_t * media_handle = media_handle1;
	//video
	media_handle->img_convert_ctx = sws_getContext(
			media_handle->ptr_video_codec_ctx->width,
			media_handle->ptr_video_codec_ctx->height,
			media_handle->ptr_video_codec_ctx->pix_fmt,
			media_handle->ptr_video_codec_ctx->width, media_handle->ptr_video_codec_ctx->height,
			/*PIX_FMT_RGB24*/PIX_FMT_RGB565LE, SWS_BICUBIC, NULL, NULL, NULL);  //little endian

	media_handle->yuv_frame = avcodec_alloc_frame();
	if(media_handle->yuv_frame == NULL){
		chris_printf("yuv_frame allocate failed %s ,%d line\n" ,__FILE__ ,__LINE__);
		exit(MEMORY_ALLOCATE_FAIL);
	}


	media_handle->decoded_frame = avcodec_alloc_frame();
	if(media_handle->decoded_frame == NULL){
		chris_printf("pFrameRGB565 allocate failed %s ,%d line\n" ,__FILE__ ,__LINE__);
		exit(MEMORY_ALLOCATE_FAIL);
	}

	int width = media_handle->ptr_video_codec_ctx->width;
	int height = media_handle->ptr_video_codec_ctx->height;
	int num_bytes = avpicture_get_size(/*PIX_FMT_RGB24*/PIX_FMT_RGB565LE, width, height);

	//malloc memory
	media_handle->decoded_video_buf = (uint8_t *)av_malloc(num_bytes * sizeof(uint8_t));
	if(media_handle->decoded_video_buf == NULL){
		chris_printf("buffer av_malloc failed %s ,%d line \n" , __FILE__ ,__LINE__);
		exit( MEMORY_ALLOCATE_FAIL );
	}

	//bind
	avpicture_fill((AVPicture *)media_handle->decoded_frame, media_handle->decoded_video_buf, /*PIX_FMT_RGB24*/PIX_FMT_RGB565LE, width, height);

	return 0;
}


/* packet queue handling */
static void packet_queue_init(PacketQueue *q) {
	memset(q, 0, sizeof(PacketQueue));

	q->first_pkt = q->last_pkt = NULL;
	q->nb_packets = 0;

	pthread_mutex_init(&q->mutex, NULL);
	pthread_cond_init(&q->cond, NULL);

	pthread_cond_init(&q->full_cond, NULL);
}

static int packet_queue_put(PacketQueue *q, AVPacket *pkt) {
	AVPacketList *pkt1;


	/* duplicate the packet */
	if (av_dup_packet(pkt) < 0)
		return DUP_PACKET_FAIL;

	pkt1 = av_malloc(sizeof(AVPacketList));
	if (!pkt1)
		return MEMORY_ALLOCATE_FAIL;
	pkt1->pkt = *pkt;
	pkt1->next = NULL;
	pthread_mutex_lock(&q->mutex);

	if(q->nb_packets == q->packet_max_num){

		pthread_cond_wait(&q->full_cond, &q->mutex);

	}
	if (!q->last_pkt) //empty queue
		q->first_pkt = pkt1;
	else
		q->last_pkt->next = pkt1;
	q->last_pkt = pkt1;
	q->nb_packets++;

	pthread_cond_signal(&q->cond); //have data

	pthread_mutex_unlock(&q->mutex);
	//printf("after put into queue..锛宯b_packet = %d\n" ,q->nb_packets);
	return 0;
}

static int packet_queue_get(PacketQueue *q, AVPacket *pkt ,int block) {
	AVPacketList *pkt1;
	int ret;


	pthread_mutex_lock(&q->mutex);
	//printf("in packet queue get ...-------------------------------------\n");
	for(;; ){
		pkt1 = q->first_pkt;
		if (pkt1) {
			//printf("###hava data in queue..\n");
			q->first_pkt = pkt1->next;
			if (!q->first_pkt)
				q->last_pkt = NULL;
			q->nb_packets--;
			*pkt = pkt1->pkt;
			av_free(pkt1);

			pthread_cond_signal(&q->full_cond);
			ret = 0;
			break;
		} else if ( block) {
		      ret = -1;
		      break;
		} else {
			//empty queue
			pthread_cond_wait(&q->cond, &q->mutex);

		}
	}
	pthread_mutex_unlock(&q->mutex);
	return ret;
}

static int  empty_queue(PacketQueue *q){
	AVPacketList *pkt1;

	pthread_mutex_lock(&q->mutex);
	for (;;) {
		pkt1 = q->first_pkt;
		if (pkt1) {
			//printf("###hava data in queue..\n");
			q->first_pkt = pkt1->next;
			if (!q->first_pkt)
				q->last_pkt = NULL;
			q->nb_packets--;
			av_free_packet(&pkt1->pkt);
			av_free(pkt1);
			if(q->nb_packets == 0) break;
		}
	}
//	q->first_pkt = q->last_pkt = NULL;
//		q->nb_packets = 0;
	pthread_mutex_unlock(&q->mutex);
	return 0;
}



/* init fuction ,return a Handle for the input file*/
PLAYER_HANDLE init_media(char *media_src ,int *src_width ,int *src_height ,int *sample) {

	media_handle_union_t * media_handle;
	if ((media_handle = malloc(sizeof(media_handle_union_t))) == NULL) {

		chris_printf("chris: memory allocate failed %s ,%d line \n" ,__FUNCTION__ ,__LINE__);
		exit(MEMORY_ALLOCATE_FAIL);
	}

	av_register_all();

	/* register network */
	avformat_network_init();

	media_handle->ptr_format_ctx = NULL;
	if(avformat_open_input(&media_handle->ptr_format_ctx, media_src, NULL,
			NULL) < 0){
			__android_log_print(ANDROID_LOG_INFO,"chris_magic", "av_open_input_file failed" );
			exit(FILE_OPEN_FAIL);

	}

	if (av_find_stream_info(media_handle->ptr_format_ctx) < 0) {
		chris_printf("av_find_stream_info failed in init function in %s line %d \n" ,__FILE__ ,__LINE__);
		exit(FIND_INFO_FAIL);
	}

	//鑾峰彇鏃堕暱
	__android_log_print(ANDROID_LOG_INFO,"chris_magic", "........the duration of the film is %lld \n" ,
			media_handle->ptr_format_ctx->start_time / AV_TIME_BASE );

	__android_log_print(ANDROID_LOG_INFO,"chris_magic", "duration %lld " ,media_handle->ptr_format_ctx->duration );

	/*printf the stream info */
	av_dump_format(media_handle->ptr_format_ctx , 0 ,media_src ,0);

	/* start to find audio stream && video stream */
	int i;
	media_handle->video_stream = -1;
	media_handle->audio_stream = -1;
	for (i = 0; i < media_handle->ptr_format_ctx->nb_streams; i++) {

		if (media_handle->ptr_format_ctx->streams[i]->codec->codec_type
				== AVMEDIA_TYPE_VIDEO && media_handle->video_stream < 0) {
			media_handle->video_stream = i;
		}

		if (media_handle->ptr_format_ctx->streams[i]->codec->codec_type
				== AVMEDIA_TYPE_AUDIO && media_handle->audio_stream < 0) {
			media_handle->audio_stream = i;
		}
	}

	if (media_handle->video_stream < 0) {
		chris_printf("do not find video stream in file: %s ,%d\n" ,__FILE__ ,__LINE__);
		exit(NO_VIDEO_STREAM);
	}

	if (media_handle->audio_stream < 0) {
		chris_printf("do not find audio stream .file :%s ,%d \n" ,__FILE__ ,__LINE__);
		exit(NO_AUDIO_STREAM);
	}

	/* in here ,I have acquire the audio info and  video info */
	/* video	*/
	media_handle->ptr_video_codec_ctx =
			media_handle->ptr_format_ctx->streams[media_handle->video_stream]->codec;
	if ((media_handle->ptr_video_codec = avcodec_find_decoder(
			media_handle->ptr_video_codec_ctx->codec_id)) == NULL) {
		chris_printf("find video decoder failed ,file %s ,%d line\n" ,__FILE__ ,__LINE__);
		exit(NO_FIND_VIDEO_DECODER);
	}

	// open video decoder
	if (avcodec_open(media_handle->ptr_video_codec_ctx,
			media_handle->ptr_video_codec) < 0) {
		chris_printf("open video decoder failed file %s ,%d line\n" ,__FILE__ ,__LINE__);
		exit(OPEN_VIDEO_DECODER_FAIL);
	}

	/* audio  */
	media_handle->ptr_audio_codec_ctx =
			media_handle->ptr_format_ctx->streams[media_handle->audio_stream]->codec;
	if ((media_handle->ptr_audio_codec = avcodec_find_decoder(
			media_handle->ptr_audio_codec_ctx->codec_id)) == NULL) {
		chris_printf("find audio decoder failed ,%s file ,%d line\n" ,__FILE__ ,__LINE__);
		exit(NO_FIND_AUDIO_DECODER);
	}

	// open audio decoder
	if (avcodec_open(media_handle->ptr_audio_codec_ctx,
			media_handle->ptr_audio_codec) < 0) {
		chris_printf("open audio decoder failed file %s ,%d line\n" ,__FILE__ ,__LINE__);
		exit(OPEN_AUDIO_DECODER_FAIL);
	}

	//init packet_queue
	packet_queue_init(&media_handle->audio_queue);
	media_handle->audio_queue.packet_max_num = AUDIO_PACKET_MAX_NUM;
	packet_queue_init(&media_handle->video_queue);
	media_handle->video_queue.packet_max_num = VIDEO_PACKET_MAX_NUM;

	//function
	malloc_rgb565_mem( media_handle );

	//audio frame
	media_handle->audio_decode_frame = avcodec_alloc_frame();
	if(media_handle->audio_decode_frame == NULL){
		chris_printf("audio_decode_frame allocate failed %s ,%d line\n" ,__FILE__ ,__LINE__);
		exit(MEMORY_ALLOCATE_FAIL);
	}

	media_handle->audio_pkt_size = 0;
	media_handle->file_over  = 0;
	media_handle->stop_mark = 0;
	media_handle->seek_mark = 0;


	int n = media_handle->ptr_audio_codec_ctx->channels * 2; //杩欓噷鐨� 搴旇鏄噺鍖栨椂鍊欎娇鐢�6bits鑰屾潵
	media_handle->bytes_per_sec = media_handle->ptr_audio_codec_ctx->sample_rate * n;
	media_handle->frame_rgb565_size = media_handle->ptr_video_codec_ctx->width
									* media_handle->ptr_video_codec_ctx->height * 2;

	/*	璁剧疆鐨勪笁涓弬鏁�*/
	*src_width = media_handle->ptr_video_codec_ctx->width;
	*src_height = media_handle->ptr_video_codec_ctx->height;
	*sample = media_handle->ptr_audio_codec_ctx->sample_rate;

	av_init_packet(&media_handle->flush_pkt);
	media_handle->flush_pkt.data = "FLUSH";

	__android_log_print(ANDROID_LOG_INFO,"chris_magic", "in init ,media_handle->seek_mark =%d." ,media_handle->seek_mark );
	return (PLAYER_HANDLE) media_handle;
}



/* read data thread*/
int read_thread(PLAYER_HANDLE handle ) {

	media_handle_union_t * media_handle = (media_handle_union_t *) handle;
	if (media_handle == NULL) {
		chris_printf("media handle is null , %s ,%d line\n" ,__FILE__ ,__LINE__);
		exit(NULL_MEDIA_HADNLE);
	}

	AVPacket packet;
	/* start to read the packet from the media file */
	while (av_read_frame(media_handle->ptr_format_ctx, &packet) >= 0) {

		if (packet.stream_index == media_handle->video_stream) {
			//video
			packet_queue_put(&media_handle->video_queue, &packet);
			//__android_log_print(ANDROID_LOG_INFO,"chris_magic", ".....put into queue ,video: %d " ,media_handle->video_queue.nb_packets);

		} else if (packet.stream_index == media_handle->audio_stream) {
			//audio
			packet_queue_put(&media_handle->audio_queue, &packet);
			//__android_log_print(ANDROID_LOG_INFO,"chris_magic", "put into queue ,audio: %d" ,media_handle->audio_queue.nb_packets);
		}


		//judge seek or not
		if(media_handle->seek_mark == 1){

			__android_log_print(ANDROID_LOG_INFO,"chris_magic", "......ffmpeg seek frame ..");
			//
			int64_t seek_target = media_handle->ptr_format_ctx->duration * media_handle->seek_pos / 100;
			seek_target += media_handle->ptr_format_ctx->start_time;
			int ret1 = av_seek_frame(media_handle->ptr_format_ctx,
						-1,	//杩欓噷蹇呴』鏄�1鎵嶅ソ浣�鐢╲ideoStream鐨勮瘽 涓嶅ソ浣�...
						seek_target , AVSEEK_FLAG_BACKWARD);


			//empty video queue and audio queue
			empty_queue(&media_handle->video_queue);
			empty_queue(&media_handle->audio_queue);
			//plus
			packet_queue_put(&media_handle->audio_queue ,&media_handle->flush_pkt);
			packet_queue_put(&media_handle->video_queue ,&media_handle->flush_pkt);
			media_handle->seek_mark = 0;

			__android_log_print(ANDROID_LOG_INFO,"chris_magic", "......ffmpeg seek over..");

			continue;
		}else if(media_handle->stop_mark == 1 ){


			__android_log_print(ANDROID_LOG_INFO,"chris_magic", "......ffmpeg game. over..");
			break;
		}

		// only after decode ,you can call av_free_packet(&packet);else you can get error  from decoder "no frame"
	}

	//set file_over_mark
	__android_log_print(ANDROID_LOG_INFO,"chris_magic", "......ffmpeg game. over.1.");
	media_handle->file_over = 1;
	__android_log_print(ANDROID_LOG_INFO,"chris_magic", ".....file_over = %d." ,media_handle->file_over);
	return 0;
}


uint8_t * get_video_frame(PLAYER_HANDLE handle ,double * video_frame_pts ,uint8_t * video_mem_addr ,int back_play_mark) {

	media_handle_union_t * media_handle = (media_handle_union_t *) handle;
	if (media_handle == NULL) {
		chris_printf("media handle is null , %s ,%d line\n" ,__FILE__ ,__LINE__);
		exit(NULL_MEDIA_HADNLE);
	}

	static AVPacket pkt1;
	static AVPacket *pkt = &pkt1;

	for(;;){
		if (packet_queue_get(&media_handle->video_queue, pkt ,media_handle->file_over) < 0){
			exit( PACKET_QUEUE_GET_FAIL );
		}

		if(back_play_mark == 1){
			int64_t pts1 = pkt->pts;
			(*video_frame_pts)
					= (double) pts1
							* av_q2d(
									media_handle->ptr_format_ctx->streams[media_handle->video_stream]->time_base)
							* 1000;

			av_free_packet(pkt);
			return NULL;

		}

		if(pkt->data  == media_handle->flush_pkt.data ){
			avcodec_flush_buffers(media_handle->ptr_video_codec_ctx);
			__android_log_print(ANDROID_LOG_INFO,"chris_magic", ".....FLUSH....");
			continue;
		}

		//__android_log_print(ANDROID_LOG_INFO,"chris_magic", "before decode video pakcet.num = %d" ,media_handle->video_queue.nb_packets );
		int got_picture = 0;
		avcodec_decode_video2(media_handle->ptr_video_codec_ctx, media_handle->yuv_frame,
				&got_picture, pkt);

		if (got_picture) {
			//__android_log_print(ANDROID_LOG_INFO,"chris_magic", "..get a picture");
//			int64_t pts1 = media_handle->yuv_frame->best_effort_timestamp;
			int64_t pts1 = media_handle->yuv_frame->pkt_pts;
			media_handle->video_frame_pts =
					(double) pts1
							* av_q2d(
									media_handle->ptr_format_ctx->streams[media_handle->video_stream]->time_base)
							* 1000;

			(*video_frame_pts) = media_handle->video_frame_pts;

			media_handle->decoded_frame->data[0] = video_mem_addr;  //鐪佷竴姝opy 宸ヤ綔
			//here ,millisecond
			sws_scale(media_handle->img_convert_ctx ,
									(const uint8_t* const*)media_handle->yuv_frame->data ,
									media_handle->yuv_frame->linesize ,0 ,media_handle->ptr_video_codec_ctx->height ,
									media_handle->decoded_frame->data ,media_handle->decoded_frame->linesize);

			av_free_packet(pkt);  //add 20120905
			return NULL;

		}else{ // no data
			__android_log_print(ANDROID_LOG_INFO,"chris_magic", "in cache" );

		}

		av_free_packet(pkt);  //鎵撳紑銆傘�
	} //for(;;)
}


uint8_t * get_audio_frame(PLAYER_HANDLE handle  , int *audio_frame_size ,double * audio_frame_pts  ,uint8_t * audio_mem_addr ) {

	media_handle_union_t * media_handle = (media_handle_union_t *) handle;
	if (media_handle == NULL) {
		chris_printf("media handle is null , %s ,%d line\n" ,__FILE__ ,__LINE__);
		exit(NULL_MEDIA_HADNLE);
	}

	static int audio_buf_index = 0;
	static AVPacket pkt ;

	static FILE *ptr_audio_file ;

	for(;;) {
		while(media_handle->audio_pkt_size > 0) {
			int got_frame = 0;

			media_handle->audio_decode_frame->data[0] = audio_mem_addr;

			int len = avcodec_decode_audio4( media_handle->ptr_audio_codec_ctx, media_handle->audio_decode_frame, &got_frame, &pkt );

			if( len < 0 ) { //decode failed ,skip frame
					fprintf( stderr, "Error while decoding audio frame\n" );
					break;
			}

			media_handle->audio_pkt_size -= len;
			if( got_frame ) {
					//acquire the large of the decoded audio info...
					int data_size = av_samples_get_buffer_size(NULL, media_handle->ptr_audio_codec_ctx->channels,
							media_handle->audio_decode_frame->nb_samples,
							media_handle->ptr_audio_codec_ctx->sample_fmt, 1);

					media_handle->audio_size = data_size;  //audio data size
					if(audio_buf_index != 0){
						media_handle->audio_frame_pts += (double) ((audio_buf_index + 1) /media_handle->bytes_per_sec );
					}
					audio_buf_index += data_size;

					*audio_frame_size = data_size;
					*audio_frame_pts = media_handle->audio_frame_pts;

//					fwrite(media_handle->audio_decode_frame->data[0] ,1 ,data_size ,ptr_audio_file);
					memcpy(audio_mem_addr ,media_handle->audio_decode_frame->data[0] ,data_size);

					return NULL;


			}else{ //no data

					 chris_printf("no data\n");
					 media_handle->audio_pkt_size = 0;
					 if( pkt.data)
						av_free_packet(&pkt);
					 continue;
			}
		} //end while


//		if( pkt.data)
//			av_free_packet(&pkt);

		/* next packet */
		if(packet_queue_get(&media_handle->audio_queue, &pkt ,media_handle->file_over) < 0) {
			exit(-1);
		}

		if(pkt.data == media_handle->flush_pkt.data ){
			avcodec_flush_buffers(media_handle->ptr_audio_codec_ctx);
			__android_log_print(ANDROID_LOG_INFO,"chris_magic", ".....FLUSH audio....");
			continue;
		}

		audio_buf_index = 0;
		int64_t pts1 = pkt.pts;
		media_handle->audio_frame_pts =
				(double) pts1
						* av_q2d(
								media_handle->ptr_format_ctx->streams[media_handle->audio_stream]->time_base)
						* 1000;

		media_handle->audio_pkt_size = pkt.size;
	}
}


int player_stop(PLAYER_HANDLE handle){
	media_handle_union_t * media_handle = (media_handle_union_t *) handle;
	if (media_handle == NULL) {
		chris_printf("media handle is null , %s ,%d line\n" ,__FILE__ ,__LINE__);
		exit(NULL_MEDIA_HADNLE);
	}


	media_handle->stop_mark = 1;

	for(;;){

		//__android_log_print(ANDROID_LOG_INFO,"chris_magic", "stop_mark");

		usleep(50);
		if( media_handle->file_over == 1){
			__android_log_print(ANDROID_LOG_INFO,"chris_magic", "stop_mark = 1");

			break;
		}
	}

	__android_log_print(ANDROID_LOG_INFO,"chris_magic", "stop_mark = 1");
	return 0;
}

int player_release(PLAYER_HANDLE handle){

	media_handle_union_t * media_handle = (media_handle_union_t *) handle;
	if (media_handle == NULL) {
		chris_printf("media handle is null , %s ,%d line\n" ,__FILE__ ,__LINE__);
		exit(NULL_MEDIA_HADNLE);
	}
	__android_log_print(ANDROID_LOG_INFO,"chris_magic", "0000.");

	sws_freeContext(media_handle->img_convert_ctx);

	__android_log_print(ANDROID_LOG_INFO,"chris_magic", "11111111...");
	//empty the audio queue && video queue
	empty_queue(&media_handle->video_queue);
	empty_queue(&media_handle->audio_queue);
	__android_log_print(ANDROID_LOG_INFO,"chris_magic", "2222 ...");
	return 0;
}


int seek_frame(PLAYER_HANDLE handle ,int seek_pos){
	media_handle_union_t * media_handle = (media_handle_union_t *) handle;
	if (media_handle == NULL) {
		chris_printf("media handle is null , %s ,%d line\n" ,__FILE__ ,__LINE__);
		exit(NULL_MEDIA_HADNLE);
	}

//	int ret1 = av_seek_frame(media_handle->ptr_format_ctx,
//				-1,	//杩欓噷蹇呴』鏄�1鎵嶅ソ浣�鐢╲ideoStream鐨勮瘽 涓嶅ソ浣�...
//				145 , AVSEEK_FLAG_BACKWARD);
//
//	printf(".................................ret1 = %d \n" ,ret1);
//	avcodec_flush_buffers(media_handle->ptr_video_codec_ctx);
//	avcodec_flush_buffers(media_handle->ptr_audio_codec_ctx);
	__android_log_print(ANDROID_LOG_INFO,"chris_magic", "...before set seek_mark ,seek_pos = %d" ,seek_pos);
	media_handle->seek_mark = 1;
	media_handle->seek_pos = seek_pos;

	__android_log_print(ANDROID_LOG_INFO,"chris_magic", "...after set seek_mark ,seek_pos = %d" ,seek_pos);
	return 0;
}
