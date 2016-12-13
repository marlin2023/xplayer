//
// Created by chris on 9/26/16.
//

#include "XMutexlock.h"

XMutexLock::XMutexLock()
{
    pthread_mutex_init(&mutex, NULL);
}

XMutexLock::~XMutexLock()
{
    pthread_mutex_destroy(&mutex);
}

void XMutexLock::lock()
{
    pthread_mutex_lock(&mutex);

}

void XMutexLock::unlock()
{
    pthread_mutex_unlock(&mutex);
}

pthread_mutex_t* XMutexLock::get_mutex()
{
    return &mutex;
}