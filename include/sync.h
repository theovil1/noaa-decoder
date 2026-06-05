#ifndef SYNC_H
#define SYNC_H

#include <stdint.h>

// Rééchantillonne et découpe le flux en ligne de 2080 pixels
// Retourne le nombre de lignes
int sync_build_image(const uint8_t *pixels, int num_samples, 
                    uint8_t **image, int *width);

void sync_extract_channel(const uint8_t *image, int width, int num_lines,
                        int channel_start, int channel_width, uint8_t *ouput);

#endif