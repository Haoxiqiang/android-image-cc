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

#add_subdirectory("libjpeg" libjpeg-turbo)
#add_subdirectory("libwebp" webp)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -fstrict-aliasing")

set(SOURCE_PATH ../../../../external/libjpeg-turbo)
message(WARNING "SOURCE_PATH libjpeg-turbo:${SOURCE_PATH}")

option(ENABLE_SHARED "Build shared libraries" FALSE)
option(ENABLE_STATIC "Build static libraries" TRUE)
option(WITH_JAVA "Build Java wrapper for the TurboJPEG API library (implies ENABLE_SHARED=1)" FALSE)
option(WITH_TURBOJPEG "Include the TurboJPEG API library and associated test programs" FALSE)
option(WITH_SIMD "Include SIMD extensions, if available for this platform" TRUE)
option(WITH_MEM_SRCDST "Include in-memory source/destination manager functions when emulating the libjpeg v6b or v7 API/ABI" FALSE)
option(WITH_ARITH_DEC "Include arithmetic decoding support when emulating the libjpeg v6b API/ABI" FALSE)
option(WITH_ARITH_ENC "Include arithmetic encoding support when emulating the libjpeg v6b API/ABI" FALSE)

add_subdirectory(${SOURCE_PATH} libjpeg-turbo)

message(WARNING "JPEG prebuild dir:${CMAKE_CURRENT_BINARY_DIR}")
set(JPEG_PATH ${CMAKE_CURRENT_BINARY_DIR}/libjpeg-turbo)

add_library(frdjpeg SHARED IMPORTED)
set_target_properties(frdjpeg PROPERTIES IMPORTED_LOCATION ${JPEG_PATH}/libjpeg.a)

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
        frdjpeg
        png
        jnigraphics
        ${log-lib})

set_target_properties(frdimage PROPERTIES
        PREFIX "lib"
        SUFFIX ".so")
