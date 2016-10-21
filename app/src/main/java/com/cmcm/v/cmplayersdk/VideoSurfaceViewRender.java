package com.cmcm.v.cmplayersdk;

import android.opengl.GLSurfaceView;

import android.opengl.GLSurfaceView.Renderer;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by chris on 10/21/16.
 */

public class VideoSurfaceViewRender implements GLSurfaceView.Renderer {

    public static String TAG = "VideoSurfaceViewRender";

    XPlayer xPlayer = null ;// = new XPlayer();

    boolean isInitilized;

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        Log.i(TAG ,"========>onSurfaceCreated");
        isInitilized = false;
        if(xPlayer == null){
            xPlayer = new XPlayer();
            xPlayer.init();
            isInitilized = true;
        }

    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        Log.i(TAG ,"========>onSurfaceChanged");

    }

    @Override
    public void onDrawFrame(GL10 gl) {

        if(isInitilized){
            Log.i(TAG ,"========>onDrawFrame");
            xPlayer.renderFrame();
        }


    }

}
