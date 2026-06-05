#ifndef PNG_WRITTER_H
#define PNG_WRITTER_H

#include "enhance.h"
#include <stdint.h>

int write_png(const char *filename, const uint8_t *pixels,
            int width, int height);

int write_png_rgb(const char *filename, const uint8_t *pixels,
                const RGB *lut, int width, int height);

#endif  