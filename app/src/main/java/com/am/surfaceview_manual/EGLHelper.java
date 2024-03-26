package com.am.surfaceview_manual;

import android.opengl.EGL14;
import android.opengl.EGLConfig;
import android.opengl.EGLContext;
import android.opengl.EGLDisplay;
import android.opengl.EGLExt;
import android.opengl.EGLSurface;
import android.opengl.GLES20;
import android.util.Log;
import android.view.Surface;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGL10;

public class EGLHelper {
    private EGLDisplay eglDisplay = EGL14.EGL_NO_DISPLAY;
    private EGLContext eglContext = EGL14.EGL_NO_CONTEXT;
    private EGLSurface eglSurface = EGL14.EGL_NO_SURFACE;
    private Surface surface; // 传入SurfaceHolder.getSurface()获取的Surface

    private native long createEGLManager(Surface surface);
    private native void drawFrame(long eglManagerPtr);
    private native void destroyEGLManager(long eglManagerPtr);

    private long eglManagerPtr = 0;

    private float x = 0.0f;

    int positionHandle = 0;
    int texCoordHandle = 0;
    int textureHandle = 0;
    FloatBuffer vertexBuffer;
    FloatBuffer texCoordBuffer;

    int textureId = -1;
    int fboId = -1;

    public EGLHelper(Surface surface) {
        this.surface = surface;
    }

    public void init() {

//        eglInit();

        manualEGLInit(surface);

//        renderInit();

    }

    public void swapBuffers() {
        EGL14.eglSwapBuffers(eglDisplay, eglSurface);
    }

    public void release() {

        if (eglManagerPtr != 0) {
            destroyEGLManager(eglManagerPtr);
            eglManagerPtr = 0;
        }

        if (eglDisplay != EGL14.EGL_NO_DISPLAY) {
            EGL14.eglMakeCurrent(eglDisplay, EGL14.EGL_NO_SURFACE, EGL14.EGL_NO_SURFACE, EGL14.EGL_NO_CONTEXT);
            if (eglSurface != EGL14.EGL_NO_SURFACE) {
                EGL14.eglDestroySurface(eglDisplay, eglSurface);
            }
            if (eglContext != EGL14.EGL_NO_CONTEXT) {
                EGL14.eglDestroyContext(eglDisplay, eglContext);
            }
            EGL14.eglTerminate(eglDisplay);
        }
        eglDisplay = EGL14.EGL_NO_DISPLAY;
        eglContext = EGL14.EGL_NO_CONTEXT;
        eglSurface = EGL14.EGL_NO_SURFACE;
        surface = null;
    }

    private EGLConfig chooseConfig() {
        // 定义我们需要的配置的属性
        int[] attributes = {
                EGL14.EGL_RENDERABLE_TYPE, EGL14.EGL_OPENGL_ES2_BIT, // 使用OpenGL ES 2.0
                EGL14.EGL_RED_SIZE, 8,
                EGL14.EGL_GREEN_SIZE, 8,
                EGL14.EGL_BLUE_SIZE, 8,
                EGL14.EGL_DEPTH_SIZE, 16, // 如果需要深度缓冲区
                EGL14.EGL_NONE // 总是以EGL_NONE结束属性列表
        };

        // 获取默认的显示设备
        EGLDisplay display = EGL14.eglGetDisplay(EGL14.EGL_DEFAULT_DISPLAY);
        if (display == EGL14.EGL_NO_DISPLAY) {
            throw new RuntimeException("Unable to get EGL14 display");
        }

        // 初始化EGL
        int[] version = new int[2];
        if (!EGL14.eglInitialize(display, version, 0, version, 1)) {
            throw new RuntimeException("Unable to initialize EGL14");
        }

        // 用于接收找到的配置数量
        int[] numConfig = new int[1];
        EGLConfig[] configs = new EGLConfig[1];
        // 获取最匹配的配置
        if (!EGL14.eglChooseConfig(display, attributes, 0, configs, 0, configs.length, numConfig, 0)) {
            throw new IllegalArgumentException("Failed to choose config");
        }

        // 终止EGL
        EGL14.eglTerminate(display);

        // 如果找到了合适的配置，返回它
        if (numConfig[0] > 0) {
            return configs[0];
        }

        // 没有找到合适的配置，返回null
        return null;
    }

