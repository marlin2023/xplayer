package com.chris.yanlong;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.graphics.RectF;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.WindowManager;

public class PlayerView extends SurfaceView implements SurfaceHolder.Callback {

	static {
		System.loadLibrary("player");
	}

	// jni

	/* native function */
	private native int init_media(String url, Integer width_src,
			Integer height_src, Integer sample_src);

	private native int read_thread(int handle);

	private native byte[] get_video_frame(int handle, Double video_frame_pts,
			ByteBuffer buf ,int back_play_mark);

	private native byte[] get_audio_frame(int handle, Double audio_frame_pts,
			Integer audio_frame_size, ByteBuffer audio_buffer);

	private native int player_stop(int handle);
	
	private native int player_release(int handle);
	
	private native int seek_frame(int handle, int progress_bar);
	
	/*data*/
	
	private static final String TAG = "chris_magic";
	private static final int MAX_DIFF = 800; // 200 ms
	
	//control
	private int pause_mark ;
	private int stop_mark;
	
	//play back ground mark
	private int back_ground_mark;

	Integer src_width = new Integer(0);
	Integer src_height = new Integer(0);
	Integer src_sample = new Integer(0);

	SurfaceHolder holder;
	Bitmap bmap;
	byte[] videoData;
	int media_handle = 0;

	/* audio */
	AudioTrack trackNo1;
	int m_out_buf_size;
	byte[] m_out_bytes;

	Double video_pts ;
	Double audio_pts = new Double(0.0);
	Integer audio_frame_size = new Integer(0);

	public PlayerView(Context context) {
		super(context);
		video_pts = new Double(0.0);
		pause_mark = 0;
		stop_mark = 0;
		back_ground_mark = 0;

	}

	public PlayerView(Context context, AttributeSet attrs) {
		super(context, attrs);
		video_pts = new Double(0.0);
		pause_mark = 0;
		stop_mark = 0;
		back_ground_mark = 0;
	}

	public PlayerView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		video_pts = new Double(0.0);
		pause_mark = 0;
		stop_mark = 0;
		back_ground_mark = 0;
	}

	public void surfaceChanged(SurfaceHolder holder, int format, int width,
			int height) {

	}

	public void surfaceCreated(SurfaceHolder holder) {
		Log.i("chris_magic", "before view surface show..");
		//
		new Thread(new PlayVideoThread()).start();
//		
		
		Log.i("chris_magic", "audio thread and video thread is over.....");
	}

	public void surfaceDestroyed(SurfaceHolder holder) {
		Log.i("chris_magic", "surface ....");
	}

//======================== interface for android app=========================
	public void init(String url){
		
		media_handle = init_player(url);
		video_pts = new Double(0.0);
		Log.i(TAG, "constrctor..1111." + src_width + "," + src_height + ","
				+ src_sample);
		
		holder = this.getHolder();
		holder.addCallback(this);
		holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
		
		bmap = Bitmap
				.createBitmap(src_width, src_height, Bitmap.Config.RGB_565);

		/* audio */
		m_out_buf_size = AudioTrack.getMinBufferSize(src_sample,
				AudioFormat.CHANNEL_CONFIGURATION_STEREO, // CHANNEL_CONFIGURATION_MONO,
				AudioFormat.ENCODING_PCM_16BIT);

		trackNo1 = new AudioTrack(AudioManager.STREAM_MUSIC,
				src_sample,
				AudioFormat.CHANNEL_CONFIGURATION_STEREO, // CHANNEL_CONFIGURATION_MONO,
				AudioFormat.ENCODING_PCM_16BIT, m_out_buf_size,
				AudioTrack.MODE_STREAM);
		
	}
	
	
	public int play(){
		
		/* read_thread */
		new Thread(new readThread()).start();
		
		Log.e(TAG, "...play is over");
		return 0;
	}
	
	public void pause_start(){
		
		pause_mark = 1;
	}	
	
	public void pause_end(){
		
		pause_mark = 0;
	}	
	
	
	public void stop(){
		
		stop_mark = 1;
		
		player_stop(media_handle);
		
		
		Log.e(TAG ,"==========================");
		//����ĳ���̡߳�
		player_release(media_handle);
		
	}
	public int seek(int progress_bar) {
		Log.i(TAG, "invoke the seek function.. ,media_handle = " + media_handle
				+ "," + progress_bar);
		pause_mark = 1;
		seek_frame(media_handle, progress_bar);
		pause_mark = 0;
		return 0;

	}

	public void play_back_ground_start(){
		
		back_ground_mark = 1; 	//play in background ,only audio .
	}
	
	public void play_back_ground_stop(){
		back_ground_mark = 0;
	}
	

