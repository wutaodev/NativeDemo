package com.example.wutao.nativedemo;

import android.content.Context;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

/**
 * 两种方式 native 回调 java
 */
public class MainActivity extends AppCompatActivity implements OnNativeCallback {
    private static final String TAG = "MainActivity";


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

    }

    public void onCallback(View view) {

        switch (view.getId()) {
            case R.id.button1:
                NativeUtils.setCallback1(this);
                break;
            case R.id.button2:
                NativeUtils.setCallback2(this);
                break;
           case R.id.button3:
                NativeUtils.destroy();
                break;
        }
    }

    @Override
    public void callbackForNative(int i) {
        Log.d(TAG, "callbackForNative: " + i);
    }

}
