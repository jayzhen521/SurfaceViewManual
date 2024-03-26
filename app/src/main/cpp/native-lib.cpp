#include <jni.h>
#include <string>

#include <android/native_window_jni.h>
#include <android/log.h>

#include "EGLManager.h"

extern "C" {
JNIEXPORT jlong JNICALL
Java_com_am_surfaceview_1manual_EGLHelper_createEGLManager(JNIEnv* env, jobject /* this */, jobject surface) {
    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
    EGLManager* eglManager = new EGLManager();
    if (eglManager->initialize(window)) {
        return reinterpret_cast<jlong>(eglManager);
    } else {
        delete eglManager;
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_com_am_surfaceview_1manual_EGLHelper_drawFrame(JNIEnv* env, jobject /* this */, jlong eglManagerPtr) {
    EGLManager* eglManager = reinterpret_cast<EGLManager*>(eglManagerPtr);
    if (eglManager != nullptr) {
        eglManager->drawFrame();
    }
}

JNIEXPORT void JNICALL
Java_com_am_surfaceview_1manual_EGLHelper_destroyEGLManager(JNIEnv* env, jobject /* this */, jlong eglManagerPtr) {
    EGLManager* eglManager = reinterpret_cast<EGLManager*>(eglManagerPtr);
    if (eglManager != nullptr) {
        eglManager->destroy();
        delete eglManager;
    }
}

}