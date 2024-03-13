// EGLManager.h
#ifndef EGLMANAGER_H
#define EGLMANAGER_H

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android/native_window.h>
#include "GLRenderer.h"

class EGLManager {
public:
    EGLManager();
    ~EGLManager();

    bool initialize(ANativeWindow* window);
    void drawFrame();
    void destroy();

private:
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    ANativeWindow* nativeWindow;

    bool isInitialized;

    GLRenderer renderer;
};

#endif // EGLMANAGER_H
