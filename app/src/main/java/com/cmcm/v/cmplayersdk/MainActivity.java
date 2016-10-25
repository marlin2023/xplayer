package com.cmcm.v.cmplayersdk;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.LinearLayout;

public class MainActivity extends AppCompatActivity {

    public static String TAG = "MainActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // 取得LinearLayout 物件
        LinearLayout ll = (LinearLayout)findViewById(R.id.viewObj);

        VideoSurfaceView surfaceView = new VideoSurfaceView(this);
        ll.addView(surfaceView);

        //surfaceView.setOnPreparedListener(mOnPreparedListener);

        surfaceView.init(); //TODO
        surfaceView.setDataSource("/sdcard/hh.mp4");
        surfaceView.prepareAsync();

    }

    private IMediaPlayer.OnPreparedListener mOnPreparedListener =
            new IMediaPlayer.OnPreparedListener() {
                public void onPrepared(IMediaPlayer mp) {
                    Log.i(TAG ,"====>onPrepared....");
                }

            };

}

