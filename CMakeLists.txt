cmake_minimum_required(VERSION 3.10.0)
project(imencv VERSION 0.1.0 LANGUAGES C CXX)

# on
set(BUILD_LIST "videoio" CACHE STRING "" FORCE)
set(BUILD_ZLIB          ON CACHE BOOL "" FORCE)
set(OPENCV_GAPI_MSMF    ON CACHE BOOL "" FORCE)
set(WITH_MSMF           ON CACHE BOOL "" FORCE)
set(WITH_MSMF_DXVA      ON CACHE BOOL "" FORCE)
set(WITH_DSHOW          ON CACHE BOOL "" FORCE)

# off
set(BUILD_SHARED_LIBS                       OFF CACHE BOOL "" FORCE)
set(BUILD_PERF_TESTS                        OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS                             OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_python_tests               OFF CACHE BOOL "" FORCE)
set(OPENCV_TEST_DNN_TFLITE                  OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_apps                       OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_python_bindings_generator  OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_python_tests               OFF CACHE BOOL "" FORCE)
set(BUILD_JAVA                              OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_java                       OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_java_bindings_generator    OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_js_bindings_generator      OFF CACHE BOOL "" FORCE)
set(WITH_VTK                                OFF CACHE BOOL "" FORCE)
set(WITH_GSTREAMER                          OFF CACHE BOOL "" FORCE)
set(BUILD_WEBP                              OFF CACHE BOOL "" FORCE)
set(WITH_WEBP                               OFF CACHE BOOL "" FORCE)
set(WITH_OPENEXR                            OFF CACHE BOOL "" FORCE)
set(BUILD_OPENJPEG                          OFF CACHE BOOL "" FORCE)
set(WITH_OPENJPEG                           OFF CACHE BOOL "" FORCE)
set(BUILD_TIFF                              OFF CACHE BOOL "" FORCE)
set(WITH_TIFF                               OFF CACHE BOOL "" FORCE)
set(BUILD_PNG                               OFF CACHE BOOL "" FORCE)
set(PNG_HARDWARE_OPTIMIZATIONS              OFF CACHE BOOL "" FORCE)
set(WITH_PNG                                OFF CACHE BOOL "" FORCE)
set(BUILD_JASPER                            OFF CACHE BOOL "" FORCE)
set(WITH_JASPER                             OFF CACHE BOOL "" FORCE)
set(BUILD_IPP_IW                            OFF CACHE BOOL "" FORCE)
set(WITH_IPP                                OFF CACHE BOOL "" FORCE)
set(BUILD_ITT                               OFF CACHE BOOL "" FORCE)
set(WITH_ITT                                OFF CACHE BOOL "" FORCE)
set(ccitt                                   OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_highgui                    OFF CACHE BOOL "" FORCE)
set(HIGHGUI_ENABLE_PLUGINS                  OFF CACHE BOOL "" FORCE)
set(OPENCV_FFMPEG_ENABLE_LIBAVDEVICE        OFF CACHE BOOL "" FORCE)
set(WITH_FFMPEG                             OFF CACHE BOOL "" FORCE)
set(USE_WIN32_FILEIO                        OFF CACHE BOOL "" FORCE)
set(WITH_WIN32UI                            OFF CACHE BOOL "" FORCE)
set(WITH_QT                                 OFF CACHE BOOL "" FORCE)
set(WITH_OPENGL                             OFF CACHE BOOL "" FORCE)

add_subdirectory(vendor/opencv)

add_executable(imencv main.cpp)

target_include_directories(imencv PRIVATE 
    vendor/opencv/include
    vendor/opencv/modules/videoio/include
    vendor/opencv/modules/imgcodecs/include
    vendor/opencv/modules/imgproc/include
    vendor/opencv/modules/core/include
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(imencv PRIVATE 
    opencv_videoio 
    opencv_imgcodecs
    opencv_imgproc 
    opencv_core
)
