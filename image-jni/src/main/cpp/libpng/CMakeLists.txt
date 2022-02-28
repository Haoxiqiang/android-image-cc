cmake_minimum_required(VERSION 3.10.2)
cmake_policy(VERSION 3.10.2)

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

project(libpng C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -fstrict-aliasing -Wno-format")

set(SOURCE_PATH ../../../../../external/libpng)
message(WARNING "SOURCE_PATH libpng:${SOURCE_PATH}")

set(PNG_LIB_NAME png)
set(PNGLIB_MAJOR 1)
set(PNGLIB_MINOR 6)
set(PNGLIB_RELEASE 37)
set(PNGLIB_NAME libpng${PNGLIB_MAJOR}${PNGLIB_MINOR})
set(PNGLIB_VERSION ${PNGLIB_MAJOR}.${PNGLIB_MINOR}.${PNGLIB_RELEASE})

# COMMAND LINE OPTIONS
option(PNG_HARDWARE_OPTIMIZATIONS "Enable hardware optimizations" ON)

message(WARNING "SOURCE_PATH: ${SOURCE_PATH}.")
message(WARNING "CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}.")

configure_file(${SOURCE_PATH}/scripts/pnglibconf.h.prebuilt ${CMAKE_CURRENT_BINARY_DIR}/include/pnglibconf.h COPYONLY)
set(PNG_CONFIG_HEADER ${CMAKE_CURRENT_BINARY_DIR}/include)
# set definitions and sources for arm
if (CMAKE_SYSTEM_PROCESSOR MATCHES "^arm" OR
        CMAKE_SYSTEM_PROCESSOR MATCHES "^aarch64")
    set(libpng_arm_sources
            ${SOURCE_PATH}/arm/arm_init.c
            ${SOURCE_PATH}/arm/filter_neon.S
            ${SOURCE_PATH}/arm/filter_neon_intrinsics.c
            ${SOURCE_PATH}/arm/palette_neon_intrinsics.c)

    add_definitions(-DPNG_ARM_NEON_OPT=2)
endif ()

set_property(SOURCE ${libpng_arm_sources}
        APPEND_STRING PROPERTY COMPILE_FLAGS " -mfpu=neon")

include_directories(
        ${SOURCE_PATH}
        ${PNG_CONFIG_HEADER}
)

# OUR SOURCES
set(libpng_public_hdrs
        ${PNG_CONFIG_HEADER}/pnglibconf.h
        ${SOURCE_PATH}/png.h
        ${SOURCE_PATH}/pngconf.h
        )
set(libpng_private_hdrs
        ${SOURCE_PATH}/pngpriv.h
        ${SOURCE_PATH}/pngdebug.h
        ${SOURCE_PATH}/pnginfo.h
        ${SOURCE_PATH}/pngstruct.h
        )

set(libpng_sources
        ${libpng_public_hdrs}
        ${libpng_private_hdrs}
        ${SOURCE_PATH}/png.c
        ${SOURCE_PATH}/pngerror.c
        ${SOURCE_PATH}/pngget.c
        ${SOURCE_PATH}/pngmem.c
        ${SOURCE_PATH}/pngpread.c
        ${SOURCE_PATH}/pngread.c
        ${SOURCE_PATH}/pngrio.c
        ${SOURCE_PATH}/pngrtran.c
        ${SOURCE_PATH}/pngrutil.c
        ${SOURCE_PATH}/pngset.c
        ${SOURCE_PATH}/pngtrans.c
        ${SOURCE_PATH}/pngwio.c
        ${SOURCE_PATH}/pngwrite.c
        ${SOURCE_PATH}/pngwtran.c
        ${SOURCE_PATH}/pngwutil.c
        ${libpng_arm_sources}
        )

find_library(
        zlib
        log-lib
        log)

message("build png shared library")

add_library(png STATIC ${libpng_sources})
#    set_target_properties(png PROPERTIES OUTPUT_NAME ${PNG_LIB_NAME})

target_link_libraries(png zlib log cpufeatures)

target_include_directories(png PUBLIC
        ${SOURCE_PATH}
        ${PNG_CONFIG_HEADER}
        ${ANDROID_NDK}/sources/android/cpufeatures)
