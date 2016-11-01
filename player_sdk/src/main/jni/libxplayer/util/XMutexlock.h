//
// Created by chris on 9/26/16.
// Custom mutex thread lock.
//

#ifndef XPLAYER_XMUTEXLOCK_H
#define XPLAYER_XMUTEXLOCK_H

#include <pthread.h>

class XMutexLock{

public:

    XMutexLock();
    ~XMutexLock();

    void lock();
    void unlock();

    pthread_mutex_t * get_mutex();

private:
    pthread_mutex_t mutex;
};
#endif //XPLAYER_XMUTEXLOCK_H
