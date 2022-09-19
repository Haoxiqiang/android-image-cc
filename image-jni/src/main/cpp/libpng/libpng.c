#include <jni.h>
#include <png.h>
#include <stdio.h>
#include <android/log.h>
#include <malloc.h>

#define LOG_TAG "LibPNG"
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG,__VA_ARGS__)
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define PNG_BYTES_TO_CHECK 8

int check_if_png(FILE *fp) {
    png_byte header[PNG_BYTES_TO_CHECK];
    if (fread(header, 1, PNG_BYTES_TO_CHECK, fp) != PNG_BYTES_TO_CHECK) {
        return 0;
    }
    int is_png = png_sig_cmp(header, 0, PNG_BYTES_TO_CHECK);
    if (is_png != 0) {
        return 0;
    }
    return 1;
}

JNIEXPORT jboolean JNICALL
Java_com_thefrodo_image_png_LibPng_isPNGImage(JNIEnv *env, jobject thiz, jstring path) {

    const char *input = (*env)->GetStringUTFChars(env, path, JNI_FALSE);
    /* source file */
    FILE *infile;
    if ((infile = fopen(input, "rb")) == NULL) {
        (*env)->ReleaseStringChars(env, path,
                                   (const jchar *) (const jchar *) (const jchar *) input);
        LOGE("can't open %s", input);
        return JNI_FALSE;
    }

    int is_png = check_if_png(infile);

    fclose(infile);
    (*env)->ReleaseStringChars(env, path, (const jchar *) input);

    return is_png == 1 ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_com_thefrodo_image_png_LibPng_pngInfo(JNIEnv *env, jobject thiz, jstring path) {

    const char *input = (*env)->GetStringUTFChars(env, path, JNI_FALSE);
    /* source file */
    FILE *infile;
    if ((infile = fopen(input, "rb")) == NULL) {
        (*env)->ReleaseStringChars(env, path, (const jchar *) input);
        LOGE("can't open %s", input);
        return;
    }

    // 检查文件是否 png 格式
    int is_png = check_if_png(infile);
    if (!is_png) {
        LOGE("it not a png file %s", input);
        fclose(infile);
        (*env)->ReleaseStringChars(env, path, (const jchar *) input);
        return;
    }

    //set up initial png structs
    png_structp png_struct_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp) NULL,
                                                        NULL,
                                                        NULL);
    if (!png_struct_ptr) {
        LOGE("PNG init error %p", png_struct_ptr);
        fclose(infile);
        (*env)->ReleaseStringChars(env, path, (const jchar *) input);
        return;
    }

    png_infop info_ptr = png_create_info_struct(png_struct_ptr);
    if (!info_ptr) {

        png_destroy_read_struct(&png_struct_ptr, NULL, NULL);

        LOGE("PNG init error %p", info_ptr);
        fclose(infile);
        (*env)->ReleaseStringChars(env, path, (const jchar *) input);
        return;
    }

    png_init_io(png_struct_ptr, infile);

    // 如果不执行这句就复位文件指针，否则会出现黑屏
    png_set_sig_bytes(png_struct_ptr, PNG_BYTES_TO_CHECK);

    png_read_png(png_struct_ptr, info_ptr,
                 (PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND),
                 NULL);

    const size_t w = png_get_image_width(png_struct_ptr, info_ptr);
    const size_t h = png_get_image_height(png_struct_ptr, info_ptr);
    const size_t pitch = png_get_rowbytes(png_struct_ptr, info_ptr);

    LOGD("%s w:%d h:%d pitch:%d ", input, w, h, pitch);

    png_destroy_read_struct(&png_struct_ptr, &info_ptr, NULL);

    fclose(infile);
    (*env)->ReleaseStringChars(env, path, (const jchar *) input);
}
