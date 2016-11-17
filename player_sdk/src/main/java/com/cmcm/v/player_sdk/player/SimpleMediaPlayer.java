package com.cmcm.v.player_sdk.player;

/**
 * Created by donghao on 16/10/31.
 */

import android.annotation.TargetApi;
import android.content.Context;
import android.view.Surface;


public abstract class SimpleMediaPlayer implements IMediaPlayer {
    private boolean mIsLogEnabled;
    protected OnPreparedListener mOnPreparedListener;
    protected OnCompletionListener mOnCompletionListener;
    protected OnBufferingUpdateListener mOnBufferingUpdateListener;
    protected OnSeekCompleteListener mOnSeekCompleteListener;
    protected OnVideoSizeChangedListener mOnVideoSizeChangedListener;
    protected OnErrorListener mOnErrorListener;
    protected OnInfoListener mOnInfoListener;

    protected GLRenderControlerListener mGLRenderControlerListener;

    public SimpleMediaPlayer() {
    }

    public boolean isLogEnabled() {
        return this.mIsLogEnabled;
    }

    public void setLogEnabled(boolean enable) {
        this.mIsLogEnabled = enable;
    }

    public boolean isPlayable() {
        return true;
    }

    public void setAudioStreamType(int streamtype) {
    }

    public int getVideoSarNum() {
        return 1;
    }

    public int getVideoSarDen() {
        return 1;
    }

    /** @deprecated */
    @Deprecated
    public void setWakeMode(Context context, int mode) {
    }

    @TargetApi(14)
    public void setSurface(Surface surface) {
    }

    public final void setOnPreparedListener(OnPreparedListener listener) {
        this.mOnPreparedListener = listener;
    }

    public final void setOnCompletionListener(OnCompletionListener listener) {
        this.mOnCompletionListener = listener;
    }

    public final void setOnBufferingUpdateListener(OnBufferingUpdateListener listener) {
        this.mOnBufferingUpdateListener = listener;
    }

    public final void setOnSeekCompleteListener(OnSeekCompleteListener listener) {
        this.mOnSeekCompleteListener = listener;
    }

    public final void setOnVideoSizeChangedListener(OnVideoSizeChangedListener listener) {
        this.mOnVideoSizeChangedListener = listener;
    }

    public final void setOnErrorListener(OnErrorListener listener) {
        this.mOnErrorListener = listener;
    }

    public final void setOnInfoListener(OnInfoListener listener) {
        this.mOnInfoListener = listener;
    }

    public void setRenderControler(GLRenderControlerListener listener) {
        this.mGLRenderControlerListener = listener;
    }

    public void resetListeners() {
        this.mOnPreparedListener = null;
        this.mOnBufferingUpdateListener = null;
        this.mOnCompletionListener = null;
        this.mOnSeekCompleteListener = null;
        this.mOnVideoSizeChangedListener = null;
        this.mOnErrorListener = null;
        this.mOnInfoListener = null;
    }

    protected final void notifyOnPrepared() {
        if(this.mOnPreparedListener != null) {
            this.mOnPreparedListener.onPrepared(this);
        }

    }

    protected final void notifyOnCompletion() {
        if(this.mOnCompletionListener != null) {
            this.mOnCompletionListener.onCompletion(this);
        }

    }

    protected final void notifyOnBufferingUpdate(int percent) {
        if(this.mOnBufferingUpdateListener != null) {
            this.mOnBufferingUpdateListener.onBufferingUpdate(this, percent);
        }

    }

    protected final void notifyOnSeekComplete() {
        if(this.mOnSeekCompleteListener != null) {
            this.mOnSeekCompleteListener.onSeekComplete(this);
        }

    }

    protected final void notifyOnVideoSizeChanged(int width, int height, int sarNum, int sarDen) {
        if(this.mOnVideoSizeChangedListener != null) {
            this.mOnVideoSizeChangedListener.onVideoSizeChanged(this, width, height, sarNum, sarDen);
        }

    }

    protected final boolean notifyOnError(int what, int extra) {
        return this.mOnErrorListener != null?this.mOnErrorListener.onError(this, what, extra):false;
    }

    protected final boolean notifyOnInfo(int what, int extra) {
        return this.mOnInfoListener != null?this.mOnInfoListener.onInfo(this, what, extra):false;
    }
}
