package com.chris.yanlong;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.MediaController;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.VideoView;
import android.view.View.OnClickListener;
import com.chris.yanlong.PlayerView;

public class player01Activity extends Activity {

	private Button btn1;
	private Button btn_pause;
	private Button btn_stop;

	private SeekBar seekBar1;

	private Button back_play_start;
	private Button back_play_stop;

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		// 得到激活它的意图
		Intent intent = this.getIntent();
		Bundle extras = intent.getExtras();
		// 得到数据
		String file_name = extras.getString("url");

		requestWindowFeature(Window.FEATURE_NO_TITLE);// 填充标题栏
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);

		//禁止锁屏
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON,
				WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		setContentView(R.layout.main);

		btn1 = (Button) findViewById(R.id.btn_1);
		btn_pause = (Button) findViewById(R.id.btn_2);
		btn_stop = (Button) findViewById(R.id.btn_stop);

		back_play_start = (Button) findViewById(R.id.btn_back_start);
		back_play_stop = (Button) findViewById(R.id.btn_back_stop);
		final PlayerView playview = (PlayerView) findViewById(R.id.surfaceView1);

		Log.i("chris_magic", "file=--" + file_name + "--");
		playview.init(file_name);

		playview.play();

		if (btn1 == null) {
			Log.i("chris_magic", "dddddddddddddddddddddddddddd");

		}
		btn1.setOnClickListener(new Button.OnClickListener() {

			public void onClick(View v) {
				// TODO Auto-generated method stub
				playview.pause_end();
			}
		});

		btn_pause.setOnClickListener(new Button.OnClickListener() {

			public void onClick(View v) {
				// TODO Auto-generated method stub
				btn1.setText("play");
				playview.pause_start();

			}
		});

		btn_stop.setOnClickListener(new Button.OnClickListener() {

			public void onClick(View v) {
				// TODO Auto-generated method stub
				btn1.setText("stop ..");
				playview.stop();

				finish(); // exit current activity ..
			}
		});

		back_play_start.setOnClickListener(new Button.OnClickListener() {

			public void onClick(View v) {
				// TODO Auto-generated method stub
				btn1.setText("back_play..");
				playview.play_back_ground_start();
			}
		});

		back_play_stop.setOnClickListener(new Button.OnClickListener() {

			public void onClick(View v) {
				// TODO Auto-generated method stub
				btn1.setText(" stop back_play..");
				playview.play_back_ground_stop();
			}
		});
		seekBar1 = (SeekBar) findViewById(R.id.l014_seekBar2);
		seekBar1.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {

			public void onProgressChanged(SeekBar seekBar, int progress,
					boolean fromUser) {
				// TODO Auto-generated method stub

				btn1.setText("0000");
			}

			public void onStartTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub

				btn1.setText("11111");
			}

			public void onStopTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub

				btn1.setText("---" + seekBar1.getProgress());
				// call seek_frame

				playview.seek(seekBar1.getProgress());
			}
		});

	}

}