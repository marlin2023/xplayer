package com.cmcm.v.cmplayersdk;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;

import java.io.IOException;
import java.lang.ref.WeakReference;

/**
 * Created by chris on 9/26/16.
 * Refer to:
 *  1. https://github.com/commshare/testSparrow/blob/92ec6613f916d4575d4c78f61820108aeec22dcd/ref/FFmpegMediaPlayer/gradle/fmp-library/library/src/main/jni/player/mediaplayer.cpp
 *  2. https://github.com/IllusionLP/platform_frameworks_base/blob/da5223eaf23dda165962fccb1974425a941a82e1/core/jni/android_media_DeviceCallback.cpp
 *  3. https://github.com/commshare/testSparrow/blob/92ec6613f916d4575d4c78f61820108aeec22dcd/ref/FFmpegMediaPlayer/gradle/fmp-library/library/src/main/jni/player/wseemann_media_MediaPlayer.cpp
 *  4. Media 播放器 postEventFromNative  研究明白 再写这块。
 *      http://blog.csdn.net/llping2011/article/details/21239635
 *      postEventFromNative(): 传递来自Native的事件
 *      // MediaPlayer postEventFromNative
 */
public class XPlayer {

    /**
     * Event Handle for event from native.
     */
    EventHandler mEventHandler;

    static {
        try {
            System.loadLibrary("ffmpeg");
            System.loadLibrary("cmxplayer");

        } catch(Exception e) {
            e.printStackTrace(System.out);
        }
    }

    public XPlayer() {
        // new player object ,and get player handle.
        native_setup();
    }

    /**
     * native setup function ,create mediaplayer.
     */
    public native void native_setup();

    /**
     * init xplayer .
     * 1. register all codecs and formats
     * 2. create need threads.
     */
    public native void init();

    /**
     * set DataSource
     */
    public native void setDataSource(String path);

    /**
     * prepare ,and get mediainfo if prepared success.
     */
    public native void prepareAsync() throws IllegalStateException;


    public native void renderFrame();


    /**
     * receive native message.
     */
    private static void postEventFromNative(Object weakThiz, int what, int arg1, int arg2, Object obj)
    {

        XPlayer mp = (XPlayer) ((WeakReference<?>) weakThiz).get();
        if (mp == null)
        {
            return;
        }
        if (mp.mEventHandler != null)
        {
            Message m = mp.mEventHandler.obtainMessage(what, arg1, arg2, obj);
            mp.mEventHandler.sendMessage(m);
        }

    }

    private class EventHandler extends Handler
    {
        private XPlayer mMediaPlayer;

        public EventHandler(XPlayer ffmpegPlayer, Looper looper)
        {
            super(looper);
            mMediaPlayer = ffmpegPlayer;
        }

        @Override
        public void handleMessage(Message msg)
        {
            Log.e("xll","FFMPEG msg "+msg.what);
            //if (mMediaPlayer.mNativeContext == 0)
            {
               // return;
            }

            switch (msg.what)
            {
                case 0:

                    break;
                //case MEDIA_PREPARED:
                //    if (mOnPreparedListener != null)
                //        mOnPreparedListener.onPrepared(mMediaPlayer);
                //    return;

                //case MEDIA_PLAYBACK_COMPLETE:
                //    if (mOnCompletionListener != null)
                 //       mOnCompletionListener.onCompletion(mMediaPlayer);
                 //   return;

                default:
                    return;
            }

        }
    }

}