//=================================================================================
	
	/*
	 * 	init function
	 * 
	 * */
	public int init_player( String url){
		Log.i("chris_magic", "========before init media_handle...");

		int player_handle = init_media(url, src_width,
				src_height, src_sample);
		
		Log.i("chris_magic", "after init meida_handle =" + player_handle);
		
		return player_handle;
	}
	
	
	/* audio */
	class PlayAudioThread implements Runnable {

		public void run() {

			Log.d(TAG, "PlayAudioThread run ,media_handle = " + media_handle);

			ByteBuffer audio_buffer = ByteBuffer.allocateDirect(m_out_buf_size);
			trackNo1.play();
			Log.d(TAG, " after track play ,media_hanle = " + media_handle);

			while (true) {
				
				if (video_pts > 0) {
					Log.e(TAG, "jjjjjjjjjjjjjjjjjjjjjjj");
					break;
				}
			}

			while (true) {
				try {
					// read pcm data
					get_audio_frame(media_handle, audio_pts, audio_frame_size,
							audio_buffer);

					if(pause_mark == 1){
						while(true){
							if(pause_mark == 0) break;
						}
					}else if(stop_mark == 1){
						trackNo1.stop();
						trackNo1.release();
						Log.e(TAG, "audio thread over...");
						return ;
//						break;   //end this play_audio_thread
					}
					// mem copy
					// Log.d(TAG, " audio_frame_size =  " + audio_frame_size);
					trackNo1.write(audio_buffer.array(), 0, audio_frame_size);
					

				} catch (Exception e) {
					// TODO: handle exception
					e.printStackTrace();
					break;
				}

			}
		}

	}

	/* video */
	class PlayVideoThread implements Runnable {

		public void run() {

			Canvas canvas;// = holder.lockCanvas(null);// 闁兼儳鍢茶ぐ鍥偨鐠囪尙顏�
			ByteBuffer buf = ByteBuffer.allocateDirect(src_width * src_height
					* 2);
			buf.order(ByteOrder.nativeOrder());
			// Bitmap ��ת

			DisplayMetrics dm = getResources().getDisplayMetrics();
			int mScreenWidth = dm.widthPixels;
			int mScreenHeight = dm.heightPixels;
			Rect mrect = new Rect(0, 0, mScreenWidth, mScreenHeight);
			Log.i(TAG, "..........width =" + mScreenWidth + ",hegiht = "
					+ mScreenHeight);

			new Thread(new PlayAudioThread()).start();
			
			while (true) {

				try {

//					get_video_frame(media_handle, video_pts, buf);
					get_video_frame(media_handle, video_pts, buf ,back_ground_mark);
					
					double diff_tmie = video_pts - audio_pts;
					if (diff_tmie > 0) {
						if(diff_tmie > MAX_DIFF * 10) {
							Log.i(TAG, "..............................drop video frame ....");
							continue;
						}
						
						//Log.i(TAG, "diff_tmie = " + diff_tmie);
						Thread.sleep((long) diff_tmie);
						
						if(back_ground_mark == 1){
							Log.i(TAG, "continue ....");
							continue;
						}
					} else {
						if (diff_tmie < -MAX_DIFF) {

							Log.i(TAG, "drop video frame ....");
							continue;

						}
					}
					
					if(pause_mark == 1){
							while(true){
								if(pause_mark == 0) break;
							}
					}else if(stop_mark == 1){

						Log.e(TAG, "video thread over...");
						break;
					}
//					Log.i(TAG, "...before lockCanvas ...");
					canvas = holder.lockCanvas(null);
					bmap.copyPixelsFromBuffer(buf);
					canvas.drawBitmap(bmap, null, mrect, null);
					holder.unlockCanvasAndPost(canvas);//

				} catch (Exception e) {
					// TODO: handle exception
					e.printStackTrace();
					break;
				}

			}
		}

	}// end mythread

	class readThread implements Runnable {

		public void run() {

			read_thread(media_handle);
			
			Log.i(TAG, "read thread over ....");
			// ;
		}

	}

}