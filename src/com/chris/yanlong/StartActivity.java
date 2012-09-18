package com.chris.yanlong;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import android.app.Activity;
import android.content.ContentResolver;
import android.content.Intent;
import android.database.Cursor;
import android.os.Bundle;
import android.provider.MediaStore;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ListView;
import android.widget.SimpleAdapter;

public class StartActivity extends Activity {

	private List<String> lstFile = new ArrayList<String>();  //结果 List
	private ListView listView;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.start);
		
		listView=(ListView)findViewById(R.id.listView_allFiles);
		
		SimpleAdapter adapter=new SimpleAdapter(this, getData(), R.layout.list_item, new String[]{"img","title","info"}, new int[]{R.id.img,R.id.title,R.id.info});
		listView.setAdapter(adapter);
		
		listView.setOnItemClickListener(new OnItemClickListener() {

			@Override
			public void onItemClick(AdapterView<?> arg0, View arg1, int arg2,long arg3) {
				Intent intent = new Intent();
                intent.setClass(StartActivity.this, player01Activity.class);
               
                Bundle bundle = new Bundle();
                bundle.putString("url", lstFile.get(arg2));  
          
                setResult(RESULT_CANCELED, intent.putExtras(bundle));
                startActivity(intent);
                finish();  //this activity is game over
			}
		});
	}

	private List<Map<String, Object>> getData() {
		GetFiles();
		
        List<Map<String, Object>> list = new ArrayList<Map<String, Object>>();
 
        for(String title:lstFile){
            Map<String, Object> map = new HashMap<String, Object>();
            map.put("title", title);
            map.put("info", "infomation");
//          map.put("img", R.drawable.i1);
            list.add(map);
        }
         
        return list;
    }
	

	private void GetFiles() {
		ContentResolver contentResolver = getContentResolver();
		String[] projection = new String[] { MediaStore.Video.Media.DATA };
		Cursor cursor = contentResolver.query(
				MediaStore.Video.Media.EXTERNAL_CONTENT_URI, projection, null,
				null, MediaStore.Video.Media.DEFAULT_SORT_ORDER);
		cursor.moveToFirst();
		int fileNum = cursor.getCount();

		for (int counter = 0; counter < fileNum; counter++) {
			lstFile.add(cursor.getString(cursor.getColumnIndex(MediaStore.Video.Media.DATA)));
			cursor.moveToNext();
		}
		cursor.close();
	}
}
