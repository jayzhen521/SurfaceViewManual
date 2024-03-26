package com.am.surfaceview_manual;

import android.content.Context;
import android.util.AttributeSet;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class MySurfaceView extends SurfaceView implements SurfaceHolder.Callback {
    private RenderingThread renderingThread;

    public MySurfaceView(Context context) {
        super(context);
        getHolder().addCallback(this);
    }

    public MySurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        getHolder().addCallback(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        renderingThread = new RenderingThread(getHolder());
        renderingThread.setRunning(true);
        renderingThread.start();
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        // 通常，你不需要在这里重启线程，除非你需要处理尺寸变化
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        boolean retry = true;
        renderingThread.setRunning(false);
        while (retry) {
            try {
                renderingThread.join();
                retry = false;
            } catch (InterruptedException e) {
                // 重试
            }
        }
    }
}
