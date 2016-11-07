package com.cmcm.v.player_sdk.view;

import android.app.Activity;
import android.content.Context;
import android.media.AudioManager;
import android.net.Uri;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.View;
import android.widget.MediaController;

import com.cmcm.v.player_sdk.player.IMediaPlayer;

import java.util.Map;

public abstract class BaseVideoView extends GLSurfaceView implements CMPlayerControl {
    protected static final String TAG = "BaseVideoView";

    // settable by the client
    protected Uri mUri;
    protected Map<String, String> mHeaders;

    // mCurrentState is a VideoView object's current state.
    // mTargetState is the state that a method caller intends to reach.
    // For instance, regardless the VideoView object's current state,
    // calling pause() intends to bring the object to a target state
    // of STATE_PAUSED.
    protected int mCurrentState = STATE_IDLE;
    protected int mTargetState = STATE_IDLE;

    // set VIDEO_LAYOUT_SCALE as default.
    protected int mVideoLayout = VIDEO_LAYOUT_FIT_PARENT;

    protected SurfaceHolder mSurfaceHolder = null;
    protected IMediaPlayer mMediaPlayer = null;
    protected int mVideoWidth;
    protected int mVideoHeight;
    protected int mVideoSarNum;
    protected int mVideoSarDen;
    protected int mSurfaceWidth;
    protected int mSurfaceHeight;

    protected int mCurrentBufferPercentage;
    protected boolean mHasFistrPic;

    protected MediaController mMediaController;
    private int mVideoRotationDegree = 0;

    protected IMediaPlayer.OnPreparedListener mOnPreparedListener;
    protected IMediaPlayer.OnCompletionListener mOnCompletionListener;
    protected IMediaPlayer.OnBufferingUpdateListener mOnBufferingUpdateListener;
    protected IMediaPlayer.OnSeekCompleteListener mOnSeekCompleteListener;
    protected IMediaPlayer.OnVideoSizeChangedListener mOnVideoSizeChangedListener;
    protected IMediaPlayer.OnErrorListener mOnErrorListener;
    protected IMediaPlayer.OnInfoListener mOnInfoListener;

    // recording the seek position while preparing
    protected int mSeekWhenPrepared;
    protected boolean mCanPause = true;
    protected boolean mCanSeekBack;

    protected boolean mCanSeekForward;
    protected Context mAppContext;

    protected IjkLibLoader mLibLoader = null;

    protected int mLastPositionOnCompletion; //记录播放完成时间点
    protected int mDuration = -1; //缓存duration值，同时也方便多次快速访问
    /**
     * 判断是否真正完成播放是根据已播放时长跟总时长的比较作为依据的,当 总时长 - 已播放时长 大于此阈值时,需要重启播放, 单位 : ms
     * 播放完成失败比较常见的，如m3u8视频，在播放过程中断网，就会发生
     */
    public static final int ONCOMPLETE_RESTART_THRES = 20000;

    public BaseVideoView(Context context, IjkLibLoader libLoader) {
        super(context);
        initVideoView(context, libLoader);
    }

    public BaseVideoView(Context context, AttributeSet attrs, IjkLibLoader libLoader){
        super(context, attrs);
        initVideoView(context,libLoader);
    }


    private void initVideoView(Context context, IjkLibLoader libLoader) {
        mLibLoader = libLoader;
        mAppContext = context.getApplicationContext();

        mVideoWidth = 0;
        mVideoHeight = 0;
        mVideoSarNum = 0;
        mVideoSarDen = 0;
        mHasFistrPic = false;

        mCurrentState = STATE_IDLE;
        mTargetState = STATE_IDLE;

        if (mAppContext instanceof Activity) {
            ((Activity) mAppContext).setVolumeControlStream(
                    AudioManager.STREAM_MUSIC);
        }
    }