    void eglInit()
    {
        // 1. 获取EGLDisplay对象
        eglDisplay = EGL14.eglGetDisplay(EGL14.EGL_DEFAULT_DISPLAY);
        if (eglDisplay == EGL14.EGL_NO_DISPLAY) {
            throw new RuntimeException("Unable to get EGL14 display");
        }

        // 2. 初始化与EGLDisplay之间的连接
        int[] version = new int[2];
        if (!EGL14.eglInitialize(eglDisplay, version, 0, version, 1)) {
            throw new RuntimeException("Unable to initialize EGL14");
        }

        // 3. 选择EGL配置
        EGLConfig eglConfig = chooseConfig(); // 实现略，需根据具体需求选择配置

        // 4. 创建EGL上下文
        int[] contextAttribs = {
                EGL14.EGL_CONTEXT_CLIENT_VERSION, 2,
                EGL14.EGL_NONE
        };
        eglContext = EGL14.eglCreateContext(eglDisplay, eglConfig, EGL14.EGL_NO_CONTEXT, contextAttribs, 0);
        if (eglContext == EGL14.EGL_NO_CONTEXT) {
            throw new RuntimeException("Failed to create EGL context");
        }

        // 5. 创建EGL表面
        int[] surfaceAttribs = {
                EGL14.EGL_NONE
        };
        eglSurface = EGL14.eglCreateWindowSurface(eglDisplay, eglConfig, surface, surfaceAttribs, 0);
        if (eglSurface == EGL14.EGL_NO_SURFACE) {
            throw new RuntimeException("Failed to create EGL surface");
        }

        // 6. 绑定EGL上下文和表面到当前线程
        if (!EGL14.eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext)) {
            throw new RuntimeException("Failed to bind context and surface");
        }
    }

    void manualEGLInit(Surface surface)
    {
        eglManagerPtr = createEGLManager(surface);
    }

    void renderInit()
    {
        int[] textureIds = new int[1];
        GLES20.glGenTextures(1, textureIds, 0);
        textureId = textureIds[0];

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId);

// 设置纹理参数
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);

// 为纹理分配空间
        GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGBA, 300, 300, 0, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, null);

        int[] fboIds = new int[1];
        GLES20.glGenFramebuffers(1, fboIds, 0);
        fboId = fboIds[0];

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, fboId);

// 将纹理附加到帧缓冲的颜色附件上
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0, GLES20.GL_TEXTURE_2D, textureId, 0);

// 检查FBO状态
        if (GLES20.glCheckFramebufferStatus(GLES20.GL_FRAMEBUFFER) != GLES20.GL_FRAMEBUFFER_COMPLETE) {
            // 错误处理
        }



















// 矩形顶点坐标（以逆时针顺序定义两个三角形）
        float[] vertices = {
                -1.0f, -1.0f,  // 左下角
                1.0f, -1.0f,  // 右下角
                -1.0f,  1.0f,  // 左上角
                1.0f,  1.0f,  // 右上角
        };

// 纹理坐标
        float[] texCoords = {
                0.0f, 1.0f,  // 左下角
                1.0f, 1.0f,  // 右下角
                0.0f, 0.0f,  // 左上角
                1.0f, 0.0f   // 右上角
        };

