#include <malloc.h>
#include "frdjpeg.h"

// TODO Libjpeg support 1-16 scale
unsigned int cal_image_sub_sample(unsigned int src_w, unsigned int src_h) {
    unsigned int width = src_w;
    if (src_w % 2 == 1) {
        width += 1;
    }

    unsigned int height = src_h;
    if (src_h % 2 == 1) {
        height += 1;
    }

    float long_side = (float) max(width, height);
    float short_side = (float) min(width, height);
    unsigned int ret;
    if (long_side == 0 || short_side == 0) {

    }
    float scale = short_side / long_side;

    if (scale <= 1 && scale > 0.5625) {
        if (long_side < 1664) {
            ret = 1;
        } else if (long_side < 4990) {
            ret = 2;
        } else if (long_side < 10240) {
            ret = 4;
        } else {
            ret = 8;
        }
    } else if (scale <= 0.5625 && scale > 0.5) {
        if (long_side / 1280 == 0) {
            ret = 1;
        } else {
            ret = (unsigned int) (long_side / 1280);
        }
    } else {
        ret = (unsigned int) ceil(long_side / (1280.0 / scale));
    }
    return ret;
}

size_t allocated_size(void *ptr) {
    return ((size_t *) ptr)[-1];
}

void free_frd_image(JNIEnv *env, struct FRD_IMAGE *frd_image) {
    (*env)->ReleaseStringChars(env, frd_image->src, (const jchar *) frd_image->input);
    (*env)->ReleaseStringChars(env, frd_image->dst, (const jchar *) frd_image->output);
    free(frd_image->pixels);
}