    @Override
    public int getCurrentState() {
        return mCurrentState;
    }


    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {

        if (mVideoRotationDegree == 90 || mVideoRotationDegree == 270) {
            int tempSpec = widthMeasureSpec;
            widthMeasureSpec  = heightMeasureSpec;
            heightMeasureSpec = tempSpec;
        }

        int width = getDefaultSize(mVideoWidth, widthMeasureSpec);
        int height = getDefaultSize(mVideoHeight, heightMeasureSpec);
        if (mVideoWidth > 0 && mVideoHeight > 0) {
            int widthSpecSize = MeasureSpec.getSize(widthMeasureSpec);
            int heightSpecSize = MeasureSpec.getSize(heightMeasureSpec);

            float specAspectRatio = (float) widthSpecSize / (float)heightSpecSize;
            float displayAspectRatio;
            switch (mVideoLayout) {
                case VIDEO_LAYOUT_16_9_FIT_PARENT:
                    displayAspectRatio = 16.0f / 9.0f;
                    if (mVideoRotationDegree == 90 || mVideoRotationDegree == 270)
                        displayAspectRatio = 1.0f / displayAspectRatio;
                    break;
                case VIDEO_LAYOUT_4_3_FIT_PARENT:
                    displayAspectRatio = 4.0f / 3.0f;
                    if (mVideoRotationDegree == 90 || mVideoRotationDegree == 270)
                        displayAspectRatio = 1.0f / displayAspectRatio;
                    break;
                case VIDEO_LAYOUT_FIT_PARENT:
                case VIDEO_LAYOUT_FILL_PARENT:
                case VIDEO_LAYOUT_WRAP_CONTENT:
                default:
                    displayAspectRatio = (float) mVideoWidth / (float) mVideoHeight;
                    if (mVideoSarNum > 0 && mVideoSarDen > 0)
                        displayAspectRatio = displayAspectRatio * mVideoSarNum / mVideoSarDen;
                    break;
            }
            boolean shouldBeWider = displayAspectRatio > specAspectRatio;

            switch (mVideoLayout) {
                case VIDEO_LAYOUT_FIT_PARENT:
                case VIDEO_LAYOUT_16_9_FIT_PARENT:
                case VIDEO_LAYOUT_4_3_FIT_PARENT:
                    if (shouldBeWider) {
                        // too wide, fix width
                        width = widthSpecSize;
                        height = (int) (width / displayAspectRatio);
                    } else {
                        // too high, fix height
                        height = heightSpecSize;
                        width = (int) (height * displayAspectRatio);
                    }
                    break;
                case VIDEO_LAYOUT_FILL_PARENT:
                    if (shouldBeWider) {
                        // not high enough, fix height
                        height = heightSpecSize;
                        width = (int) (height * displayAspectRatio);
                    } else {
                        // not wide enough, fix width
                        width = widthSpecSize;
                        height = (int) (width / displayAspectRatio);
                    }
                    break;
                case VIDEO_LAYOUT_WRAP_CONTENT:
                default:
                    if (shouldBeWider) {
                        // too wide, fix width
                        width = Math.min(mVideoWidth, widthSpecSize);
                        height = (int) (width / displayAspectRatio);
                    } else {
                        // too high, fix height
                        height = Math.min(mVideoHeight, heightSpecSize);
                        width = (int) (height * displayAspectRatio);
                    }
                    break;
            }
        }

        setMeasuredDimension(width, height);
    }

    @Override
    public View getView(){
        return this;
    }

    @Override
    public int getVideoLayout() {
        return mVideoLayout;
    }

    @Override
    public void setVideoLayout(int layout) {
        mVideoLayout = layout;
        requestLayout();
    }

    public boolean isValid() {
        return (mSurfaceHolder != null &&
                mSurfaceHolder.getSurface().isValid());
    }

    @Override
    public void setVideoPath(String path) {
        setVideoURI(Uri.parse(path));
    }

    @Override
    public void setVideoURI(Uri uri) {
        setVideoURI(uri, null);
    }

    @Override
    public void setVideoURI(Uri uri, Map<String, String> headers) {
        mUri = uri;
        mHeaders = headers;
        mSeekWhenPrepared = 0;
        openVideo();
        requestLayout();
        invalidate();
    }