// 创建缓冲区
        vertexBuffer = ByteBuffer.allocateDirect(vertices.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(vertices);
        vertexBuffer.position(0);

        texCoordBuffer = ByteBuffer.allocateDirect(texCoords.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(texCoords);
        texCoordBuffer.position(0);


        String vertexShaderCode =
                "attribute vec4 vPosition;\n" +   // 顶点位置
                        "attribute vec2 vTexCoord;\n" +   // 纹理坐标
                        "varying vec2 texCoord;\n" +      // 用于传递给片段着色器的纹理坐标
                        "\n" +
                        "void main() {\n" +
                        "    texCoord = vTexCoord;\n" +
                        "    gl_Position = vPosition;\n" +
                        "}";

        String fragmentShaderCode =
                "precision mediump float;\n" +
                        "uniform sampler2D uTexture; // 纹理单元\n" +
                        "varying vec2 texCoord;      // 从顶点着色器接收的纹理坐标\n" +
                        "\n" +
                        "void main() {\n" +
                        "    gl_FragColor = texture2D(uTexture, texCoord);\n" +
                        "}";

        int shaderProgram = createShaderProgram(vertexShaderCode, fragmentShaderCode);


// 使用着色器程序
        GLES20.glUseProgram(shaderProgram);

// 获取属性位置
        positionHandle = GLES20.glGetAttribLocation(shaderProgram, "vPosition");
        texCoordHandle = GLES20.glGetAttribLocation(shaderProgram, "vTexCoord");
        textureHandle = GLES20.glGetUniformLocation(shaderProgram, "uTexture");

// 启用顶点属性




    }

    void onDrawFrame()
    {
        manualEGLDrawFrame();
    }

    void manualEGLDrawFrame()
    {
        drawFrame(eglManagerPtr);
    }

    void renderDrawFrame()
    {
        //        x += 0.05f;
//        if(x > 1.0)
//        {
//            x = 0.0f;
//        }
//        GLES20.glClearColor(0.0f, 0.0f, x, 1.0f);
//
//        // 清除颜色缓冲区
//        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        // 绑定帧缓冲，渲染到纹理
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, fboId);


        // 设置视口大小
        GLES20.glViewport(0, 0, 300, 300);

        GLES20.glClearColor(1.0f, 1.0f, 0.0f, 1.0f);

        // 清除屏幕
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);


        // 绑定回默认帧缓冲
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);


        GLES20.glClearColor(0.0f, 1.0f, 1.0f, 1.0f);

        // 清除屏幕
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);

        GLES20.glEnableVertexAttribArray(positionHandle);
        GLES20.glEnableVertexAttribArray(texCoordHandle);

        // 准备坐标数据
        GLES20.glVertexAttribPointer(positionHandle, 2, GLES20.GL_FLOAT, false, 0, vertexBuffer);
        GLES20.glVertexAttribPointer(texCoordHandle, 2, GLES20.GL_FLOAT, false, 0, texCoordBuffer);

// 绑定纹理
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId);
        GLES20.glUniform1i(textureHandle, 0);

// 绘制矩形
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);

// 禁用顶点属性
        GLES20.glDisableVertexAttribArray(positionHandle);
        GLES20.glDisableVertexAttribArray(texCoordHandle);

        Log.i("GLError", "" + GLES20.glGetError());
    }

    private int loadShader(int type, String shaderCode) {
        // 创建一个新的着色器对象
        final int shader = GLES20.glCreateShader(type);

        // 将源码添加到着色器并编译它
        GLES20.glShaderSource(shader, shaderCode);
        GLES20.glCompileShader(shader);

        // 检查编译是否成功
        final int[] compileStatus = new int[1];
        GLES20.glGetShaderiv(shader, GLES20.GL_COMPILE_STATUS, compileStatus, 0);
        if (compileStatus[0] == 0) {
            Log.e("Shader", "Error compiling shader: " + GLES20.glGetShaderInfoLog(shader));
            GLES20.glDeleteShader(shader);
            return 0;
        }

        return shader;
    }

    private int createShaderProgram(String vertexShaderCode, String fragmentShaderCode) {
        // 加载顶点着色器
        int vertexShader = loadShader(GLES20.GL_VERTEX_SHADER, vertexShaderCode);
        // 加载片段着色器
        int fragmentShader = loadShader(GLES20.GL_FRAGMENT_SHADER, fragmentShaderCode);

        // 创建一个新的程序
        final int program = GLES20.glCreateProgram();

        // 将顶点着色器附加到程序
        GLES20.glAttachShader(program, vertexShader);
        // 将片段着色器附加到程序
        GLES20.glAttachShader(program, fragmentShader);

        // 链接着色器程序
        GLES20.glLinkProgram(program);

        // 检查链接状态
        final int[] linkStatus = new int[1];
        GLES20.glGetProgramiv(program, GLES20.GL_LINK_STATUS, linkStatus, 0);
        if (linkStatus[0] == 0) {
            Log.e("Program", "Error linking program: " + GLES20.glGetProgramInfoLog(program));
            GLES20.glDeleteProgram(program);
            return 0;
        }

        return program;
    }


}
