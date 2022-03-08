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

project(heifjni LANGUAGES CXX C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -fstrict-aliasing -Wno-format")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fstrict-aliasing -Wno-format")
add_definitions("-DANDROID_STL=c++_shared -DNO_TESTS=true")

set(SOURCE_PATH ../../../../../external/heif/srcs/api-java/cpp)

add_subdirectory("${SOURCE_PATH}/../../" ${PROJECT_BINARY_DIR}/heif)

set(sources
        ${SOURCE_PATH}/CleanAperturePropertyJNI.cpp
        ${SOURCE_PATH}/HEIFJNI.cpp
        ${SOURCE_PATH}/Helpers.cpp
        ${SOURCE_PATH}/Helpers.h
        ${SOURCE_PATH}/ImageItemJNI.cpp
        ${SOURCE_PATH}/ItemJNI.cpp
        ${SOURCE_PATH}/ItemPropertyJNI.cpp
        ${SOURCE_PATH}/GridImageItemJNI.cpp
        ${SOURCE_PATH}/CodedImageItemJNI.cpp
        ${SOURCE_PATH}/DerivedImageItemJNI.cpp
        ${SOURCE_PATH}/DecoderConfigJNI.cpp
        ${SOURCE_PATH}/HEVCImageItemJNI.cpp
        ${SOURCE_PATH}/RawPropertyJNI.cpp
        ${SOURCE_PATH}/RotatePropertyJNI.cpp
        ${SOURCE_PATH}/PixelInformationPropertyJNI.cpp
        ${SOURCE_PATH}/PixelAspectRatioJNI.cpp
        ${SOURCE_PATH}/RelativeLocationPropertyJNI.cpp
        ${SOURCE_PATH}/OverlayImageItemJNI.cpp
        ${SOURCE_PATH}/AuxiliaryPropertyJNI.cpp
        ${SOURCE_PATH}/MirrorPropertyJNI.cpp
        ${SOURCE_PATH}/ColourInformationPropertyJNI.cpp
        ${SOURCE_PATH}/IdentityImageItemJNI.cpp
        ${SOURCE_PATH}/AVCDecoderConfigJNI.cpp
        ${SOURCE_PATH}/AVCImageItemJNI.cpp
        ${SOURCE_PATH}/HEVCDecoderConfigJNI.cpp
        ${SOURCE_PATH}/JPEGDecoderConfigJNI.cpp
        ${SOURCE_PATH}/JPEGImageItemJNI.cpp
        ${SOURCE_PATH}/MPEG7ItemJNI.cpp
        ${SOURCE_PATH}/XMPItemJNI.cpp
        ${SOURCE_PATH}/MimeItemJNI.cpp
        ${SOURCE_PATH}/ExifItemJNI.cpp
        ${SOURCE_PATH}/InputStream.cpp
        ${SOURCE_PATH}/OutputStream.cpp
        ${SOURCE_PATH}/AudioTrackJNI.cpp
        ${SOURCE_PATH}/VideoTrackJNI.cpp
        ${SOURCE_PATH}/TrackJNI.cpp
        ${SOURCE_PATH}/SampleJNI.cpp
        ${SOURCE_PATH}/HEVCSampleJNI.cpp
        ${SOURCE_PATH}/AACDecoderConfigJNI.cpp
        ${SOURCE_PATH}/AACSampleJNI.cpp
        ${SOURCE_PATH}/AVCSampleJNI.cpp
        ${SOURCE_PATH}/AlternativeTrackGroupJNI.cpp
        ${SOURCE_PATH}/EntityGroupJNI.cpp
        ${SOURCE_PATH}/EquivalenceGroupJNI.cpp
        ${SOURCE_PATH}/AlternateGroupJNI.cpp
        ${SOURCE_PATH}/StereoPairGroupJNI.cpp
        ${SOURCE_PATH}/ImageSequenceJNI.cpp
        ${SOURCE_PATH}/VideoSampleJNI.cpp
        )


add_library(heifjni SHARED ${sources})
target_link_libraries(heifjni heifpp)