    @Override
    public void stopPlayback() {
        Log.i("chenyg", "stopPlayback()");
        if (mMediaPlayer != null) {
            mDuration = -1;
            mLastPositionOnCompletion = 0;
            mMediaPlayer.stop();
            mMediaPlayer.release();
            mMediaPlayer = null;
            mCurrentState = STATE_IDLE;
            mTargetState = STATE_IDLE;
            AudioManager am = (AudioManager) mAppContext.getSystemService(
                    Context.AUDIO_SERVICE);
            am.abandonAudioFocus(null);
        }
    }

    protected abstract void openVideo();

    protected abstract boolean hasErrorWhenComplete();

    @Override
    public void setMediaController(MediaController controller) {
        if (mMediaController != null) {
            mMediaController.hide();
        }
        mMediaController = controller;
        attachMediaController();
    }

    protected void attachMediaController() {
        if (mMediaPlayer != null && mMediaController != null) {
            mMediaController.setMediaPlayer(this);
            View anchorView = this.getParent() instanceof View ?
                    (View) this.getParent() : this;
            mMediaController.setAnchorView(anchorView);
            mMediaController.setEnabled(isInPlaybackState());
        }
    }



    protected IMediaPlayer.OnCompletionListener mCompletionListener =
            new IMediaPlayer.OnCompletionListener() {
                public void onCompletion(IMediaPlayer mp) {
                    mDuration = getDuration();
                    mLastPositionOnCompletion = getCurrentPosition();
                    if (mMediaController != null) {
                        mMediaController.hide();
                    }
                    if(hasErrorWhenComplete()) { //断网时，或是播放中断网然后seekto就会走if分支
                        mCurrentState = STATE_ERROR;
                        mTargetState = STATE_PLAYBACK_COMPLETED;
                        if (mOnErrorListener != null) {
                            mOnErrorListener.onError(mMediaPlayer, 2, 0); //这种播放完成失败的定义为what=2，extra=0
                        }
                    } else {
                        mCurrentState = STATE_PLAYBACK_COMPLETED;
                        mTargetState = STATE_PLAYBACK_COMPLETED;
                        if (mOnCompletionListener != null) {
                            mOnCompletionListener.onCompletion(mMediaPlayer);
                        }
                    }
                }
            };

    protected IMediaPlayer.OnPreparedListener mPreparedListener = new IMediaPlayer.OnPreparedListener() {
        public void onPrepared(IMediaPlayer mp) {
            mCurrentState = STATE_PREPARED;

            // Get the capabilities of the player for this stream
            // REMOVED: Metadata

            if (mOnPreparedListener != null) {
                mOnPreparedListener.onPrepared(mMediaPlayer);
            }
            if (mMediaController != null) {
                mMediaController.setEnabled(true);
            }
            mVideoWidth = mp.getVideoWidth();
            mVideoHeight = mp.getVideoHeight();

            int seekToPosition = mSeekWhenPrepared;
            // mSeekWhenPrepared may be changed after seekTo() call

            if (seekToPosition != 0) {
                seekTo(seekToPosition);
            }else{
                if(!mHasFistrPic) {
                    //if (BaseVideoView.this instanceof VideoViewAndroid) {
                        if (mOnInfoListener != null) {
                            mOnInfoListener.onInfo(mMediaPlayer, IMediaPlayer.MEDIA_INFO_FIRST_SHOW_PIC, 0);
                        }
                    //}
                    mHasFistrPic = true;
                }
            }

            if (mVideoWidth != 0 && mVideoHeight != 0) {
                setVideoLayout(mVideoLayout);
                getHolder().setFixedSize(mVideoWidth, mVideoHeight);
                if (mSurfaceWidth == mVideoWidth &&
                        mSurfaceHeight == mVideoHeight) {
                    // We didn't actually change the size (it was
                    // already at the size we need), so we won't get a
                    // "surface changed" callback, so start the video
                    // here instead of in the callback.
                    if (mTargetState == STATE_PLAYING) {
                        start();
                        if (mMediaController != null) {
                            mMediaController.show();
                        }
                    } else if (!isPlaying() && (seekToPosition != 0 || getCurrentPosition() > 0)) {
                        if (mMediaController != null) {
                            mMediaController.show(0);
                        }
                    }
                }
            } else {
                // We don't know the video size yet, but should start
                // anyway. The video size might be reported to us later.
                if (mTargetState == STATE_PLAYING) {
                    start();
                }
            }
        }
    };


