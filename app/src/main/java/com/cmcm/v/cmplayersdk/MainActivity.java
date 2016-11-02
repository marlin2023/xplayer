package com.cmcm.v.cmplayersdk;

import android.app.Activity;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.MediaController;

import com.cmcm.v.player_sdk.builder.CMPlayerBuilder;
import com.cmcm.v.player_sdk.player.IMediaPlayer;
import com.cmcm.v.player_sdk.view.CMPlayerControl;
import com.cmcm.v.player_sdk.view.VideoSurfaceView;
import com.cmcm.v.player_sdk.view.VideoViewAndroid;

public class MainActivity extends AppCompatActivity implements View.OnClickListener{

    public static String TAG = "MainActivity";

    CMPlayerControl surfaceView;   // video view
    //VideoViewAndroid surfaceView;

    int isCalledPlay ;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        isCalledPlay = 0;   // TODO
        // 取得LinearLayout 物件
        FrameLayout ll = (FrameLayout)findViewById(R.id.viewObj);

        surfaceView = CMPlayerBuilder.getInstance().build(this,
                //CMPlayerBuilder.Type.ANDROID,
                CMPlayerBuilder.Type.IJKSW,
                null);
        ll.addView(surfaceView.getView());

        surfaceView.setOnPreparedListener(mOnPreparedListener);
        surfaceView.setVideoPath("/sdcard/hh.mp4");
//        surfaceView.start();
    }


    private IMediaPlayer.OnPreparedListener mOnPreparedListener =
            new IMediaPlayer.OnPreparedListener() {
                public void onPrepared(IMediaPlayer mp) {
                    Log.d(TAG ,"====>onPrepared....");
                    // call start function
                    surfaceView.start();
                }

            };

    @Override
    public void onClick(View v) {
        switch (v.getId()){
//            case R.id.btn:
//            //surfaceView.prepareAsync();
//            break;
        }

    }

}

