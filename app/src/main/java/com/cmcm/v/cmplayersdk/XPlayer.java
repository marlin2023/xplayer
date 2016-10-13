package com.cmcm.v.cmplayersdk;

/**
 * Created by chris on 9/26/16.
 */
public class XPlayer {

    static {
        try {
            System.loadLibrary("ffmpeg");
            System.loadLibrary("cmxplayer");

        } catch(Exception e) {
            e.printStackTrace(System.out);
        }
    }


    // native function
    public native void sayHello();

}
