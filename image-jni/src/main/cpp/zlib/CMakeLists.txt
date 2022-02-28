cmake_minimum_required(VERSION 3.10.2)
project(zlib C)

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

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -fstrict-aliasing -Wimplicit-function-declaration")

set(SOURCE_PATH ../../../../../external/zlib)

add_library(zlib STATIC
        # Common sources
        ${SOURCE_PATH}/adler32.c
        ${SOURCE_PATH}/compress.c
        ${SOURCE_PATH}/crc32.c
        ${SOURCE_PATH}/deflate.c
        ${SOURCE_PATH}/gzclose.c
        ${SOURCE_PATH}/gzlib.c
        ${SOURCE_PATH}/gzread.c
        ${SOURCE_PATH}/gzwrite.c
        ${SOURCE_PATH}/infback.c
        ${SOURCE_PATH}/inffast.c
        ${SOURCE_PATH}/inflate.c
        ${SOURCE_PATH}/inftrees.c
        ${SOURCE_PATH}/trees.c
        ${SOURCE_PATH}/uncompr.c
        ${SOURCE_PATH}/zutil.c
        )

target_link_libraries(zlib log cpufeatures)

configure_file(${SOURCE_PATH}/zconf.h ${CMAKE_CURRENT_BINARY_DIR}/include/zconf.h COPYONLY)
configure_file(${SOURCE_PATH}/zlib.h ${CMAKE_CURRENT_BINARY_DIR}/include/zlib.h COPYONLY)

set(ZLIB_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include/)

target_include_directories(zlib PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}/include/
        ${ANDROID_NDK}/sources/android/cpufeatures)

target_compile_definitions(zlib PUBLIC
        ADLER32_SIMD_SSSE3
        CRC32_SIMD_SSE42_PCLMUL
        INFLATE_CHUNK_READ_64LE
        X86_WINDOWS
        ZLIB_CONST
        )

