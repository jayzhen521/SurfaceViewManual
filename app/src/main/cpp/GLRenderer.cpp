// GLRenderer.cpp
#include "GLRenderer.h"

#include <android/log.h>

void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        __android_log_print(ANDROID_LOG_ERROR, "GLRenderer", "after %s() glError (0x%x)\n", op, error);
    }
}

const char* vertexShaderSource = R"glsl(
    attribute vec4 vPosition;
    void main() {
        gl_Position = vPosition;
    }
)glsl";

const char* fragmentShaderSource = R"glsl(
    precision mediump float;
    void main() {
        gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); // 红色
    }
)glsl";

const GLfloat vertices[] = {
        0.0f, 0.5f, 0.0f, // 顶点
        -0.5f,-0.5f, 0.0f, // 左下
        0.5f,-0.5f, 0.0f  // 右下
};

GLRenderer::GLRenderer() : program(0) {}

GLRenderer::~GLRenderer() {
    destroy();
}

void GLRenderer::initialize() {
    compileShaders();
}

void GLRenderer::compileShaders() {
    // 这里添加编译和链接着色器的代码，参照前面的示例
    // 确保设置program成员变量
    program = createShaderProgram(vertexShaderSource, fragmentShaderSource);
}

void GLRenderer::drawFrame() {
    if (program == 0) return;

    glUseProgram(program);
    checkGlError("glUseProgram");

    GLuint positionAttribute = glGetAttribLocation(program, "vPosition");
    checkGlError("glGetAttribLocation");

    glEnableVertexAttribArray(positionAttribute);
    checkGlError("glEnableVertexAttribArray");

    glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    checkGlError("glVertexAttribPointer");

    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");

    glDrawArrays(GL_TRIANGLES, 0, 3);
    checkGlError("glDrawArrays");
}

void GLRenderer::destroy() {
    if (program != 0) {
        glDeleteProgram(program);
        program = 0;
    }
}

GLuint GLRenderer::compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    return shader;
}

GLuint GLRenderer::createShaderProgram(const char* vertexSource, const char* fragmentSource) {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

