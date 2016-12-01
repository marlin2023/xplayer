//
// Created by chris on 10/26/16.
// Refer to:
//          https://gitlab.tubit.tu-berlin.de/justus.beyer/streamagame_platform_frameworks_base/blob/d670b8c61ebd13324ac21bdbc08d8a02fc0a765a/include/media/mediaplayer.h

#ifndef XPLAYER_MEDIAPLAYERLISTENER_H
#define XPLAYER_MEDIAPLAYERLISTENER_H

class MediaPlayerListener
{
public:
    virtual void notify(int msg, int ext1, int ext2) = 0;

    virtual void JNIStopGlRenderMode() = 0;
    virtual void JNIStartGlRenderMode() = 0;

    virtual void JNI2BufferState() = 0;

    /**
     * create opensl es object for audio mix out
     */
     virtual void JNICreateAudioMixObj() = 0;
};

#endif //XPLAYER_MEDIAPLAYERLISTENER_H
