//
// Created by chris on 9/26/16.
// custom log print for xplayer project.
//

#ifndef XPLAYER_XLOG_H
#define XPLAYER_XLOG_H

#include <android/log.h>

class XLog
{
public:

    /**
    * custom log print for debug program.
    */
    static void d(int prio, const char *tag,  const char *fmt, ...);

    /**
    * custom log print for error log print.
    */
    static void e(const char *tag,  const char *fmt, ...);

    /**
    * if mLogEnable is true ,the debug log will print out ,else will not be printed.
    */
    static bool mLogEnable;
};

#endif //XPLAYER_XLOG_H
