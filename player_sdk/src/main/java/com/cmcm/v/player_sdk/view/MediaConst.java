package com.cmcm.v.player_sdk.view;

/**
 * Created by i on 2016/5/23.
 */
public class MediaConst {

    //onError的what为-1
    public static final int MEDIA_ERROR_OPEN_VIDEO_EXCEPTION = -1;                  //openVideo时有异常

    //onError的what为-1时的extra
    public static final int ERROR_EXTRA_ILLEGAL_ARG = -1;                           //IllegalArgumentException
    public static final int ERROR_EXTRA_SECURITY = -2;                              //SecurityException
    public static final int ERROR_EXTRA_ILLEGAL_STAT = -3;                          //IllegalStateException
    public static final int ERROR_EXTRA_IO = -4;                                    //IOException
    public static final int ERROR_EXTRA_EXCETPION = -5;                             //Exception
//    public static final int ERROR_EXTRA_ERROR = -6;                                 //Error
    public static final int ERROR_EXTRA_UNSATISFIEDLINK = -7;                       //UnsatisfiedLinkError
}
