#include <malloc.h>
#include "frdjpeg.h"

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
