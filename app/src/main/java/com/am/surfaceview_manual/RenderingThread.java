package com.am.surfaceview_manual;

import android.opengl.GLES20;
import android.view.Surface;
import android.view.SurfaceHolder;

class RenderingThread extends Thread {
    private SurfaceHolder surfaceHolder;
    private boolean running = false;
    private EGLHelper eglHelper;

    public RenderingThread(SurfaceHolder surfaceHolder) {
        this.surfaceHolder = surfaceHolder;
    }

    @Override
    public void run() {
        Surface surface = surfaceHolder.getSurface();
        eglHelper = new EGLHelper(surface);
        eglHelper.init();

        while (running) {
            // 渲染逻辑...

            eglHelper.DrawFrame();

            // 交换缓冲区，显示绘制内容
            eglHelper.swapBuffers();
        }

        eglHelper.release();
    }

    public void setRunning(boolean running) {
        this.running = running;
    }
}
