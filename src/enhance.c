#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../include/enhance.h"

static int compare_uint8(const void *a, const void *b) {
    return (int)(*(uint8_t *)a) - (int)(*(uint8_t *)b);
}

void enhance_contrast(uint8_t *pixels, int num_pixels) {

    // 1. Copier et trier pour trouver les percentiles
    uint8_t *sorted = malloc(num_pixels);
    memcpy(sorted, pixels, num_pixels);
    qsort(sorted, num_pixels, sizeof(uint8_t), compare_uint8);

    // 2. Percentiles 2% et 98%
    uint8_t low = sorted[(int)(num_pixels * 0.02f)];
    uint8_t high = sorted[(int)(num_pixels * 0.98f)];
    free(sorted);

    if (high <= low) return;

    // 3. Remappe chaque pixel sur 0-255
    for (int i = 0; i < num_pixels; i++) {
        if (pixels[i] <= low)
            pixels[i] = 0;
        else if (pixels[i] >= high)
            pixels[i] = 255;
        else
            pixels[i] = (uint8_t)(((pixels[i] - low) * 255.0f) / (high - low));
    }

}

void enhance_median(uint8_t *pixels, int width, int height) {

    uint8_t *copy = malloc(width * height);
    memcpy(copy, pixels, width * height);

    for (int y = 1; y < height - 1; y++) {
        
        for (int x = 1; x < width - 1; x++) {
            // Récupérer les 9 voisins
            uint8_t neighbors[9];
            int n = 0;
            for (int dy = -1; dy <= 1; dy++) {

                for (int dx = -1; dx <= 1; dx++) {
                    neighbors[n++] = copy[(y + dy) * width + (x + dx)];
                }

            }

            // Tri insertion pour trouver la médiane
            for (int i = 1; i < 9; i++) {
                uint8_t key = neighbors[i];
                int j = i - 1;
                while (j >= 0 && neighbors[j] > key) {
                    neighbors[j + 1] = neighbors[j];
                    j--;
                }
                neighbors[j + 1] = key;
            }

            pixels[y * width + x] = neighbors[4]; // médiane = milieu
        }

    }

    free(copy);
}

void enhance_create_thermal_lut(RGB *lut) {

    for (int i = 0; i < 256; i++) {
        float t = i / 255.0f;

        if (t < 0.25f) {
            // Noir -> Bleu
            float s = t / 0.25f;
            lut[i].r = 0;
            lut[i].g = 0;
            lut[i].b = (uint8_t)(s * 255);
        } else if (t < 0.25f) {
            // Bleu -> Cyan
            float s = (t - 0.25f) / 0.25f;
            lut[i].r = 0;
            lut[i].g = (uint8_t)(s * 255);
            lut[i].b = 255;
        } else if (t < 0.75f) {
            // Cyan -> Jaune
            float s = (t - 0.5f) / 0.25f;
            lut[i].r = (uint8_t)(s * 255);
            lut[i].g = 255;
            lut[i].b = (uint8_t)((1 - s) * 255);
        } else {
            // Jaune -> Rouge
            float s = (t - 0.75f) / 0.25f;
            lut[i].r = 255;
            lut[i].g = (uint8_t)((1 - s) * 255);
            lut[i].b = 0;
        }
    }

}