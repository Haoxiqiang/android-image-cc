# Android Image Decompress/Compress And Convert/Transform

A library built with the CMake is used for image file decompress/compress or conversion on the Android platform.

## WHY

* fix Bitmap.compress(Bitmap.CompressFormat.JPEG, quality, outputStream)
  ; [Use libjpeg-turbo for YUV->RGB conversion in jpeg encoder](https://github.com/google/skia/commit/c7d01d3e1d3621907c27b283fb7f8b6e177c629d)
* easy customization
* compile common libraries for android.

## Feature

* zlib v1.2.11
* libjpeg-turbo v2.1.2
* libpng v1.6.37
* libwebp v1.2.2
* support libjpeg-turbo built by cmake on Android platform.
* support jpeg compress.
* support heif v3.7.0

## Building

* build zlib v1.2.11, all ndk versions contains zlib that v1.2.3, it will cause many image libs
  which use zlib failed.

### Requirements

- Android NDK r22
- Android SDK
- Android SDK Build tools 31.x
- CMake 3.10.2

you can change the ndk version by build.gradle#ndkVersion

## Build Problems

* "libclang_rt.asan-arm-android.so" not found. CauseBy: -fsanitize=address, if midSdk < 21, remove
  the cflags or upgrade minSdk at least 21.

* libpng with zlib build failed.

```c
#if PNG_ZLIB_VERNUM != 0 && PNG_ZLIB_VERNUM != ZLIB_VERNUM
#  error ZLIB_VERNUM != PNG_ZLIB_VERNUM \
      "-I (include path) error: see the notes in pngpriv.h"
```

check `zlib.h` and `pngpriv.h` ZLIB_VERNUM == PNG_ZLIB_VERNUM

## Roadmap

- [x] add libjpeg built
- [x] add libpng built
- [ ] add libwebp built
- [x] add heif built
