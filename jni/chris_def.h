/*
 * chris_error.h
 *
 *  Created on: Jun 25, 2012
 *      Author: chris
 */

#ifndef CHRIS_ERROR_H_
#define CHRIS_ERROR_H_


#ifdef CHRIS
#define  chris_printf(format, ...) { \
				printf (format, ##__VA_ARGS__); \
				fflush (stdout); \
}
#else
	#define chris_printf(format ,...)
#endif



#define MEMORY_ALLOCATE_FAIL				0x01

#define FILE_OPEN_FAIL      				0x02
#define FIND_INFO_FAIL      				0x03

#define NO_VIDEO_STREAM						0x04
#define NO_AUDIO_STREAM						0x05

#define NO_FIND_VIDEO_DECODER				0x06
#define NO_FIND_AUDIO_DECODER				0x07

#define OPEN_VIDEO_DECODER_FAIL				0x08
#define OPEN_AUDIO_DECODER_FAIL				0x09

#define NULL_MEDIA_HADNLE					0x0a
#define DUP_PACKET_FAIL						0x0b

#define PACKET_QUEUE_GET_FAIL				0x0c

#define AUDIO_DECODER_FAIL					0x10

#define NULL_POINT							0x11


#endif /* CHRIS_ERROR_H_ */
