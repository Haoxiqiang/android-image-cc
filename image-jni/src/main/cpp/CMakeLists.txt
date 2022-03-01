cmake_minimum_required(VERSION 3.10.2)
project(image-jni)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_VERBOSE_MAKEFILE ON)

if (CMAKE_ANDROID_ARCH_ABI STREQUAL armeabi-v7a)
    set(ANDROID_LLVM_TRIPLE armv7-none-linux-androideabi)
elseif (CMAKE_ANDROID_ARCH_ABI STREQUAL arm64-v8a)
    set(ANDROID_LLVM_TRIPLE aarch64-none-linux-android)
elseif (CMAKE_ANDROID_ARCH_ABI STREQUAL x86)
    set(ANDROID_LLVM_TRIPLE i686-none-linux-android)
elseif (CMAKE_ANDROID_ARCH_ABI STREQUAL x86_64)
    set(ANDROID_LLVM_TRIPLE x86_64-none-linux-android)
else ()
    message(FATAL_ERROR "Invalid Android ABI: ${ANDROID_ABI}.")
endif ()

message(WARNING "ANDROID_NDK :${ANDROID_NDK}")

# cpufeatures
include_directories(${ANDROID_NDK}/sources/android/cpufeatures)
add_library(cpufeatures STATIC
        ${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c)

# zlib
add_subdirectory("zlib" zlib)
add_subdirectory("libpng" libpng)
add_subdirectory("libjpeg" libjpeg)

#add_subdirectory("libwebp" webp)


add_library(frdimage
        SHARED
        libjpeg/libjpeg-jni.c
        libjpeg/frdjpeg.c
        libjpeg/frdjpegreader.c
        libjpeg/frdjpegwriter.c
        libjpeg/frdtools.c
        libpng/libpng.c
        )

target_include_directories(frdimage PUBLIC
        libjpeg/frdjpeg.h
        ${SOURCE_PATH}
        ${JPEG_PATH})

find_library(
        log-lib
        log
)

target_link_libraries(frdimage
        libjpeg
        png
        jnigraphics
        ${log-lib})

set_target_properties(frdimage PROPERTIES
        PREFIX "lib"
        SUFFIX ".so")
