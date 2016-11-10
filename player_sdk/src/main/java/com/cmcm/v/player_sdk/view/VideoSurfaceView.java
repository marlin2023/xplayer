package com.cmcm.v.player_sdk.view;

import android.content.Context;
import android.graphics.Bitmap;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;

import com.cmcm.v.player_sdk.player.IMediaPlayer;
import com.cmcm.v.player_sdk.player.XPlayer;

import java.io.IOException;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by chris on 10/20/16.
 * 1) TextureView+SurfaceTexture+OpenGL ES来播放视频
 *           http://www.jianshu.com/p/d3d3186eefcb
 * 2) Actvity will new this object ,and add it.
 *
 */

public class VideoSurfaceView extends BaseVideoView implements GLSurfaceView.Renderer{

    public static String TAG = "VideoSurfaceView";

    boolean isEGLContextInitilized;

    public int lastW = 0;

    public int lastH = 0;

    public VideoSurfaceView(Context context, IjkLibLoader libLoader) {
        super(context, libLoader);
        initView();
    }

    public VideoSurfaceView(Context context, AttributeSet attrs, IjkLibLoader libLoader) {
        super(context, attrs, libLoader);
        initView();
    }

    protected void initView(){
        Log.i(TAG, "VideoSurfaceView Construct...");

        isEGLContextInitilized = false;

        mMediaPlayer = new XPlayer();

        setEGLContextClientVersion(2);                  // set opengl es version
        // setEGLConfigChooser(8, 8, 8, 8, 8, 0);
        // getHolder().setFormat(PixelFormat.RGBA_8888);
        // bind the render to the surfaceivew
        setRenderer(this);
        setRenderMode(RENDERMODE_WHEN_DIRTY);           // set render mode
        Log.i(TAG, "VideoSurfaceView Construct ok...");
    }


    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        Log.i(TAG ,"========>onSurfaceCreated");
        lastW = 0;
        lastH = 0;
        // create gl program
        if(!isEGLContextInitilized){
            this.mMediaPlayer.initEGLCtx();
            isEGLContextInitilized = true;
        }
        //
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        Log.e(TAG, "=====>on GLSurfaceView onSurfaceChanged.function..-->width=" + width + ",height=" + height);

