// EGLManager.h
#ifndef EGLMANAGER_H
#define EGLMANAGER_H

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android/native_window.h>

#include "Example_Texture.h"

class EGLManager {
public:
    EGLManager();
    ~EGLManager();

    bool initialize(ANativeWindow* window);
    void drawFrame();
    void destroy();

    EGLInfo GetGLInfo();

private:
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    ANativeWindow* nativeWindow;

    bool isInitialized;

    Sp_Example_Texturing exampleTexturing;

};

#endif // EGLMANAGER_H
