#ifndef ENHANCE_H
#define ENHANCE_H

#include <stdint.h>

typedef struct {
    uint8_t r, g, b;
} RGB;

void enhance_contrast(uint8_t *pixels, int num_pixels);

void enhance_median(uint8_t *pixels, int width, int height);

void enhance_create_thermal_lut(RGB *lut);

#endif