        if(mMediaPlayer!= null){
            if ((lastW != width) || (lastH != width))
            {
                Log.e(TAG, "=====>on GLSurfaceView onSurfaceChanged.set glViewport....set -->width=" + width + ",height=" + height);
                gl.glViewport(0, 0, width , height );
                //gl.glViewport(0, 0, 640, 360);
                lastW = width;
                lastH = height;
            }
        }

    }

    @Override
    public void onDrawFrame(GL10 gl) {
        if(mMediaPlayer!= null){
            mMediaPlayer.renderFrame();
        }

    }

    @Override
    public void onResume() {
        super.onResume();

    }

    @Override
    public void onPause() {
        super.onPause();
        // start
        queueEvent(new Runnable(){
            @Override
            public void run() {
                Log.i(TAG, ".........");
            }
        });
        // end
    }

    public void init() {
        // start
        Log.i(TAG, "xPlayer.init() run in main thread. .........");
        if(mMediaPlayer != null){
            mMediaPlayer.initPlayer();
            Log.i(TAG, "after xPlayer.init()  .........");
        }
        // end
    }

    public void setDataSource(String path) throws IOException{
        mMediaPlayer.setDataSource(path);
    }

    public void prepareAsync() throws IllegalStateException {
        mMediaPlayer.prepareAsync();
    }



    public void play(){
        mMediaPlayer.playInterface();
        // set GLSurfaceView.Render MODE
        setRenderMode(RENDERMODE_CONTINUOUSLY); // set render mode RENDERMODE_CONTINUOUSLY
        mCurrentState = STATE_PLAYING;
    }


    @Override
    protected void openVideo() {
        if (mUri == null) {
            // not ready for playback just yet, will try again later
            return;
        }
        // we shouldn't clear the target state, because somebody might have
        // called start() previously
        //release(false);

//        AudioManager am = (AudioManager) mAppContext.getSystemService(Context.AUDIO_SERVICE);
//        am.requestAudioFocus(null, AudioManager.STREAM_MUSIC, AudioManager.AUDIOFOCUS_GAIN);

        try {

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
            //mMediaPlayer.setDisplay(mSurfaceHolder);
//            mMediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
            mMediaPlayer.setScreenOnWhilePlaying(true);
            mMediaPlayer.prepareAsync();

            // we don't set the target state here either, but preserve the
            // target state that was there before.
            mCurrentState = STATE_PREPARING;
            attachMediaController();
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


    private int isCalledPlay = 0;
    protected IMediaPlayer.OnBufferingUpdateListener mBufferingUpdateListener =
            new IMediaPlayer.OnBufferingUpdateListener() {
                public void onBufferingUpdate(IMediaPlayer mp, int percent) {
                    mCurrentBufferPercentage = percent;
//                    if (mOnBufferingUpdateListener != null) {
//                        mOnBufferingUpdateListener.onBufferingUpdate(mp,
//                                percent);
//                    }
                    if((percent == 100) && (isCalledPlay == 0)){
                        isCalledPlay = 1;
                        Log.i("chris" ,"=====>call play function ");
                        play();
                    }
                }
            };


    @Override
    protected boolean hasErrorWhenComplete() {
        if (mMediaPlayer == null) {
            return true;
        }
        boolean result = false;

        boolean islive = false;
        if (mMediaPlayer.getIsLiveVideo() == IMediaPlayer.MEDIA_VIDEO_PLAYER_LIVE) {
            islive = true;
        }

        // 满足一下情况认为是错误的onComplete:
        // 1.直播
        // 2.普通视频没有播完
        if (islive) {
            result = true;
        } else if (mDuration >= 0 && mLastPositionOnCompletion >= 0) {
            if ((mDuration - mLastPositionOnCompletion) > ONCOMPLETE_RESTART_THRES) {
                // 未播放完成
                result = true;
            }
        }
        Log.i(TAG, "hasErrorWhenComplete()=" + result + ", islive=" + islive + ", mLastPositionOnCompletion=" + mLastPositionOnCompletion + ", mDuration=" + mDuration);
        return result;
    }

    @Override
    public void setVideoRotation(int degree) {

    }

    /**
     * 直播的判断  返回值0-非直播， 1-直播
     * @return
     * @throws IllegalStateException
     */
    @Override
    public int getIsLiveVideo() throws IllegalStateException {
        if(mMediaPlayer != null) {
            return mMediaPlayer.getIsLiveVideo();
        }
        return IMediaPlayer.MEDIA_UNKNOW_STATUS;
    }

    /**
     * 获取视频源的http请求信息（http_code, mime_type, content, url）
     * @return
     * @throws IllegalStateException
     */
    @Override
    public String getHttpInfoStr() throws IllegalStateException {
        if(mMediaPlayer != null) {
            return mMediaPlayer.getHttpInfoStr();
        }
        return "";
    }

    @Override
    public String getLibCompileTime() {
        if(mMediaPlayer != null) {
            return mMediaPlayer.getLibCompileTime();
        }
        return "";
    }

    @Override
    public Bitmap getCurrentFrame() {
        if(mMediaPlayer != null) {
            return mMediaPlayer.getCurrentFrame();
        }
        return null;
    }

    @Override
    public void start() throws IllegalStateException {
        try {
            if(mCurrentState == STATE_SEEKING){return;} // add for soft decoder.

            if (isInPlaybackState()) {
                if(mCurrentState == STATE_PAUSED){
                    mMediaPlayer.resume();
                }else{
                    mMediaPlayer.start();
                }
                mCurrentState = STATE_PLAYING;
                setRenderMode(RENDERMODE_CONTINUOUSLY); // set render mode RENDERMODE_CONTINUOUSLY
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
                    setRenderMode(RENDERMODE_WHEN_DIRTY); // set render mode RENDERMODE_WHEN_DIRTY
                    mMediaPlayer.pause();
                    mCurrentState = STATE_PAUSED;
                }
            }
            mTargetState = STATE_PAUSED;
        } catch (Exception e) {

        }
    }

    @Override
    public void stopPlayback() {
        Log.i("chenyg", "stopPlayback()");
        if (mMediaPlayer != null) {
            mDuration = -1;
            mLastPositionOnCompletion = 0;

            setRenderMode(RENDERMODE_WHEN_DIRTY);   // set render mode
            mMediaPlayer.stop();
            mMediaPlayer.release();
            mMediaPlayer = null;
            mCurrentState = STATE_IDLE;
            mTargetState = STATE_IDLE;
//            AudioManager am = (AudioManager) mAppContext.getSystemService(
//                    Context.AUDIO_SERVICE);
//            am.abandonAudioFocus(null);
        }
    }

    @Override
    public void seekTo(int msec) {
        if (isInPlaybackState()) {
            setRenderMode(RENDERMODE_WHEN_DIRTY);   // set render mode
            mMediaPlayer.seekTo(msec);
            mCurrentState = STATE_SEEKING;
            mSeekWhenPrepared = 0;
        } else {
            mSeekWhenPrepared = msec;
        }
    }
}
