#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "../include/demod.h"

float *demod_envelope(const float *samples, int num_samples, int window) {

    float *rectified = malloc(num_samples * sizeof(float));
    if (!rectified) return NULL;

    // 1. Redressement (valeur absolue)
    for (int i = 0; i < num_samples; i++) {
        rectified[i] = fabs(samples[i]);
    }

    // 2. Moyenne glissante
    float *smoothed = malloc(num_samples * sizeof(float));
    if (!smoothed) {
        free(rectified);
        return NULL;
    }

    int half = window / 2;
    for (int i = 0; i < num_samples; i++) {
        float sum = 0;
        int count = 0;

        for (int j = i - half; j <= i + half; j++) {
            if (j >= 0 &&  j <num_samples) {
                sum += rectified[j];
                count++;
            }
        }

        smoothed[i] = sum / count;
    }

    free(rectified);
    return smoothed;

}

uint8_t *demod_to_pixels(const float *envelope, int num_samples) {

    uint8_t *pixels = malloc(num_samples * sizeof(uint8_t));
    if (!pixels) return NULL;

    // 1. Trouver min et max
    float min = envelope[0];
    float max = envelope[0];

    for (int i = 1; i < num_samples; i++) {
        if (envelope[i] < min) min = envelope[i];
        if (envelope[i] > max) max = envelope[i];
    }

    // 2. Convertir chaque valeur en 0-255
    for (int i = 0; i < num_samples; i++) {
        pixels[i] = (uint8_t) (((envelope[i] - min) / (max - min)) * 255.0f);
    }

    return pixels;

}