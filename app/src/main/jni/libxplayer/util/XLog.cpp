//
// Created by chris on 9/26/16.
//

#include "XLog.h"

void XLog::d(int prio, const char *tag,  const char *fmt, ...)
{
    va_list ap;
    va_start(ap,fmt);
    if (mLogEnable){
        __android_log_vprint(prio,tag,fmt,ap);
    }
    va_end(ap);
}

void XLog::e(const char *tag,  const char *fmt, ...)
{
    va_list ap;
    va_start(ap,fmt);
    if (mLogEnable){
        __android_log_vprint(ANDROID_LOG_ERROR,tag,fmt,ap);
    }
    va_end(ap);
}

// if set mLogEnable to false ,
// then debug log will not be printed.
bool XLog::mLogEnable = true ;

