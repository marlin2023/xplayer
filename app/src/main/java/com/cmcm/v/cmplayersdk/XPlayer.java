package com.cmcm.v.cmplayersdk;

import android.content.Context;
import android.graphics.Bitmap;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.SurfaceHolder;

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
public class XPlayer implements IMediaPlayer{

    public static String TAG = "XPlayer";


    /**
     * Event Handle for event from native.
     */
    EventHandler mEventHandler;

    /**
     * OnPreparedListener for XPlayer
     */
    private IMediaPlayer.OnPreparedListener mOnPreparedListener;


    static {
        try {
            System.loadLibrary("ffmpeg");
            System.loadLibrary("cmxplayer");

        } catch(Exception e) {
            e.printStackTrace(System.out);
        }

        //
        _native_init();
    }

    public XPlayer() {
        // new player object ,and get player handle.
        Looper looper;
        if ((looper = Looper.myLooper()) != null) {
            mEventHandler = new EventHandler(this, looper);
        } else if ((looper = Looper.getMainLooper()) != null) {
            mEventHandler = new EventHandler(this, looper);
        } else {
            mEventHandler = null;
        }

        mOnPreparedListener = null;


        //native_setup();
        _native_setup(new WeakReference<XPlayer>(this));
    }

    @Override
    public void setDisplay(SurfaceHolder sh) {

    }

    @Override
    public void setDataSource(String path) {
        _setDataSource(path);
    }

    @Override
    public void prepareAsync() throws IllegalStateException {
        _prepareAsync();
    }

    @Override
    public String getDataSource() {
        return null;
    }

    @Override
    public void start() throws IllegalStateException {

    }

    @Override
    public void stop() throws IllegalStateException {

    }

    @Override
    public void pause() throws IllegalStateException {

    }

    @Override
    public void setScreenOnWhilePlaying(boolean screenOn) {

    }

    @Override
    public int getVideoWidth() {
        return 0;
    }

    @Override
    public int getVideoHeight() {
        return 0;
    }

    @Override
    public boolean isPlaying() {
        return false;
    }

    @Override
    public void seekTo(long msec) throws IllegalStateException {

    }

    @Override
    public long getCurrentPosition() {
        return 0;
    }

    @Override
    public long getDuration() {
        return 0;
    }

    @Override
    public void release() {

    }

    @Override
    public void reset() {

    }

    @Override
    public void setVolume(float leftVolume, float rightVolume) {

    }

    @Override
    public int getAudioSessionId() {
        return 0;
    }

    @Override
    public void setLogEnabled(boolean enable) {

    }

    @Override
    public boolean isPlayable() {
        return false;
    }

    @Override
    public void setOnPreparedListener(OnPreparedListener listener) {
        mOnPreparedListener = listener;
    }

    @Override
    public void setOnCompletionListener(OnCompletionListener listener) {

    }

    @Override
    public void setOnBufferingUpdateListener(OnBufferingUpdateListener listener) {

    }

    @Override
    public void setOnSeekCompleteListener(OnSeekCompleteListener listener) {

    }

    @Override
    public void setOnVideoSizeChangedListener(OnVideoSizeChangedListener listener) {

    }

    @Override
    public void setOnErrorListener(OnErrorListener listener) {

    }

    @Override
    public void setOnInfoListener(OnInfoListener listener) {

    }

    @Override
    public void resetListeners() {

    }

    @Override
    public void setAudioStreamType(int streamtype) {

    }

    @Override
    public void setKeepInBackground(boolean keepInBackground) {

    }

    @Override
    public int getVideoSarNum() {
        return 0;
    }

    @Override
    public int getVideoSarDen() {
        return 0;
    }

    @Override
    public void setWakeMode(Context context, int mode) {

    }

    @Override
    public void setLooping(boolean looping) {

    }

    @Override
    public boolean isLooping() {
        return false;
    }

    @Override
    public int getIsLiveVideo() throws IllegalStateException {
        return 0;
    }

    @Override
    public String getHttpInfoStr() throws IllegalStateException {
        return null;
    }

    @Override
    public String getLibCompileTime() {
        return null;
    }

    @Override
    public Bitmap getCurrentFrame() {
        return null;
    }

    /**
     * receive native message.
     */
    private static void postEventFromNative(Object weakThiz, int what, int arg1, int arg2, Object obj)
    {

        XPlayer mp = (XPlayer) ((WeakReference<?>) weakThiz).get();
        if (mp == null)
        {
            Log.e(TAG ,"===>in postEventFromNative ,but mp equal null.");
            return;
        }
        if (mp.mEventHandler != null)
        {
            Message m = mp.mEventHandler.obtainMessage(what, arg1, arg2, obj);
            mp.mEventHandler.sendMessage(m);
        }

    }


    private static final int MEDIA_NOP = 0; // interface test message
    private static final int MEDIA_PREPARED = 1;
    private static final int MEDIA_PLAYBACK_COMPLETE = 2;
    private static final int MEDIA_BUFFERING_UPDATE = 3;
    private static final int MEDIA_SEEK_COMPLETE = 4;
    private static final int MEDIA_SET_VIDEO_SIZE = 5;
    private static final int MEDIA_ERROR = 100;
    private static final int MEDIA_INFO = 200;

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
                case MEDIA_PREPARED:
                    Log.e(TAG ,"====>on ...MEDIA_PREPARED");
                    if (mOnPreparedListener != null){
                        mOnPreparedListener.onPrepared(mMediaPlayer); // TODO call player start function ,on jni start function will send according to EVT message.
                    }
                    return;
                case MEDIA_PLAYBACK_COMPLETE:
//                    if (mOnCompletionListener != null)
//                        mOnCompletionListener.onCompletion(mMediaPlayer);
//                    stayAwake(false);
                    return;

                case MEDIA_BUFFERING_UPDATE:
//                    if (mOnBufferingUpdateListener != null)
//                        mOnBufferingUpdateListener.onBufferingUpdate(mMediaPlayer, msg.arg1);
                    return;

                case MEDIA_SEEK_COMPLETE:
                    return;

                case MEDIA_SET_VIDEO_SIZE:
                    return;

                case MEDIA_ERROR:
                    Log.e(TAG, "Error (" + msg.arg1 + "," + msg.arg2 + ")");
                    return;

                case MEDIA_INFO:
                    Log.i(TAG, "Info (" + msg.arg1 + "," + msg.arg2 + ")");
                    return;

                case MEDIA_NOP: // interface test message - ignore
                    break;

                default:
                    Log.e(TAG, "Unknown message type " + msg.what);
                    return;
            }

        }
    }



    public static native void _native_init();

    /**
     * native setup function ,create mediaplayer.
     * Native setup requires a weak reference to our object.
     * It's easier to create it here than in C++.
     * @param mediaplayer_this
     */
    public native void _native_setup(Object mediaplayer_this); //(WeakReference<XPlayer> xPlayerWeakReference);

    /**
     * init xplayer .
     * 1. register all codecs and formats
     * 2. create need threads.
     */
    public native void _init();

    /**
     * init opengl context .
     */
    public native void _initEGLCtx();

    /**
     * set DataSource
     */
    public native void _setDataSource(String path);

    /**
     * prepare ,and get mediainfo if prepared success.
     */
    public native void _prepareAsync() throws IllegalStateException;


    public native void _renderFrame();

}
