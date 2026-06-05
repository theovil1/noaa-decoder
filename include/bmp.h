#ifndef BMP_H
#define BMP_H

#include "enhance.h"
#include <stdint.h>
#include <sys/cdefs.h>

typedef struct __attribute__((packed)) {
    char        signature[2];   // "BM"
    uint32_t    file_size;
    uint16_t    reserved1;
    uint16_t    reserved2;
    uint32_t    pixel_offset;   // 14 + 40 + 1024 = 1078
} BMPFileHeader;

typedef struct __attribute__((packed)) {
    uint32_t    header_size;        // 40
    int32_t     width;
    int32_t     height;
    uint16_t    planes;             // 1
    uint16_t    bits_per_pixel;     // 8
    uint32_t    compression;        // 0
    uint32_t    image_size;
    int32_t     x_ppm;              // 0
    int32_t     y_ppm;              // 0
    uint32_t    colors_used;        // 256
    uint32_t    colors_important;   // 0
} BMPInfoHeader;

int write_bmp(const char *filename, const uint8_t *pixels,
            int width, int height);

int write_bmp_rgb(const char *filename, const uint8_t *pixels,
                const RGB *lut, int width, int height);

#endif