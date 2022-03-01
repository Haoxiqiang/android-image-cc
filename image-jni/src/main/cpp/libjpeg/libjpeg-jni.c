#include <malloc.h>
#include "frdjpeg.h"

JNIEXPORT jboolean JNICALL
Java_com_thefrodo_image_jpeg_LibJpeg_isJpegImage(JNIEnv *env, jobject thiz, jobject path) {

    const char *input = (*env)->GetStringUTFChars(env, path, JNI_FALSE);
    /* source file */
    FILE *infile;
    if ((infile = fopen(input, "rb")) == NULL) {
        (*env)->ReleaseStringChars(env, path,
                                   (const jchar *) (const jchar *) (const jchar *) input);
        LOGE("can't open %s", input);
        return JNI_FALSE;
    }

    struct jpeg_decompress_struct info;

    struct jpeg_error_mgr err;
    info.err = jpeg_std_error(&err);

    jpeg_create_decompress(&info);
    jpeg_stdio_src(&info, infile);
    jpeg_read_header(&info, TRUE);

    JDIMENSION src_w = info.image_width;
    JDIMENSION src_h = info.image_height;

    LOGI("image(w:%d, h:%d)", src_w, src_h);

    jpeg_destroy_compress(&info);

    fclose(infile);
    (*env)->ReleaseStringChars(env, path, (const jchar *) input);

    return (src_w > 0 && src_h > 0) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_com_thefrodo_image_jpeg_LibJpeg_jpegInfo(JNIEnv *env, jobject thiz, jobject path) {

    const char *input = (*env)->GetStringUTFChars(env, path, JNI_FALSE);
    /* source file */
    FILE *infile;
    if ((infile = fopen(input, "rb")) == NULL) {
        (*env)->ReleaseStringChars(env, path,
                                   (const jchar *) (const jchar *) (const jchar *) input);
        LOGE("can't open %s", input);
        return;
    }

    struct jpeg_decompress_struct info;

    struct jpeg_error_mgr err;
    info.err = jpeg_std_error(&err);

    jpeg_create_decompress(&info);
    jpeg_stdio_src(&info, infile);
    jpeg_read_header(&info, TRUE);

    JDIMENSION src_w = info.image_width;
    JDIMENSION src_h = info.image_height;
    J_COLOR_SPACE color_space = info.jpeg_color_space;
    UINT8 major_version = info.JFIF_major_version;
    UINT8 minor_version = info.JFIF_minor_version;

    LOGI("image(w:%d, h:%d, color_space:%d, major_version:%d, minor_version:%d)",
         src_w, src_h, color_space, major_version, minor_version);

    jpeg_destroy_compress(&info);

    fclose(infile);
    (*env)->ReleaseStringChars(env, path, (const jchar *) input);
}

JNIEXPORT jint JNICALL
Java_com_thefrodo_image_jpeg_LibJpeg_calImageSubsamplingSize(JNIEnv *env, jobject obj,
                                                             jint width, jint height) {
    int sub_sample_size = cal_image_sub_sample((unsigned int) width, (unsigned int) height);
    LOGI("calImageSubsamplingSize(w:%d, h:%d) = %d", width, height, sub_sample_size);
    return (jint) sub_sample_size;
}

JNIEXPORT jint JNICALL
Java_com_thefrodo_image_jpeg_LibJpeg_nativeCompress(JNIEnv *env, jobject obj,
                                                    jstring src, jstring dst,
                                                    jint quality, jboolean fancy_upsampling) {
    return jpeg_compress_file(env, src, dst, quality, fancy_upsampling);
}

JNIEXPORT jboolean JNICALL
Java_com_thefrodo_image_jpeg_LibJpeg_nativeCompressBitmap(JNIEnv *env, jobject _,
                                                          jobject bitmap, jstring dst,
                                                          jint quality) {
    return (jboolean) (jpeg_compress_bitmap(env, bitmap, dst, quality) == 1 ? JNI_TRUE : JNI_FALSE);
}
