# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.22.1)

# Declares and names the project.

project("RenderLib")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        RenderLib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        native-lib.cpp
        EGLManager.h
        EGLManager.cpp
        Example_Texture.h
        Example_Texture.cpp)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

set(ANDROID_ABI arm64-v8a)
set(LLGL_ROOT E:/LibCore/vlDependencyRes/LLGL)
set(LLGL_INCLUDE_DIR "${LLGL_ROOT}/include")
set(LLGL_LIB_DIR "${LLGL_ROOT}/build_android/${ANDROID_ABI}/build")

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

find_library(ANDROID_LIB android)
find_library(egl-lib EGL)
find_library(gles-lib GLESv3)

add_library(lib_llgl SHARED IMPORTED)
set_target_properties(lib_llgl PROPERTIES IMPORTED_LOCATION "${LLGL_LIB_DIR}/libLLGLD.a")

add_library(lib_llgl_gles3 SHARED IMPORTED)
set_target_properties(lib_llgl_gles3 PROPERTIES IMPORTED_LOCATION "${LLGL_LIB_DIR}/libLLGL_OpenGLES3D.a")

add_library(lib_ExampleBase STATIC IMPORTED)
set_target_properties(lib_ExampleBase PROPERTIES IMPORTED_LOCATION "${LLGL_LIB_DIR}/libExampleBaseD.a")



target_include_directories(
        RenderLib

        PRIVATE

        ${LLGL_INCLUDE_DIR}
        ${LLGL_ROOT}/external/GaussianLib/include
        ${LLGL_ROOT}/external
        ${LLGL_ROOT}/examples/Cpp/ExampleBase
        ${ANDROID_NDK}/sources/android/native_app_glue
)

MESSAGE(STATUS "ExampleBase=${LLGL_ROOT}/examples/Cpp/ExampleBase")

target_link_libraries(
        RenderLib

        ${ANDROID_LIB}
        ${egl-lib}
        ${gles-lib}
        ${log-lib}

        lib_llgl
        lib_llgl_gles3
        lib_ExampleBase
)