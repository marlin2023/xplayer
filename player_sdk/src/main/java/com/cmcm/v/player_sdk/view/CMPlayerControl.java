package com.cmcm.v.player_sdk.view;

import android.graphics.Bitmap;
import android.net.Uri;
import android.view.View;
import android.widget.MediaController;

import com.cmcm.v.player_sdk.player.IMediaPlayer;

import java.util.Map;

/**
 * Created by chris on 11/1/16.
 */

public interface CMPlayerControl extends MediaController.MediaPlayerControl {
    // all possible internal states
    int STATE_ERROR = -1;
    int STATE_IDLE = 0;
    int STATE_PREPARING = 1;
    int STATE_PREPARED = 2;
    int STATE_PLAYING = 3;
    int STATE_PAUSED = 4;
    int STATE_PLAYBACK_COMPLETED = 5;
    int STATE_SEEKING = 6;

    // video view layout types
    int VIDEO_LAYOUT_FIT_PARENT = 0;
    int VIDEO_LAYOUT_FILL_PARENT = 1;
    int VIDEO_LAYOUT_WRAP_CONTENT = 2;
    int VIDEO_LAYOUT_16_9_FIT_PARENT = 3;
    int VIDEO_LAYOUT_4_3_FIT_PARENT = 4;
    int VIDEO_LAYOUT_MAX = 5;

    // public methods
    void setVideoPath(String path);
    void setVideoURI(Uri uri);
    void setVideoURI(Uri uri, Map<String, String> headers);
    void stopPlayback();
    void setMediaController(MediaController controller);
    void setOnPreparedListener(IMediaPlayer.OnPreparedListener l);
    void setOnCompletionListener(IMediaPlayer.OnCompletionListener l);
    void setOnBufferingUpdateListener(IMediaPlayer.OnBufferingUpdateListener l);
    void setOnSeekCompleteListener(IMediaPlayer.OnSeekCompleteListener l);
    void setOnVideoSizeChangedListener(IMediaPlayer.OnVideoSizeChangedListener l);
    void setOnErrorListener(IMediaPlayer.OnErrorListener l);
    void setOnInfoListener(IMediaPlayer.OnInfoListener l);
    void setRenderControler(IMediaPlayer.GLRenderControlerListener l);

    void release(boolean clearTargetState);
    View getView();
    int  getCurrentState();
    int  getVideoLayout();
    void setVideoLayout(int layout);

    void setVideoRotation(int degree);
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