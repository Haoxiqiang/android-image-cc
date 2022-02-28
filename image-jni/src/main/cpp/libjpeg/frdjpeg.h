#ifndef FRD_JPEG_H
#define FRD_JPEG_H

#include <jni.h>
#include <stdio.h>
#include <jpeglib.h>
#include <android/log.h>
#include <setjmp.h>
#include <math.h>

#define LOG_TAG "LibJpeg"
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG,__VA_ARGS__)
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define FILE_OPEN_FAILED -1
#define LIBJPEG_SIGNAL_ERROR -2
#define LIBJPEG_TOO_BIG -3

#ifndef min
#define min(a, b)  ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a, b)  ((a) > (b) ? (a) : (b))
#endif

typedef struct frd_jpeg_error_mgr {
    struct jpeg_error_mgr pub;
    jmp_buf set_jmp_buffer;
} *frd_jpeg_error_ptr;

struct FRD_IMAGE {

    jstring src;
    jstring dst;

    const char *input;
    const char *output;

    unsigned int in_w;
    unsigned int in_h;
    int components;
    J_COLOR_SPACE color_space;
    unsigned int out_w;
    unsigned int out_h;
    boolean fancy_upsampling;
    JOCTET *icc_profile;

    unsigned char *pixels;
};

typedef uint8_t BYTE;

unsigned int cal_image_sub_sample(unsigned int src_w, unsigned int src_h);

int read_JPEG_file(struct FRD_IMAGE *frd_image);

int write_JPEG_file(struct FRD_IMAGE *frd_image, int quality);

int
jpeg_compress_file(JNIEnv *env, jstring src, jstring dst, jint quality, jboolean fancy_upsampling);

int jpeg_compress_bitmap(JNIEnv *env, jobject bitmap, jstring dst, jint quality);

void free_frd_image(JNIEnv *env, struct FRD_IMAGE *frd_image);

size_t allocated_size(void *ptr);

#endif
