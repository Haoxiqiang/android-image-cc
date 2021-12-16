# AndroidLibJpeg
A library built with the CMake is used for jpeg file decompress/compress on the Android platform.

## WHY

* fix Bitmap.compress(Bitmap.CompressFormat.JPEG, quality, outputStream); [Use libjpeg-turbo for YUV->RGB conversion in jpeg encoder](https://github.com/google/skia/commit/c7d01d3e1d3621907c27b283fb7f8b6e177c629d)
* easy customization
* I want to get to know the diff. image's library.

## Feature

* support libjpeg-turbo built by cmake on Android platform.
* support jpeg compress.

## Building
### Requirements
- Android NDK r22
- Android SDK 
- Android SDK Build tools 31.x
- CMake 3.10.2

you can change the ndk version by build.gradle#ndkVersion

## Build Problems

* "libclang_rt.asan-arm-android.so" not found.  CauseBy: -fsanitize=address, if midSdk < 21, remove the cflags or upgrade minSdk at least 21.

## Roadmap

- [x] add libjpeg built
- [ ] add libwebp built