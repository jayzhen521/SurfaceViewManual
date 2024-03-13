// EGLManager.cpp
#include "EGLManager.h"
#include <android/log.h>

EGLManager::EGLManager() :
        display(EGL_NO_DISPLAY),
        surface(EGL_NO_SURFACE),
        context(EGL_NO_CONTEXT),
        nativeWindow(nullptr),
        isInitialized(false) {
}

EGLManager::~EGLManager() {
    destroy();
}

bool EGLManager::initialize(ANativeWindow* window) {
    nativeWindow = window;


    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        __android_log_print(ANDROID_LOG_ERROR, "EGLManager", "Unable to obtain EGL display.");
        return false;
    }


    if (!eglInitialize(display, nullptr, nullptr)) {
        __android_log_print(ANDROID_LOG_ERROR, "EGLManager", "Unable to initialize EGL.");
        return false;
    }


    const EGLint attribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, // 请求OpenGL ES 2.0
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
    };


    EGLConfig config;
    EGLint numConfigs;
    if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs)) {
        __android_log_print(ANDROID_LOG_ERROR, "EGLManager", "Failed to choose config.");
        return false;
    }
    if (numConfigs <= 0) {
        __android_log_print(ANDROID_LOG_ERROR, "EGLManager", "No suitable config found.");
        return false;
    }


    surface = eglCreateWindowSurface(display, config, nativeWindow, nullptr);
    if (surface == EGL_NO_SURFACE) {
        __android_log_print(ANDROID_LOG_ERROR, "EGLManager", "Surface creation failed.");
        return false;
    }


    EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    if (context == EGL_NO_CONTEXT) {
        __android_log_print(ANDROID_LOG_ERROR, "EGLManager", "Context creation failed.");
        return false;
    }


    if (!eglMakeCurrent(display, surface, surface, context)) {
        __android_log_print(ANDROID_LOG_ERROR, "EGLManager", "eglMakeCurrent failed.");
        return false;
    }

    exampleTexturing = std::make_shared<Example_Texturing>(GetGLInfo());

    isInitialized = true;
    return true;
}

void EGLManager::drawFrame() {
    if (!isInitialized) return;

//    glClearColor(1.0, 0.0, 0.0, 1.0);
//    glClear(GL_COLOR_BUFFER_BIT);

    exampleTexturing->Run();

    eglSwapBuffers(display, surface); // 显示渲染结果
}

void EGLManager::destroy() {

    if (display != EGL_NO_DISPLAY) {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (context != EGL_NO_CONTEXT) {
            eglDestroyContext(display, context);
        }
        if (surface != EGL_NO_SURFACE) {
            eglDestroySurface(display, surface);
        }
        eglTerminate(display);
    }

    display = EGL_NO_DISPLAY;
    context = EGL_NO_CONTEXT;
    surface = EGL_NO_SURFACE;
    nativeWindow = nullptr;
    isInitialized = false;
}

EGLInfo EGLManager::GetGLInfo()
{
    EGLInfo info;
    info.display = display;
    info.context = context;
    info.surface = surface;
    info.nativeWindow = nativeWindow;
    info.width = ANativeWindow_getWidth(nativeWindow);
    info.height = ANativeWindow_getHeight(nativeWindow);

    return info;
}