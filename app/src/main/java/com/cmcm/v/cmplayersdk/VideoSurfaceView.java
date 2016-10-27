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

public class VideoSurfaceView extends GLSurfaceView implements GLSurfaceView.Renderer{

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
            this.xPlayer._initEGLCtx();
            isEGLContextInitilized = true;
        }

        //
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {

    }

    @Override
    public void onDrawFrame(GL10 gl) {
        if(xPlayer!= null){
            xPlayer._renderFrame();
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
        if(xPlayer != null){
            xPlayer._init();
            Log.i(TAG, "after xPlayer.init()  .........");
        }
        // end
    }

    public void setDataSource(String path) {
        xPlayer.setDataSource(path);
    }

    public void prepareAsync() throws IllegalStateException {
        xPlayer.prepareAsync();
    }

    /**
     * Start To Play
     * Change State into Buffering State.
     */
    public void start() throws IllegalStateException {
        xPlayer.start();
    }

    public void play(){
        xPlayer.play();
        // set GLSurfaceView.Render MODE
        setRenderMode(RENDERMODE_CONTINUOUSLY); // set render mode RENDERMODE_CONTINUOUSLY
    }

    // =================set listener=======
    public void setOnPreparedListener(IMediaPlayer.OnPreparedListener listener) {
        xPlayer.setOnPreparedListener(listener);
    }

    public void setOnBufferingUpdateListener(IMediaPlayer.OnBufferingUpdateListener listener) {
        xPlayer.setOnBufferingUpdateListener(listener);
    }


    // =================set listener end=======
}
