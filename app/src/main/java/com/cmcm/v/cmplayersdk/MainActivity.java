package com.cmcm.v.cmplayersdk;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.LinearLayout;

public class MainActivity extends AppCompatActivity {

    public static String TAG = "MainActivity";

    VideoSurfaceView surfaceView;   // video view

    int isCalledPlay ;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        isCalledPlay = 0;   // TODO
        // 取得LinearLayout 物件
        LinearLayout ll = (LinearLayout)findViewById(R.id.viewObj);

        surfaceView = new VideoSurfaceView(this);
        ll.addView(surfaceView);

        surfaceView.setOnPreparedListener(mOnPreparedListener);
        surfaceView.setOnBufferingUpdateListener(mOnBufferingUpdateListener);

        surfaceView.init(); //will not create opengl program.
        surfaceView.setDataSource("/sdcard/hh.mp4");
        surfaceView.prepareAsync();

    }



    // Need Java Layer implements
    private IMediaPlayer.OnPreparedListener mOnPreparedListener =
            new IMediaPlayer.OnPreparedListener() {
                public void onPrepared(IMediaPlayer mp) {
                    Log.d(TAG ,"====>onPrepared....");
                    // call start function
                    surfaceView.start();
                }

            };

    private IMediaPlayer.OnBufferingUpdateListener mOnBufferingUpdateListener =
            new IMediaPlayer.OnBufferingUpdateListener() {
                @Override
                public void onBufferingUpdate(IMediaPlayer mp, int percent) {
                    Log.d(TAG ,"====>onBufferingUpdate...." + percent + "% ,isCalledPlay =" + isCalledPlay );
                    // TODO

                    if((percent == 100) && (isCalledPlay == 0)){
                        isCalledPlay = 1;
                        surfaceView.play();
                    }
                }
            };



}

