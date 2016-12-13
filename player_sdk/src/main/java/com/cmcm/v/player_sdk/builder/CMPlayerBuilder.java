package com.cmcm.v.player_sdk.builder;

import android.content.Context;
import android.util.Log;

import com.cmcm.v.player_sdk.view.CMPlayerControl;
import com.cmcm.v.player_sdk.view.IjkLibLoader;
import com.cmcm.v.player_sdk.view.VideoSurfaceView;
import com.cmcm.v.player_sdk.view.VideoViewAndroid;

public class CMPlayerBuilder {
    private static final String TAG = "CMPlayer";
    private static final String SDK_VERSION = "v1.0";

    public enum Type {
        ANDROID,    // android default player
        IJKSW,      // ijkplayer with software decoder
        IJKHW       // ijkplayer with hardware decoder
    }

    private static class SingletonHolder {
        private static final CMPlayerBuilder INSTANCE = new CMPlayerBuilder();
    }

    private CMPlayerBuilder() {}

    public static final CMPlayerBuilder getInstance() {
        return SingletonHolder.INSTANCE;
    }

    public CMPlayerControl build(Context context, Type type, IjkLibLoader libLoader) {
        CMPlayerControl view = null;
        switch(type) {
            case ANDROID:
                Log.i(TAG, "creat Android player");
                view = new VideoViewAndroid(context, libLoader);
                break;
            case IJKSW:
                Log.i(TAG, "creat IjkSW player");
                //view = new VideoSurfaceView(context, libLoader);
                view = new VideoSurfaceView(context, null);
                break;
            default:
        }
        return view;
    }

    /**
     * 根据CMPlayerControl获取播放器类型
     * @param view
     * @return Type.ANDROID，或者 Type.IJKSW，或者 Type.IJKHW
     */
    public Type getType(CMPlayerControl view) {
        Type type = Type.ANDROID;
        if (view instanceof VideoViewAndroid) {
            type = Type.ANDROID;
        } else if (view instanceof VideoSurfaceView) {
            type = Type.IJKSW;
        }
        return type;
    }

    /**
     * 获取版本号
     */
    public String getVersion() {
        return SDK_VERSION;
    }
}
