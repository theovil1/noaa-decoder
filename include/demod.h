#ifndef DEMOD_H
#define DEMOD_H

#include <stdint.h>

// Redresser le signal (valeur absolue) puis lisse avec moyenne glissante
// Retourne un tableau de floats positifs lissés, ou NULL si erreur
float *demod_envelope(const float *samples, int num_samples, int windows);

// Convertit les floats en pixels 0-255
uint8_t *demod_to_pixels(const float *envelope, int num_samples);

#endif