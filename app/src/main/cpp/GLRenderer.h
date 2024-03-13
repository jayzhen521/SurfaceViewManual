// GLRenderer.h
#ifndef GLRENDERER_H
#define GLRENDERER_H

#include <GLES2/gl2.h>

class GLRenderer {
public:
    GLRenderer();
    ~GLRenderer();

    void initialize();
    void drawFrame();
    void destroy();
    GLuint compileShader(GLenum type, const char* source);
    GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource);

private:
    GLuint program;
    void compileShaders();
};

#endif // GLRENDERER_H
