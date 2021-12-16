#include <android/bitmap.h>
#include <malloc.h>
#include "frdjpeg.h"

int
jpeg_compress_file(JNIEnv *env, jstring src, jstring dst, jint quality, jboolean fancy_upsampling) {
    int ret = 0;

    struct FRD_IMAGE frd_image;

    frd_image.src = src;
    frd_image.dst = dst;

    frd_image.in_w = 0;
    frd_image.in_h = 0;
    frd_image.out_w = 0;
    frd_image.out_h = 0;
    frd_image.fancy_upsampling = fancy_upsampling ? TRUE : FALSE;

    jboolean isCopy = JNI_FALSE;
    frd_image.input = (*env)->GetStringUTFChars(env, src, &isCopy);
    frd_image.output = (*env)->GetStringUTFChars(env, dst, &isCopy);

    LOGI("\ntry compress\n\t%s\nto\n\t%s", frd_image.input, frd_image.output);

    ret = read_JPEG_file(&frd_image);

    LOGD("frd image size in_w:%d in_h:%d out_w:%d out_h:%d", frd_image.in_w,
         frd_image.in_h, frd_image.out_w, frd_image.out_h);

    if (ret < 0) {
        free_frd_image(env, &frd_image);
        return ret;
    }

    LOGD("buffer size %zu", allocated_size(&frd_image));

    ret = write_JPEG_file(&frd_image, quality);

    free_frd_image(env, &frd_image);

    return ret;
}

// TODO remove with the write_JPEG_file
int generateJpg(BYTE *data, int width, int height, int quality, const char *outfile) {
    int component = 3;

    struct jpeg_compress_struct jcs;
    struct jpeg_error_mgr jem;

    jcs.err = jpeg_std_error(&jem);

    jpeg_create_compress(&jcs);

    FILE *file = fopen(outfile, "wb");
    if (file == NULL) {
        return 0;
    }

    jpeg_stdio_dest(&jcs, file);

    jcs.image_width = (JDIMENSION) width;
    jcs.image_height = (JDIMENSION) height;
    jcs.arith_code = FALSE;
    jcs.input_components = component;
    jcs.in_color_space = JCS_RGB;
    jcs.optimize_coding = TRUE;

    jpeg_set_defaults(&jcs);
    jpeg_set_quality(&jcs, quality, TRUE);
    jpeg_start_compress(&jcs, TRUE);

    JSAMPROW rowPointer[1];
    int rowStride = jcs.image_width * component;

    while (jcs.next_scanline < jcs.image_height) {
        rowPointer[0] = &data[jcs.next_scanline * rowStride];
        jpeg_write_scanlines(&jcs, rowPointer, 1);
    }

    jpeg_finish_compress(&jcs);
    jpeg_destroy_compress(&jcs);
    fclose(file);

    return JNI_TRUE;
}

// TODO change data cache
int jpeg_compress_bitmap(JNIEnv *env, jobject bitmap, jstring dst, jint quality) {

    jboolean isCopy = JNI_FALSE;
    const char *output = (*env)->GetStringUTFChars(env, dst, &isCopy);

    LOGI("\ntry compress bitmap to\n\t%s", output);

    AndroidBitmapInfo bitmap_info;

    if ((AndroidBitmap_getInfo(env, bitmap, &bitmap_info)) < 0) {
        LOGE("compress error");
        (*env)->ReleaseStringChars(env, dst, (const jchar *) output);
        return false;
    }

    int in_w = bitmap_info.width;
    int in_h = bitmap_info.height;
    int components = 3;

    BYTE *pixelColor;

    if ((AndroidBitmap_lockPixels(env, bitmap, (void **) &pixelColor)) < 0) {
        LOGE("lock pixels error");
        (*env)->ReleaseStringChars(env, dst, (const jchar *) output);
        return false;
    }

    BYTE r, g, b;
    int color;
    BYTE *data;
    BYTE *tempData;
    data = malloc((size_t) (in_w * in_h * components));
    tempData = data;

    for (int i = 0; i < in_h; i++) {
        for (int j = 0; j < in_w; j++) {
            color = *((int *) pixelColor);
            r = (BYTE) ((color & 0x00FF0000) >> 16);
            g = (BYTE) ((color & 0x0000FF00) >> 8);
            b = (BYTE) (color & 0X000000FF);

            *data = b;
            *(data + 1) = g;
            *(data + 2) = r;
            data += 3;
            pixelColor += 4;
        }
    }

    AndroidBitmap_unlockPixels(env, bitmap);

    int result = generateJpg(tempData, in_w, in_h, quality, output);
    LOGI("compress complete, result code is %d", result);
    (*env)->ReleaseStringChars(env, dst, (const jchar *) output);
    free(tempData);
    return result;
}
