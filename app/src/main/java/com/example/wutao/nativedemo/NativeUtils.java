package com.example.wutao.nativedemo;

/**
 * Created by wutao on 2018/4/3.
 */

public class NativeUtils {

    static {
        System.loadLibrary("native-lib");
    }

    public native static void setCallback1(OnNativeCallback callback);

    public native static void setCallback2(OnNativeCallback callback);
}
