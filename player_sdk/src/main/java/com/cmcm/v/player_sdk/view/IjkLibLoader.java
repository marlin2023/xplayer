package com.cmcm.v.player_sdk.view;

public interface IjkLibLoader {
    void loadLibrary(String libName) throws UnsatisfiedLinkError, SecurityException;
}