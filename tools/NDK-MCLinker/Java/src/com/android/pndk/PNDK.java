package com.android.pndk;

import android.app.Activity;
import android.os.Bundle;

import android.widget.TextView;

public class PNDK extends Activity
{
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
       
        System.loadLibrary("mc_jni");
 
        TextView tv = new TextView(this);
 
        compile();      

        tv.setText("Finish compilation");
        setContentView(tv);
    }

    static public native void compile();
}
