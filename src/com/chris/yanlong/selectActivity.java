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
import android.widget.EditText;
import android.widget.MediaController;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.VideoView;
import android.view.View.OnClickListener;
import com.chris.yanlong.PlayerView;

public class selectActivity extends Activity {

	private EditText file_name;
	private Button btn_play;
	
	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.select_file);
		
		file_name = (EditText)findViewById(R.id.editText1);
//		file_name.setText("/mnt/sdcard/b3.mp4");
		file_name.setText("http://192.168.1.26:9000/chris/amite.m3u8");
		
//		file_name.setText("http://192.168.1.15:9000/amite/amite.m3u8");
//		file_name.setText("http://192.168.1.15:9000/test/amite.m3u8");
//		file_name.setText("http://192.168.1.15:9000/ruguo2/amite.m3u8");
//		file_name.setText("http://192.168.1.15:9000/280/amite.m3u8");
//		file_name.setText("http://192.168.1.15:9000/280.ts");
		
		
		btn_play = (Button)findViewById(R.id.button1);
		btn_play.setOnClickListener(new Button.OnClickListener() {
			
			public void onClick(View v) {
				// TODO Auto-generated method stub
			
				Intent intent = new Intent();
                intent.setClass(selectActivity.this, player01Activity.class);
               
                Bundle bundle = new Bundle();
                bundle.putString("url", file_name.getText().toString());  
                
                Log.i("chris_magic", "url = " + file_name.getText().toString());
                setResult(RESULT_CANCELED, intent.putExtras(bundle));   //使用bundle 要把这个添加上。。
                
                startActivity(intent);
                finish();  //this activity is game over
			}
			
		});
		
		
	}
	
}