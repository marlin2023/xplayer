package com.cmcm.v.player_sdk.view;

import android.content.Context;
import android.graphics.Bitmap;
import android.net.Uri;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;
import android.widget.MediaController;

import com.cmcm.v.player_sdk.player.IMediaPlayer;
import com.cmcm.v.player_sdk.player.XPlayer;

import java.util.Map;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by chris on 10/20/16.
 * 1) TextureView+SurfaceTexture+OpenGL ES来播放视频
 *           http://www.jianshu.com/p/d3d3186eefcb
 * 2) Actvity will new this object ,and add it.
 *
 */

public class VideoSurfaceView extends GLSurfaceView implements GLSurfaceView.Renderer ,CMPlayerControl{

    public static String TAG = "VideoSurfaceView";

    XPlayer xPlayer = null ;

    boolean isEGLContextInitilized;

    // set VIDEO_LAYOUT_SCALE as default.
    private int mVideoLayout = VIDEO_LAYOUT_FIT_PARENT;

    protected int mVideoWidth;
    protected int mVideoHeight;
    protected int mVideoSarNum;
    protected int mVideoSarDen;

    private int mVideoRotationDegree = 0;

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
    public int getVideoLayout() {
        return mVideoLayout;
    }

    @Override
    public void setVideoLayout(int layout) {
        mVideoLayout = layout;
        requestLayout();
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

        // TODO openVideo abstract function
        xPlayer.setDataSource(uri.getPath());
//        requestLayout();
//        invalidate();
    }

    @Override
    public void stopPlayback() {

    }

    @Override
    public void setMediaController(MediaController controller) {

    }


    @Override
    public void release(boolean clearTargetState) {

    }

    @Override
    public View getView() {
        return null;
    }

    @Override
    public int getCurrentState() {
        return 0;
    }


    @Override
    public void setVideoRotation(int degree) {

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

    @Override
    public void pause() {

    }

    @Override
    public int getDuration() {
        return 0;
    }

    @Override
    public int getCurrentPosition() {
        return 0;
    }

    @Override
    public void seekTo(int pos) {

    }

    @Override
    public boolean isPlaying() {
        return false;
    }

    @Override
    public int getBufferPercentage() {
        return 0;
    }

    @Override
    public boolean canPause() {
        return false;
    }

    @Override
    public boolean canSeekBackward() {
        return false;
    }

    @Override
    public boolean canSeekForward() {
        return false;
    }

    @Override
    public int getAudioSessionId() {
        return 0;
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


    // set listener start

    @Override
    public void setOnPreparedListener(IMediaPlayer.OnPreparedListener l) {
        xPlayer.setOnPreparedListener(l);
    }

    @Override
    public void setOnCompletionListener(IMediaPlayer.OnCompletionListener l) {
        xPlayer.setOnCompletionListener(l);
    }

    @Override
    public void setOnBufferingUpdateListener(IMediaPlayer.OnBufferingUpdateListener l) {
        xPlayer.setOnBufferingUpdateListener(l);
    }

    @Override
    public void setOnSeekCompleteListener(IMediaPlayer.OnSeekCompleteListener l) {

    }

    @Override
    public void setOnVideoSizeChangedListener(IMediaPlayer.OnVideoSizeChangedListener l) {

    }

    @Override
    public void setOnErrorListener(IMediaPlayer.OnErrorListener l) {

    }

    @Override
    public void setOnInfoListener(IMediaPlayer.OnInfoListener l) {

    }
    // set listener end

}
