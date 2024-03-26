package com.am.surfaceview_manual;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.content.pm.PackageManager;
import android.Manifest;
import android.os.Bundle;
import android.view.Surface;
import android.view.SurfaceHolder;

public class MainActivity extends AppCompatActivity {

    static {
        System.loadLibrary("RenderLib");
    }

    private MySurfaceView surfaceView;

    private long eglManagerPtr = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        surfaceView = findViewById(R.id.my_surface_view);


        if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1);
        }





//        surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
//            @Override
//            public void surfaceCreated(SurfaceHolder holder) {
//                eglManagerPtr = createEGLManager(holder.getSurface());
//            }
//
//            @Override
//            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
//                drawFrame(eglManagerPtr);
//            }
//
//            @Override
//            public void surfaceDestroyed(SurfaceHolder holder) {
//                if (eglManagerPtr != 0) {
//                    destroyEGLManager(eglManagerPtr);
//                    eglManagerPtr = 0;
//                }
//            }
//        });

    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        switch (requestCode) {
            case 1: // 与requestPermissions()中的requestCode相对应
                if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    // 权限被用户同意，可以执行文件读写操作
                } else {
                    // 权限被用户拒绝，不能执行文件读写操作
                }
                break;
        }
    }
}