package com.cmcm.v.player_sdk.view;

import android.content.Context;
import android.graphics.Bitmap;
import android.media.AudioManager;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;

import com.cmcm.v.player_sdk.player.AndroidMediaPlayer;
import com.cmcm.v.player_sdk.player.IMediaPlayer;

import java.io.IOException;

public class VideoViewAndroid extends BaseVideoView {
    public VideoViewAndroid(Context context, IjkLibLoader libLoader) {
        super(context, libLoader);
    }

    public VideoViewAndroid(Context context, AttributeSet attrs, IjkLibLoader libLoader) {
        super(context, attrs, libLoader);
    }

    @Override
    protected void openVideo() {
        if (mUri == null || mSurfaceHolder == null) {
            // not ready for playback just yet, will try again later
            return;
        }
        // we shouldn't clear the target state, because somebody might have
        // called start() previously
        release(false);

        AudioManager am = (AudioManager) mAppContext.getSystemService(Context.AUDIO_SERVICE);
        am.requestAudioFocus(null, AudioManager.STREAM_MUSIC, AudioManager.AUDIOFOCUS_GAIN);

        try {
            mMediaPlayer = new AndroidMediaPlayer();
            mMediaPlayer.setOnPreparedListener(mPreparedListener);
            mMediaPlayer.setOnCompletionListener(mCompletionListener);
            mMediaPlayer.setOnBufferingUpdateListener(mBufferingUpdateListener);
            mMediaPlayer.setOnSeekCompleteListener(mSeekCompleteListener);
            mMediaPlayer.setOnVideoSizeChangedListener(mSizeChangedListener);
            mMediaPlayer.setOnErrorListener(mErrorListener);
            mMediaPlayer.setOnInfoListener(mInfoListener);
            mCurrentBufferPercentage = 0;
            mDuration = -1;
            mLastPositionOnCompletion = 0;
            mMediaPlayer.setDataSource(mUri.toString());
            mMediaPlayer.setDisplay(mSurfaceHolder);
            mMediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
            mMediaPlayer.setScreenOnWhilePlaying(true);
            mMediaPlayer.prepareAsync();

            // we don't set the target state here either, but preserve the
            // target state that was there before.
            mCurrentState = STATE_PREPARING;
            attachMediaController();
        } catch (IOException ex) {
            Log.w(TAG, "Unable to open content: " + mUri, ex);
            mCurrentState = STATE_ERROR;
            mTargetState = STATE_ERROR;
            mErrorListener.onError(mMediaPlayer, MediaConst.MEDIA_ERROR_OPEN_VIDEO_EXCEPTION, MediaConst.ERROR_EXTRA_IO);
            return;
        } catch (IllegalArgumentException ex) {
            Log.w(TAG, "Unable to open content: " + mUri, ex);
            mCurrentState = STATE_ERROR;
            mTargetState = STATE_ERROR;
            mErrorListener.onError(mMediaPlayer, MediaConst.MEDIA_ERROR_OPEN_VIDEO_EXCEPTION, MediaConst.ERROR_EXTRA_ILLEGAL_ARG);
            return;
        } catch (IllegalStateException ex) {
            Log.w(TAG, "Unable to open content: " + mUri, ex);
            mCurrentState = STATE_ERROR;
            mTargetState = STATE_ERROR;
            mErrorListener.onError(mMediaPlayer, MediaConst.MEDIA_ERROR_OPEN_VIDEO_EXCEPTION, MediaConst.ERROR_EXTRA_ILLEGAL_STAT);
            return;
        } catch (SecurityException ex) {
            Log.w(TAG, "Unable to open content: " + mUri, ex);
            mCurrentState = STATE_ERROR;
            mTargetState = STATE_ERROR;
            mErrorListener.onError(mMediaPlayer, MediaConst.MEDIA_ERROR_OPEN_VIDEO_EXCEPTION, MediaConst.ERROR_EXTRA_SECURITY);
            return;
        }  catch (UnsatisfiedLinkError ex) {
            Log.w(TAG, "Unable to open content: " + mUri, ex);
            mCurrentState = STATE_ERROR;
            mTargetState = STATE_ERROR;
            mErrorListener.onError(mMediaPlayer, MediaConst.MEDIA_ERROR_OPEN_VIDEO_EXCEPTION, MediaConst.ERROR_EXTRA_UNSATISFIEDLINK);
            return;
        }  catch (Exception ex) {
            Log.w(TAG, "Unable to open content: " + mUri, ex);
            mCurrentState = STATE_ERROR;
            mTargetState = STATE_ERROR;
            mErrorListener.onError(mMediaPlayer, MediaConst.MEDIA_ERROR_OPEN_VIDEO_EXCEPTION, MediaConst.ERROR_EXTRA_EXCETPION);
            return;
        } finally {
        }
    }

    @Override
    protected boolean hasErrorWhenComplete() {
        return false;
    }

    @Override
    public int getIsLiveVideo() throws IllegalStateException {
        return IMediaPlayer.MEDIA_UNKNOW_STATUS;
    }

    @Override
    public String getHttpInfoStr() throws IllegalStateException {
        return "";
    }

    @Override
    public String getLibCompileTime() {
        return "";
    }

    @Override
    public Bitmap getCurrentFrame() {
        return null;
    }

    @Override
    public void setVideoRotation(int degree) {

    }

    public void surfaceChanged(SurfaceHolder holder,
                               int format,
                               int w,
                               int h) {
        Log.i("chenyg", "SurfaceChangeed()");
        mSurfaceHolder = holder;
        if (mMediaPlayer != null) {
            mMediaPlayer.setDisplay(mSurfaceHolder);
        }

        mSurfaceWidth = w;
        mSurfaceHeight = h;
        boolean isValidState = (mTargetState == STATE_PLAYING);
        boolean hasValidSize = (mVideoWidth == w && mVideoHeight == h);
        if (mMediaPlayer != null && isValidState && hasValidSize) {
            if (mSeekWhenPrepared != 0)
                seekTo(mSeekWhenPrepared);
            start();
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.i("chenyg", "surfaceCreated()");
        mSurfaceHolder = holder;
        if (mMediaPlayer != null) {
            mMediaPlayer.setDisplay(mSurfaceHolder);
        } else {
            openVideo();
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.i("chenyg", "surfaceDestroyed()");
        mSurfaceHolder = null;
        releaseWithoutStop();
    }

    protected IMediaPlayer.OnBufferingUpdateListener mBufferingUpdateListener =
        new IMediaPlayer.OnBufferingUpdateListener() {
            public void onBufferingUpdate(IMediaPlayer mp, int percent) {
                mCurrentBufferPercentage = percent;
                if (mOnBufferingUpdateListener != null) {
                    mOnBufferingUpdateListener.onBufferingUpdate(mp,
                            percent);
                }
            }
        };
}
