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

private:

    /**
    * first packet in packet queue.
    */
    AVPacketList*   first_packet;

    /**
    * last packet in packet queue.
    */
    AVPacketList*   last_packet;

    int nb_packets;

    unsigned long long q_size;
    int abort_request;

    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

#endif //XPLAYER_PACKETQUEUE_H
