#include <png.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "../include/png_writter.h"
#include "../include/enhance.h"

int write_png(const char *filename, const uint8_t *pixels, int width, int height) {

    FILE *f = fopen(filename, "wb");
    if (!f) return -1;

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info = png_create_info_struct(png);

    png_init_io(png, f);
    png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_GRAY,
         PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    for (int y = 0; y < height; y++) {
        png_write_row(png, &pixels[y * width]);
    }

    png_write_end(png, NULL);
    png_destroy_write_struct(&png, &info);
    fclose(f);
    return 0;

}

int write_png_rgb(const char *filename, const uint8_t *pixels, const RGB *lut, int width, int height) {

    FILE *f = fopen(filename, "wb");
    if (!f) return -1;

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info = png_create_info_struct(png);

    png_init_io(png, f);
    png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    uint8_t *row = malloc(width * 3);
    for (int y = 0; y < height; y++) {

        for (int x = 0; x < width; x++) {
            uint8_t gray = pixels[y * width + x];
            row[x * 3]      = lut[gray].r;
            row[x * 3 + 1]  = lut[gray].g;
            row[x * 3 + 2]  = lut[gray].b;
        }
        png_write_row(png, row);
    }

    free(row);
    png_write_end(png, NULL);
    png_destroy_write_struct(&png, &info);
    fclose(f);
    return 0;

}