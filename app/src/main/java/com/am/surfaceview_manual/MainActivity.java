package com.am.surfaceview_manual;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class MainActivity extends AppCompatActivity {

    static {
        System.loadLibrary("RenderLib");
    }

    private SurfaceView surfaceView;

    private long eglManagerPtr = 0;

    private native long createEGLManager(Surface surface);
    private native void drawFrame(long eglManagerPtr);
    private native void destroyEGLManager(long eglManagerPtr);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        surfaceView = findViewById(R.id.my_surface_view);

        surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                eglManagerPtr = createEGLManager(holder.getSurface());
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                drawFrame(eglManagerPtr);
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                if (eglManagerPtr != 0) {
                    destroyEGLManager(eglManagerPtr);
                    eglManagerPtr = 0; // 重置指针
                }
            }
        });

    }
}