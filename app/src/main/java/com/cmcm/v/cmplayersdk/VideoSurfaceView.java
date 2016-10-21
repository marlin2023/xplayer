package com.cmcm.v.cmplayersdk;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

/**
 * Created by chris on 10/20/16.
 * 1) TextureView+SurfaceTexture+OpenGL ES来播放视频
 *           http://www.jianshu.com/p/d3d3186eefcb
 * 2)
 *
 */

public class VideoSurfaceView extends GLSurfaceView {

    /**
     * view render
     */
    VideoSurfaceViewRender mRenderer;


    public VideoSurfaceView(Context context) {
        super(context);

        setEGLContextClientVersion(2);  // set opengl es version
        mRenderer = new VideoSurfaceViewRender();

        // bind the render to the surfaceivew
        setRenderer(mRenderer);

    }

    public VideoSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);

    }


}