    protected IMediaPlayer.OnSeekCompleteListener mSeekCompleteListener =
            new IMediaPlayer.OnSeekCompleteListener() {
                public void onSeekComplete(IMediaPlayer mp) {

                    Log.d(TAG, "onSeekComplete 1");
                    if(!mHasFistrPic) {
                        Log.d(TAG, "onSeekComplete 2");
                        //TODO:
                        //if (BaseVideoView.this instanceof VideoViewAndroid) {
                            Log.d(TAG, "onSeekComplete 3");
                            if (mOnInfoListener != null) {
                                Log.d(TAG, "onSeekComplete 4");
                                mOnInfoListener.onInfo(mMediaPlayer, IMediaPlayer.MEDIA_INFO_FIRST_SHOW_PIC, 0);
                            }
                        //}
                        mHasFistrPic = true;
                    }

                    Log.d(TAG, "onSeekComplete");
                    if (mOnSeekCompleteListener != null) {
                        mOnSeekCompleteListener.onSeekComplete(mp);
                    }
                }
            };

    protected IMediaPlayer.OnVideoSizeChangedListener mSizeChangedListener =
            new IMediaPlayer.OnVideoSizeChangedListener() {
                public void onVideoSizeChanged(IMediaPlayer mp,
                                               int width,
                                               int height,
                                               int sarNum,
                                               int sarDen) {
                    if (mOnVideoSizeChangedListener != null) {
                        mOnVideoSizeChangedListener.onVideoSizeChanged(mp,
                                width, height, sarNum, sarDen);
                    }
                    mVideoWidth = mp.getVideoWidth();
                    mVideoHeight = mp.getVideoHeight();
                    mVideoSarNum = sarNum;
                    mVideoSarDen = sarDen;
                    if (mVideoWidth != 0 && mVideoHeight != 0) {
                        setVideoLayout(mVideoLayout);

                        getHolder().setFixedSize(mVideoWidth, mVideoHeight);
                        requestLayout();
                    }
                }
            };

    protected IMediaPlayer.OnInfoListener mInfoListener =
            new IMediaPlayer.OnInfoListener() {
                public boolean onInfo(IMediaPlayer mp, int arg1, int arg2) {
                    if (mOnInfoListener != null) {
                        mOnInfoListener.onInfo(mp, arg1, arg2);
                    }
                    return true;
                }
            };

    protected IMediaPlayer.OnErrorListener mErrorListener =
            new IMediaPlayer.OnErrorListener() {
                public boolean onError(IMediaPlayer mp,
                                       int framework_err,
                                       int impl_err) {
                    Log.d(TAG, "Error: " + framework_err + "," + impl_err);
                    mCurrentState = STATE_ERROR;
                    mTargetState = STATE_ERROR;
                    if (mMediaController != null) {
                        mMediaController.hide();
                    }

                    /* If an error handler has been supplied, use it and
                     * finish.
                     */
                    if (mOnErrorListener != null) {
                        if (mOnErrorListener.onError(mMediaPlayer,
                                framework_err, impl_err)) {
                            return true;
                        }
                    }


                    return true;
                }
            };

    @Override
    public void setOnPreparedListener(IMediaPlayer.OnPreparedListener l) {
        mOnPreparedListener = l;
    }

    @Override
    public void setOnCompletionListener(IMediaPlayer.OnCompletionListener l) {
        mOnCompletionListener = l;
    }

