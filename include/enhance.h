#ifndef ENHANCE_H
#define ENHANCE H

#include <stdint.h>

void enhance_contrast(uint8_t *pixels, int num_pixels);

void enhance_median(uint8_t *pixels, int width, int height);

#endif