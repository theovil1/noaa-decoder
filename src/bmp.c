#include <stdint.h>
#include <stdio.h>
#include "../include/bmp.h"

int write_bmp(const char *filename, const uint8_t *pixels, int width, int height) {

    FILE *f = fopen(filename, "wb");
    if (!f) return -1;

    int padding =  (4 - (width % 4)) % 4;
    int row_size = width + padding;
    int image_size = row_size * height;

    // File header
    BMPFileHeader fh = {
        .signature = {'B', 'M'},
        .file_size = 1078 + image_size,
        .reserved1 = 0,
        .reserved2 = 0,
        .pixel_offset = 1078
    };
    fwrite(&fh, sizeof(fh), 1, f);

    // Info header
    BMPInfoHeader ih = {
        .header_size = 40,
        .width = width,
        .height = height,
        .planes = 1,
        .bits_per_pixel = 8,
        .compression = 0,
        .image_size = image_size,
        .x_ppm = 0,
        .y_ppm = 0,
        .colors_used = 256,
        .colors_important = 0
    };
    fwrite(&ih, sizeof(ih), 1, f);

    // Palette de gris : 256 entrées de 4 octets (RGB + 0)
    for (int i = 0; i < 256; i++) {
        uint8_t entry[4] = {i, i, i, 0};
        fwrite(entry, 4, 1, f);
    }

    // Pixels, lignes inversées (dernière ligne en premier)
    uint8_t pad[3] = {0};
    for (int y = height - 1; y >= 0; y--) {
        fwrite(&pixels[y * width], 1, width, f);
        if (padding > 0) {
            fwrite(pad, 1, padding, f);
        }
    }

    fclose(f);
    return 0;

}