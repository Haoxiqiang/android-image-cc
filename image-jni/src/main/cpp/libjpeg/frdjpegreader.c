#include <malloc.h>
#include <string.h>
#include "frdjpeg.h"

METHODDEF(void)

frd_jpeg_dc_error_exit(j_common_ptr
jpeg_dc_info) {
// jpeg_dc_info->err really points to a frd_jpeg_error_mgr struct, so coerce pointer
frd_jpeg_error_ptr fj_err = (frd_jpeg_error_ptr) jpeg_dc_info->err;
//  We could postpone this until after returning, if we chose.
(*jpeg_dc_info->err->output_message)(jpeg_dc_info);
// Return control to the setjmp point
longjmp(fj_err
->set_jmp_buffer, 1);
}

int read_JPEG_file(struct FRD_IMAGE *frd_image) {

    struct jpeg_decompress_struct jpeg_dc_info;
    struct frd_jpeg_error_mgr frd_jpeg_err;
    FILE *infile;                 /* source file */

    if ((infile = fopen(frd_image->input, "rb")) == NULL) {
        LOGE("can't open %s", frd_image->input);
        return FILE_OPEN_FAILED;
    }

    // Step 1: set up the normal JPEG error routines, then override error_exit
    jpeg_dc_info.err = jpeg_std_error(&frd_jpeg_err.pub);
    frd_jpeg_err.pub.error_exit = frd_jpeg_dc_error_exit;

    // Step 2 establish the setjmp return context for frd_jpeg_error_exit to use.
    if (setjmp(frd_jpeg_err.set_jmp_buffer)) {
        jpeg_destroy_decompress(&jpeg_dc_info);
        fclose(infile);
        return LIBJPEG_SIGNAL_ERROR;
    }

    // initialize the JPEG decompression object
    jpeg_create_decompress(&jpeg_dc_info);
    // Step 2: read data from file and fill jpeg_dc_info
    jpeg_stdio_src(&jpeg_dc_info, infile);
    // Step 3: read file parameters with jpeg_read_header(), ignore the return value from jpeg_read_header,since
    // (a) suspension is not possible with the stdio data source, and
    // (b) we passed TRUE to reject a tables-only JPEG file as an error.
    (void) jpeg_read_header(&jpeg_dc_info, TRUE);

    jpeg_calc_output_dimensions(&jpeg_dc_info);

    // Step 4: set parameters for decompression
    JDIMENSION src_w = jpeg_dc_info.image_width;
    JDIMENSION src_h = jpeg_dc_info.image_height;

    // 图像大小就是 scale_num/scale_denom，支持1/1, 1/2, 1/4,和1/8这几种缩小比例。
    unsigned int scale_denom = cal_image_sub_sample(src_w, src_h);
    jpeg_dc_info.scale_num = 1;
    jpeg_dc_info.scale_denom = scale_denom;

    if (jpeg_dc_info.jpeg_color_space == JCS_GRAYSCALE) {
        jpeg_dc_info.out_color_space = JCS_GRAYSCALE;
        jpeg_dc_info.output_components = 1;
    } else {
        jpeg_dc_info.out_color_space = JCS_RGB;
        jpeg_dc_info.output_components = 3;
    }

    jpeg_dc_info.do_fancy_upsampling = frd_image->fancy_upsampling;

    // Step 5: start decompressor ignore the return value since suspension is not possible with the stdio data source.
    (void) jpeg_start_decompress(&jpeg_dc_info);

    frd_image->in_w = jpeg_dc_info.image_width;
    frd_image->in_h = jpeg_dc_info.image_height;
    frd_image->out_w = jpeg_dc_info.output_width;
    frd_image->out_h = jpeg_dc_info.output_height;
    frd_image->components = jpeg_dc_info.output_components;
    frd_image->color_space = jpeg_dc_info.out_color_space;

    long counter = 0;
    long may_stride = (long) (jpeg_dc_info.output_width * jpeg_dc_info.output_components);
    if (may_stride > INT32_MAX || may_stride < 0) {
        LOGE("stride is too big. output_width:%d output_components:%d", jpeg_dc_info.output_width,
             jpeg_dc_info.output_components);
        jpeg_destroy_decompress(&jpeg_dc_info);
        fclose(infile);
        return LIBJPEG_TOO_BIG;
    }
    size_t row_stride = (size_t) may_stride;
    frd_image->pixels = malloc(
            sizeof(unsigned char) * may_stride * jpeg_dc_info.output_height);

    JDIMENSION buffer_height = 1;
    JSAMPARRAY buffer = (JSAMPARRAY) malloc(sizeof(JSAMPROW) * buffer_height);
    buffer[0] = (JSAMPROW) malloc(sizeof(JSAMPLE) * row_stride);

    // Step 6: read data
    while (jpeg_dc_info.output_scanline < jpeg_dc_info.output_height) {
        (void) jpeg_read_scanlines(&jpeg_dc_info, buffer, buffer_height);
        memcpy(frd_image->pixels + counter, buffer[0], row_stride);
        counter += row_stride;
    }

    // Step 7: Finish decompression
    (void) jpeg_finish_decompress(&jpeg_dc_info);

    // Step 8: Release JPEG decompression object
    jpeg_destroy_decompress(&jpeg_dc_info);

    free(buffer[0]);
    free(buffer);
    fclose(infile);
    return 1;
}

