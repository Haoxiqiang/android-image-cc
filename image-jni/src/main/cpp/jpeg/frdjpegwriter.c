#include "frdjpeg.h"

int
write_JPEG_file(struct FRD_IMAGE *frd_image, int quality) {
    struct jpeg_compress_struct jpeg_c_info;
    struct jpeg_error_mgr jerr;

    FILE *outfile;                /* target file */

    // Step 1: allocate and initialize JPEG compression object
    jpeg_c_info.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&jpeg_c_info);

    if ((outfile = fopen(frd_image->output, "wb")) == NULL) {
        fprintf(stderr, "can't open %s\n", frd_image->output);
        return LIBJPEG_SIGNAL_ERROR;
    }

    jpeg_stdio_dest(&jpeg_c_info, outfile);

    // Step 3: set parameters for compression
    jpeg_c_info.image_width = frd_image->out_w;
    jpeg_c_info.image_height = frd_image->out_h;
    jpeg_c_info.input_components = frd_image->components;
    jpeg_c_info.in_color_space = frd_image->color_space;

    jpeg_set_defaults(&jpeg_c_info);

    jpeg_set_quality(&jpeg_c_info, quality, TRUE);

    // Step 4: Start compressor
    jpeg_start_compress(&jpeg_c_info, TRUE);
    // Step 5: while (scan lines remain to be written)

    JSAMPROW row_pointer[1];

    int row_stride = frd_image->out_w * 3;

    while (jpeg_c_info.next_scanline < jpeg_c_info.image_height) {
        int position = jpeg_c_info.next_scanline * row_stride;
        row_pointer[0] = &frd_image->pixels[position];
        (void) jpeg_write_scanlines(&jpeg_c_info, row_pointer, 1);
    }

    // Step 6: Finish compression
    jpeg_finish_compress(&jpeg_c_info);
    /* After finish_compress, we can close the output file. */
    fclose(outfile);
    // Step 7: release JPEG compression object
    jpeg_destroy_compress(&jpeg_c_info);
    return 1;
}
