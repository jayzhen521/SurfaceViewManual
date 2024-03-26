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
        IcebergRender.h
        IcebergRender.cpp)


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

add_library(lib_llgl STATIC IMPORTED)
set_target_properties(lib_llgl PROPERTIES IMPORTED_LOCATION "${LLGL_LIB_DIR}/libLLGLD.a")

add_library(lib_llgl_gles3 STATIC IMPORTED)
set_target_properties(lib_llgl_gles3 PROPERTIES IMPORTED_LOCATION "${LLGL_LIB_DIR}/libLLGL_OpenGLES3D.a")

#add_library(lib_ExampleBase STATIC IMPORTED)
#set_target_properties(lib_ExampleBase PROPERTIES IMPORTED_LOCATION "${LLGL_LIB_DIR}/libExampleBase.a")


#vlCore-----------------------------------------------------------------------------------------
set(VLCORE_ROOT E:/LibCore/vlCore)
set(VLCORE_LIB_DIR "${VLCORE_ROOT}/build_android/${ANDROID_ABI}/build")

set(VLCORE_BASIC_MODULES UniUtil Util Comp Layer Material Rendering)
set(VLCORE_DRIVE_MODULES MgrResource Plugin ProcessA ProcessV Timeline)
set(VLCORE_LOGIC_MODULES Editor Exporter Player Project Toolbox)
set(VLCORE_ENGINEAPI_MODULES EngineAPI)
set(VLCORE_BUSINESSAPI_MODULES BAPIHelper BComp BExporter BLayer BMaterial BPlayer BProject)

foreach(libName IN LISTS VLCORE_BASIC_MODULES)
        add_library(${libName} STATIC IMPORTED)
        set_target_properties(${libName} PROPERTIES IMPORTED_LOCATION "${VLCORE_LIB_DIR}/lib${libName}.a")
        target_link_libraries(RenderLib PRIVATE ${libName})
endforeach()

foreach(libName IN LISTS VLCORE_DRIVE_MODULES)
        add_library(${libName} STATIC IMPORTED)
        set_target_properties(${libName} PROPERTIES IMPORTED_LOCATION "${VLCORE_LIB_DIR}/lib${libName}.a")
        target_link_libraries(RenderLib PRIVATE ${libName})
endforeach()

foreach(libName IN LISTS VLCORE_LOGIC_MODULES)
        add_library(${libName} STATIC IMPORTED)
        set_target_properties(${libName} PROPERTIES IMPORTED_LOCATION "${VLCORE_LIB_DIR}/lib${libName}.a")
        MESSAGE(STATUS "LOGICMODULE-${VLCORE_LIB_DIR}/lib${libName}.so")
        target_link_libraries(RenderLib PRIVATE ${libName})
endforeach()

foreach(libName IN LISTS VLCORE_ENGINEAPI_MODULES)
        add_library(${libName} STATIC IMPORTED)
        set_target_properties(${libName} PROPERTIES IMPORTED_LOCATION "${VLCORE_LIB_DIR}/lib${libName}.a")
        target_link_libraries(RenderLib PRIVATE ${libName})
endforeach()

foreach(libName IN LISTS VLCORE_BUSINESSAPI_MODULES)
        add_library(${libName} STATIC IMPORTED)
        set_target_properties(${libName} PROPERTIES IMPORTED_LOCATION "${VLCORE_LIB_DIR}/lib${libName}.a")
        target_link_libraries(RenderLib PRIVATE ${libName})
endforeach()





foreach(projName IN LISTS VLCORE_BASIC_MODULES)
        target_include_directories(RenderLib PRIVATE ${VLCORE_ROOT}/Basic/${projName}/include)
endforeach()

foreach(projName IN LISTS VLCORE_ENGINEAPI_MODULES)
        target_include_directories(RenderLib PRIVATE ${VLCORE_ROOT}/EngineAPI/${projName}/include)
endforeach()

foreach(projName IN LISTS VLCORE_BUSINESSAPI_MODULES)
        target_include_directories(RenderLib PRIVATE ${VLCORE_ROOT}/BusinessAPI/${projName}/include)
endforeach()

#thirdparty-------------------------------------------------------------------------------------
set(VLCORE_THIRDPARTY_ROOT E:/LibCore/vlDependencyRes)

set(VLCORE_THIRDPARTY_MODULES glog)

foreach(libName IN LISTS VLCORE_THIRDPARTY_MODULES)
        add_library(${libName} SHARED IMPORTED)
        set_target_properties(${libName} PROPERTIES IMPORTED_LOCATION "${VLCORE_THIRDPARTY_ROOT}/${libName}/build_android/${ANDROID_ABI}/lib${libName}.so")
        target_link_libraries(RenderLib PRIVATE ${libName})
        target_include_directories(RenderLib PRIVATE "${VLCORE_THIRDPARTY_ROOT}/${libName}/build_android/include")
endforeach()

add_library(rttr_core SHARED IMPORTED)
set_target_properties(rttr_core PROPERTIES IMPORTED_LOCATION "${VLCORE_THIRDPARTY_ROOT}/rttr/build_android/${ANDROID_ABI}/lib/librttr_core.so")
target_link_libraries(RenderLib PRIVATE rttr_core)
target_include_directories(RenderLib PRIVATE "${VLCORE_THIRDPARTY_ROOT}/rttr/build_android/include")

target_include_directories(RenderLib PRIVATE "${VLCORE_THIRDPARTY_ROOT}/rapidjson-1.1.0/include")
target_include_directories(RenderLib PRIVATE "${VLCORE_THIRDPARTY_ROOT}/glm")



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
        PRIVATE
        ${ANDROID_LIB}
        ${egl-lib}
        ${gles-lib}
        ${log-lib}

        lib_llgl
        lib_llgl_gles3
)