    @Override
    public void setOnBufferingUpdateListener(IMediaPlayer.OnBufferingUpdateListener l) {
        mOnBufferingUpdateListener = l;
    }

    @Override
    public void setOnSeekCompleteListener(IMediaPlayer.OnSeekCompleteListener l) {
        mOnSeekCompleteListener = l;
    }

    @Override
    public void setOnVideoSizeChangedListener(IMediaPlayer.OnVideoSizeChangedListener l) {
        mOnVideoSizeChangedListener = l;
    }

    @Override
    public void setOnErrorListener(IMediaPlayer.OnErrorListener l) {
        mOnErrorListener = l;
    }

    @Override
    public void setOnInfoListener(IMediaPlayer.OnInfoListener l) {
        mOnInfoListener = l;
    }


    public void releaseWithoutStop() {
        if (mMediaPlayer != null)
            mMediaPlayer.setDisplay(null);
    }

    @Override
    public void release(boolean clearTargetState) {
        if (mMediaPlayer != null) {
            mDuration = -1;
            mLastPositionOnCompletion = 0;
            mMediaPlayer.reset();
            mMediaPlayer.release();
            mMediaPlayer = null;
            mCurrentState = STATE_IDLE;
            if (clearTargetState) {
                mTargetState = STATE_IDLE;
            }
            AudioManager am = (AudioManager) mAppContext.getSystemService(Context.AUDIO_SERVICE);
            am.abandonAudioFocus(null);
        }
    }

    private void toggleMediaControlsVisibility() {
        if (mMediaController.isShowing()) {
            mMediaController.hide();
        } else {
            mMediaController.show();
        }
    }

    /**
     * Start To Play
     * Change State into Buffering State.
     */
    @Override
    public void start() throws IllegalStateException {
        try {
            if (isInPlaybackState()) {
                mMediaPlayer.start();
                mCurrentState = STATE_PLAYING;
            }
            mTargetState = STATE_PLAYING;
        } catch (Exception e) {

        }
    }

    @Override
    public void pause() {
        try {
            if (isInPlaybackState()) {
                if (mMediaPlayer.isPlaying()) {
                    mMediaPlayer.pause();
                    mCurrentState = STATE_PAUSED;
                }
            }
            mTargetState = STATE_PAUSED;
        } catch (Exception e) {

        }
    }

    @Override
    public int getDuration() {
        if (isInPlaybackState()) {
            if(mDuration > 0) {
                return mDuration;
            }
            mDuration = (int) mMediaPlayer.getDuration();
            return mDuration;
        }
        mDuration = -1;
        return -1;
    }

    @Override
    public int getCurrentPosition() {
        if (isInPlaybackState()) {
            return (int) mMediaPlayer.getCurrentPosition();
        }
        return 0;
    }

    @Override
    public void seekTo(int msec) {
        if (isInPlaybackState()) {
            mMediaPlayer.seekTo(msec);
            mSeekWhenPrepared = 0;
        } else {
            mSeekWhenPrepared = msec;
        }
    }

    @Override
    public boolean isPlaying() {
        return isInPlaybackState() && mMediaPlayer.isPlaying();
    }

    @Override
    public int getBufferPercentage() {
        if (mMediaPlayer != null) {
            return mCurrentBufferPercentage;
        }
        return 0;
    }

    protected boolean isInPlaybackState() {
        return (mMediaPlayer != null &&
                mCurrentState != STATE_ERROR &&
                mCurrentState != STATE_IDLE &&
                mCurrentState != STATE_PREPARING);
    }

    @Override
    public boolean onTouchEvent(MotionEvent ev) {
        if (isInPlaybackState() && mMediaController != null) {
            toggleMediaControlsVisibility();
        }

        return false;
    }

    @Override
    public boolean canPause() {
        return mCanPause;
    }

    @Override
    public boolean canSeekBackward() {
        return mCanSeekBack;
    }

    @Override
    public boolean canSeekForward() {
        return mCanSeekForward;
    }

    @Override
    public int getAudioSessionId() {
        return 0;
    }
}
