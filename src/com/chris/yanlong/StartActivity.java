package com.chris.yanlong;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import android.app.Activity;
import android.content.ContentResolver;
import android.content.Intent;
import android.database.Cursor;
import android.media.ThumbnailUtils;
import android.os.Bundle;
import android.provider.MediaStore;
import android.provider.MediaStore.Video;
import android.provider.MediaStore.Video.Thumbnails;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ListView;
import android.widget.SimpleAdapter;


public class StartActivity extends Activity {

	private List<FileModel> lstFile = new ArrayList<FileModel>();  //结果 List
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.start);
		
		ListView listView=(ListView)findViewById(R.id.listView_allFiles);
		
		SimpleAdapter adapter=new SimpleAdapter(this, getData(), R.layout.list_item, new String[]{"img","title","info"}, new int[]{R.id.img,R.id.title,R.id.info});
		listView.setAdapter(adapter);
		
		listView.setOnItemClickListener(new OnItemClickListener() {

			@Override
			public void onItemClick(AdapterView<?> arg0, View arg1, int arg2,long arg3) {
				Intent intent = new Intent();
                intent.setClass(StartActivity.this, player01Activity.class);
               
                Bundle bundle = new Bundle();
                bundle.putString("url", lstFile.get(arg2).getFilePath());  
          
                setResult(RESULT_CANCELED, intent.putExtras(bundle));
                startActivity(intent);
                finish();  //this activity is game over
			}
		});
	}

	private List<Map<String, Object>> getData() {
		GetFiles();
		
        List<Map<String, Object>> list = new ArrayList<Map<String, Object>>();
 
        for(FileModel file:lstFile){
            Map<String, Object> map = new HashMap<String, Object>();
            map.put("title", file.getFileName());
            map.put("info", file.getFilePath());
            map.put("img", ThumbnailUtils.createVideoThumbnail(file.getFileThumbnailPath(), Video.Thumbnails.MINI_KIND));
            list.add(map);
        }
         
        return list;
    }
	

	private void GetFiles() {
		ContentResolver contentResolver = getContentResolver();
		Cursor cursor = contentResolver.query(
				MediaStore.Video.Media.EXTERNAL_CONTENT_URI, null, null,
				null, MediaStore.Video.Media.DEFAULT_SORT_ORDER);
		try {
			while (cursor.moveToNext()) {
				String id = cursor.getString(cursor.getColumnIndexOrThrow(MediaStore.Video.Media._ID));
				String fileName = cursor.getString(cursor.getColumnIndexOrThrow(MediaStore.Video.Media.DISPLAY_NAME));
				String filePath = cursor.getString(cursor.getColumnIndexOrThrow(MediaStore.Video.Media.DATA));
				long fileSize = cursor.getLong(cursor.getColumnIndexOrThrow(MediaStore.Video.Media.SIZE));
			
			
				//if file exist  
	            if (new File(filePath).exists()) {   
	            	
	                String fileThumbnailPath="";  
	                Cursor curThumb = getContentResolver().query(Thumbnails.EXTERNAL_CONTENT_URI, null, Thumbnails.VIDEO_ID+"=?" ,new String[]{id}, null);  
	                while (curThumb.moveToNext()) {  
	                    fileThumbnailPath = curThumb.getString(curThumb.getColumnIndexOrThrow(Thumbnails.DATA));  
	                }
	                curThumb.close();//TODO error?
	                  
	                FileModel model = new FileModel();
	                model.setFileName(fileName);  
	                model.setFilePath(filePath);  
	                model.setFileThumbnailPath(fileThumbnailPath);
	                model.setFileSize(fileSize); 
	                lstFile.add(model);  
	            }  
			}
		} catch (Exception e) {
			e.printStackTrace();
		}finally{
			if(cursor!=null){
				cursor.close();
			}
		}
		
		
	}
}
