//
// Created by chris on 10/14/16.
// Packet Queue for audio packet & video packet.
//

#ifndef XPLAYER_PACKETQUEUE_H
#define XPLAYER_PACKETQUEUE_H


#include <pthread.h>

extern "C" {

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"

} // end of extern C

#include "util/cm_std.h"


class PacketQueue
{
public:
    PacketQueue();
    ~PacketQueue();

    /**
    * cleanup the packet queue.
    */
    void flush();

    /**
    * put packet into the packet queue.
    */
    int put(AVPacket* pkt);

    /**
    * get packet from the packet queue.
    * return < 0 if aborted, 0 if no packet and > 0 if packet.
    */
    int get(AVPacket *pkt, bool block);

    int size();

    void abort();



    /**
    * get buffered packet sum duration in packet queue.
    */
    int64_t get_buffer_packet_ts();

    unsigned long long q_size;

private:

    /**
    * get first packet pts in packet queue.
    */
    int64_t get_first_pkt_pts();

    /**
    * get last packet pts in packet queue
    */
    int64_t get_last_pkt_pts();


    /**
    * first packet in packet queue.
    */
    AVPacketList*   first_packet;

    /**
    * last packet in packet queue.
    */
    AVPacketList*   last_packet;

    int nb_packets;

    //unsigned long long q_size;
    int abort_request;

    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

#endif //XPLAYER_PACKETQUEUE_H
