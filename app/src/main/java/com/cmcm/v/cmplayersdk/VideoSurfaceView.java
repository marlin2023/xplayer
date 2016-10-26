package com.cmcm.v.cmplayersdk;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;

import org.greenrobot.eventbus.EventBus;

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

public class VideoSurfaceView extends GLSurfaceView implements IMediaPlayer ,GLSurfaceView.Renderer{

    public static String TAG = "VideoSurfaceView";

    XPlayer xPlayer = null ;

    boolean isEGLContextInitilized;


    public VideoSurfaceView(Context context) {
        super(context);
        Log.i(TAG, "VideoSurfaceView Construct...");

        isEGLContextInitilized = false;
        //if(xPlayer == null){
        xPlayer = new XPlayer();
        //}

        setEGLContextClientVersion(2);                  // set opengl es version
        // setEGLConfigChooser(8, 8, 8, 8, 8, 0);
        // getHolder().setFormat(PixelFormat.RGBA_8888);
        // bind the render to the surfaceivew
        setRenderer(this);
        setRenderMode(RENDERMODE_WHEN_DIRTY);           // set render mode
        setZOrderOnTop(true);
        Log.i(TAG, "VideoSurfaceView Construct ok...");
    }

    public VideoSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);

    }


    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        Log.i(TAG ,"========>onSurfaceCreated");
        // create gl program
        if(!isEGLContextInitilized){
            this.xPlayer.initEGLCtx();
            isEGLContextInitilized = true;
        }

        //
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {

    }

    @Override
    public void onDrawFrame(GL10 gl) {

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
        if(xPlayer != null){
            xPlayer.init();
            Log.i(TAG, "after xPlayer.init()  .........");
        }

        // end
    }

    @Override
    public void setDisplay(SurfaceHolder sh) {

    }

    @Override
    public void setDataSource(String path) {
        // call xplayer setDataSource path.
        xPlayer.setDataSource(path);

    }

    @Override
    public String getDataSource() {
        return null;
    }

    @Override
    public void prepareAsync() throws IllegalStateException {
        xPlayer.prepareAsync();
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
        // set xplayer listener
        xPlayer.setOnPreparedListener(listener);
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


    //---
    // prepared listener implement
    protected IMediaPlayer.OnPreparedListener mPreparedListener =
            new IMediaPlayer.OnPreparedListener() {

                public void onPrepared(IMediaPlayer mp) {

                }
            };

    // buffer update listener
    protected IMediaPlayer.OnBufferingUpdateListener mBufferingUpdateListener =
            new IMediaPlayer.OnBufferingUpdateListener() {
                public void onBufferingUpdate(IMediaPlayer mp, int percent) {

                }
            };


}
