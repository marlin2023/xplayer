/*
 * com_chris_yanlong_player.c
 *
 *  Created on: Jul 24, 2012
 *      Author: chris
 */

#include "com_chris_yanlong_PlayerView.h"
#include "player.h"
#include "chris_def.h"

#include <android/log.h>

/*
 * Class:     com_chris_yanlong_player
 * Method:    init_media
 * Signature: (Ljava/lang/String;Ljava/lang/Integer;Ljava/lang/Integer;)I
 */
JNIEXPORT jint JNICALL Java_com_chris_yanlong_PlayerView_init_1media
  (JNIEnv *env, jobject obj, jstring url, jobject width_src, jobject height_src, jobject sample_src){

	char *input_url = NULL;
	input_url = (char*)(*env)->GetStringUTFChars(env, url, NULL);

	__android_log_print(ANDROID_LOG_INFO,"chris_magic", "open file +%s+ " ,input_url);

	int width = 0;
	int height = 0;
	int audio_sample = 0;

	PLAYER_HANDLE media_handle = init_media(input_url ,&width ,&height ,&audio_sample);
	__android_log_print(ANDROID_LOG_INFO,"chris_magic", "after function init_media width = %d ,height = %d ,audio_sample = %d "
			"media_handle = %d"
				,width ,height ,audio_sample ,media_handle);

	jclass c;
	jfieldID id;
	c = (*env)->FindClass(env ,"java/lang/Integer");
	if (c == NULL)
	{
	   __android_log_print(ANDROID_LOG_INFO,"chris_magic", "jclass is null");
	   return -1;
	}

	id = (*env)->GetFieldID( env ,c, "value", "I");
	if (id == NULL)
	{
	   __android_log_print(ANDROID_LOG_INFO,"chris_magic", "id is null");
	   return -1;
	}

	(*env)->SetIntField(env ,width_src, id, width);
	(*env)->SetIntField(env ,height_src, id, height);
	(*env)->SetIntField(env ,sample_src, id, audio_sample);
	return (jint)(media_handle);

}

/*
 * Class:     com_chris_yanlong_player
 * Method:    decode_thread
 * Signature: (I[BLjava/lang/Integer;Ljava/lang/Double;)I
 */
JNIEXPORT jint JNICALL Java_com_chris_yanlong_PlayerView_read_1thread
  (JNIEnv *env, jobject obj, jint handle ){

	read_thread((int)handle );

}

/*
 * Class:     com_chris_yanlong_player
 * Method:    get_video_frame
 * Signature: (ILjava/lang/Double;)[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_chris_yanlong_PlayerView_get_1video_1frame
  (JNIEnv *env, jobject obj, jint handle , jobject video_pts ,jobject byteBuffer ,jint back_play_mark){

	jbyte *directBuffer = (*env)->GetDirectBufferAddress(env, byteBuffer);

	media_handle_union_t * media_handle = (media_handle_union_t *) handle;

//	 __android_log_print(ANDROID_LOG_INFO,"chris_magic", "video packet.num = %d ,audio packet.num = %d" ,media_handle->video_queue.nb_packets
//			 	 ,media_handle->audio_queue.nb_packets);
	jclass c;
	jfieldID id;
	c = (*env)->FindClass(env ,"java/lang/Double");
	if (c == NULL)
	{
	   __android_log_print(ANDROID_LOG_INFO,"chris_magic", "jclass is null");
	  exit(NULL_POINT);
	   //return -1;
	}

	id = (*env)->GetFieldID( env ,c, "value", "D");
	if (id==NULL)
	{
	   __android_log_print(ANDROID_LOG_INFO,"chris_magic", "id is null");
//	   return -1;
	   exit(NULL_POINT);
	}
	double video_frame_pts = 0;
	get_video_frame((int)handle , &video_frame_pts ,directBuffer ,(int)back_play_mark);


	(*env)->SetDoubleField(env ,video_pts, id, video_frame_pts);

	return NULL;
}

/*
 * Class:     com_chris_yanlong_player
 * Method:    get_audio_frame
 * Signature: (I)[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_chris_yanlong_PlayerView_get_1audio_1frame
	(JNIEnv *env, jobject obj, jint handle , jobject audio_pts , jobject audio_size ,jobject byteBuffer){

	jbyte *directBuffer = (*env)->GetDirectBufferAddress(env, byteBuffer);

	media_handle_union_t * media_handle = (media_handle_union_t *) handle;

	jclass c;
	jfieldID id;
	c = (*env)->FindClass(env ,"java/lang/Double");
	if (c == NULL)
	{
	   __android_log_print(ANDROID_LOG_INFO,"chris_magic", "jclass is null");
//	   return -1;
	   exit(NULL_POINT);
	}

	id = (*env)->GetFieldID( env ,c, "value", "D");
	if (id==NULL)
	{
	   __android_log_print(ANDROID_LOG_INFO,"chris_magic", "id is null");
//	   return -1;
	   exit(NULL_POINT);
	}

	double audio_frame_pts = 0;
	int audio_frame_size = 0;
	get_audio_frame((int)handle ,&audio_frame_size ,&audio_frame_pts  ,directBuffer);

	(*env)->SetDoubleField(env ,audio_pts, id, audio_frame_pts);

	jclass c1;
	jfieldID id1;
	c1 = (*env)->FindClass(env ,"java/lang/Integer");
	if (c1 == NULL)
	{
	   __android_log_print(ANDROID_LOG_INFO,"chris_magic", "jclass is null");
//	   return -1;
	   exit(NULL_POINT);
	}

	id1 = (*env)->GetFieldID( env ,c1, "value", "I");
	if (id1==NULL)
	{
	   __android_log_print(ANDROID_LOG_INFO,"chris_magic", "id is null");
//	   return -1;
	   exit(NULL_POINT);
	}
	(*env)->SetIntField(env ,audio_size, id1, audio_frame_size);

	jbyteArray result=NULL;
//	result= (*env)->NewByteArray(env ,media_handle->audio_size);
//
//	(*env)->SetByteArrayRegion(env ,result ,0 ,media_handle->audio_size ,media_handle->audio_buf );

	return result;

}


JNIEXPORT jint JNICALL Java_com_chris_yanlong_PlayerView_seek_1frame
  (JNIEnv *env, jobject obj, jint handle ,jint progress_bar){

	 __android_log_print(ANDROID_LOG_INFO,"chris_magic", "seek frame ...");
	seek_frame((int)handle ,progress_bar);
}

JNIEXPORT jint JNICALL Java_com_chris_yanlong_PlayerView_player_1stop
(JNIEnv *env, jobject obj, jint handle ){

	player_stop((int)handle);
}


JNIEXPORT jint JNICALL Java_com_chris_yanlong_PlayerView_player_1release
  (JNIEnv *env, jobject obj, jint handle ){

	__android_log_print(ANDROID_LOG_INFO,"chris_magic", "player release ...");
	player_release((int)handle);

}
