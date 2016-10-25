package com.cmcm.v.cmplayersdk;

import android.annotation.TargetApi;
import android.content.Context;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Build;
import android.view.Surface;
import android.view.SurfaceHolder;

import java.io.FileDescriptor;
import java.io.IOException;
import java.util.Map;

/**
 * Created by chris on 10/25/16.
 */

public interface IMediaPlayer {


    void setDisplay(SurfaceHolder sh);

    void setDataSource(String path);

    String getDataSource();

    void prepareAsync() throws IllegalStateException;

    void start() throws IllegalStateException;

    void stop() throws IllegalStateException;

    void pause() throws IllegalStateException;

    void setScreenOnWhilePlaying(boolean screenOn);

    int getVideoWidth();

    int getVideoHeight();

    boolean isPlaying();

    void seekTo(long msec) throws IllegalStateException;

    long getCurrentPosition();

    long getDuration();

    void release();

    void reset();

    void setVolume(float leftVolume, float rightVolume);

    int getAudioSessionId();

    @SuppressWarnings("EmptyMethod")
    @Deprecated
    void setLogEnabled(boolean enable);

    @Deprecated
    boolean isPlayable();

    void setOnPreparedListener(OnPreparedListener listener);

    void setOnCompletionListener(OnCompletionListener listener);

    void setOnBufferingUpdateListener(
            OnBufferingUpdateListener listener);

    void setOnSeekCompleteListener(
            OnSeekCompleteListener listener);

    void setOnVideoSizeChangedListener(
            OnVideoSizeChangedListener listener);

    void setOnErrorListener(OnErrorListener listener);

    void setOnInfoListener(OnInfoListener listener);

    void resetListeners();

    /*--------------------
     * Listeners
     */
    interface OnPreparedListener {
        void onPrepared(IMediaPlayer mp);
    }

    interface OnCompletionListener {
        void onCompletion(IMediaPlayer mp);
    }

    interface OnBufferingUpdateListener {
        void onBufferingUpdate(IMediaPlayer mp, int percent);
    }

    interface OnSeekCompleteListener {
        void onSeekComplete(IMediaPlayer mp);
    }

    interface OnVideoSizeChangedListener {
        void onVideoSizeChanged(IMediaPlayer mp, int width, int height,
                                int sar_num, int sar_den);
    }

    interface OnErrorListener {
        boolean onError(IMediaPlayer mp, int what, int extra);
    }

    interface OnInfoListener {
        boolean onInfo(IMediaPlayer mp, int what, int extra);
    }

    /*--------------------
     * Optional
     */
    void setAudioStreamType(int streamtype);

    @Deprecated
    void setKeepInBackground(boolean keepInBackground);

    int getVideoSarNum();

    int getVideoSarDen();

    @Deprecated
    void setWakeMode(Context context, int mode);

    void setLooping(boolean looping);

    boolean isLooping();

    /**
     * 获取当前是否是直播视频
     * @return
     * @throws IllegalStateException
     */
    int getIsLiveVideo() throws IllegalStateException;

    /**
     * 获取http请求信息
     * @return
     * @throws IllegalStateException
     */
    String getHttpInfoStr() throws IllegalStateException;

    /**
     * 获取so的版本，即编译的so时间戳
     */
    String getLibCompileTime();


    /**
     * 获取当前播放截屏
     * @return bitmap
     */
    Bitmap getCurrentFrame();

}
