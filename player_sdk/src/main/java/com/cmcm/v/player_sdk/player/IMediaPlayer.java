package com.cmcm.v.player_sdk.player;

import android.annotation.TargetApi;
import android.content.Context;
import android.graphics.Bitmap;
import android.os.Build;
import android.view.Surface;
import android.view.SurfaceHolder;

import java.io.IOException;

public interface IMediaPlayer {
    /*
     * Do not change these values without updating their counterparts
     * in native ijk-player libraries.
     */
    int MEDIA_INFO_UNKNOWN = 1;
    int MEDIA_INFO_STARTED_AS_NEXT = 2;
    int MEDIA_INFO_VIDEO_RENDERING_START = 3;
    int MEDIA_INFO_VIDEO_TRACK_LAGGING = 700;
    int MEDIA_INFO_BUFFERING_START = 701;
    int MEDIA_INFO_BUFFERING_END = 702;
    int MEDIA_INFO_NETWORK_BANDWIDTH = 703;
    int MEDIA_INFO_BAD_INTERLEAVING = 800;
    int MEDIA_INFO_NOT_SEEKABLE = 801;
    int MEDIA_INFO_METADATA_UPDATE = 802;
    int MEDIA_INFO_TIMED_TEXT_ERROR = 900;
    // some extra info begin
    int MEDIA_INFO_OPEN_FILE_INFO = 911;
    int MEDIA_INFO_FIRST_SHOW_PIC = 1001;
    int MEDIA_INFO_VIDEO_ROTATION_CHANGED = 1002;

    int MEDIA_INFO_OPEN_FILE_PROGRESS = 10703;
    int MEDIA_INFO_DOWNLOAD_RATE_CHANGED = 10901;
    // some extra info end

    int MEDIA_ERROR_UNKNOWN = 1;
    int MEDIA_ERROR_SERVER_DIED = 100;
    int MEDIA_ERROR_NOT_VALID_FOR_PROGRESSIVE_PLAYBACK = 200;
    int MEDIA_ERROR_IO = -1004;
    int MEDIA_ERROR_MALFORMED = -1007;
    int MEDIA_ERROR_UNSUPPORTED = -1010;
    int MEDIA_ERROR_TIMED_OUT = -110;

    //for browser begin
    //播放器数值定义
    /** 播放器处于未知状态*/
    int MEDIA_UNKNOW_STATUS         = -1;
    /** 非直播*/
    int MEDIA_VIDEO_PLAYER_NORMAL   = 0;
    /** 视频直播*/
    int MEDIA_VIDEO_PLAYER_LIVE     = 1;
    //for browser end

    void setDisplay(SurfaceHolder sh);

    void setDataSource(String path) throws
            IOException, IllegalArgumentException, SecurityException, IllegalStateException;

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

    //MediaInfo getMediaInfo();

    void setLogEnabled(boolean enable);

    boolean isPlayable();

    void setOnPreparedListener(OnPreparedListener listener);

    void setOnCompletionListener(OnCompletionListener listener);

    void setOnBufferingUpdateListener(OnBufferingUpdateListener listener);

    void setOnSeekCompleteListener(OnSeekCompleteListener listener);

    void setOnVideoSizeChangedListener(OnVideoSizeChangedListener listener);

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
        void onVideoSizeChanged(IMediaPlayer mp, int width, int height, int sar_num, int sar_den);
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

    int getVideoSarNum();

    int getVideoSarDen();

    @Deprecated
    void setWakeMode(Context context, int mode);

    @TargetApi(Build.VERSION_CODES.ICE_CREAM_SANDWICH)
    void setSurface(Surface surface);

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