//
// Created by chris on 12/1/16.
//

#include "XTimeUtil.h"
#include "XLog.h"

#define TAG "UTIL_XTimeUtil"

long long XTimeUtil::getCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);

    //XLog::d(ANDROID_LOG_WARN ,TAG ,"======>system clock time :%lld in millisecond.\n",(long long)tv.tv_sec*1000);
    return (long long)tv.tv_sec * 1000 + (long long)tv.tv_usec / 1000;
}
