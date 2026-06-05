#ifndef WAV_H
#define WAV_H

#include <stdint.h>

typedef struct __attribute__((packed)) {
    char        riff_id[4];         // "RIFF"
    uint32_t    file_size;          // Taille totale - 8
    char        wave_id[4];         // "WAVE"
    char        fmt_id[4];          // "fmt "
    uint32_t    fmt_size;           // 16 pour PCM
    uint16_t    audio_format;       // 1 = PCM
    uint16_t    num_channels;       // 1 = mono
    uint32_t    sample_rate;        // 11025
    uint32_t    byte_rate;          // sample_rate * channels * bits/8
    uint16_t    block_align;        // channels * bits/8
    uint16_t    bits_per_sample;    // 16
    char        data_id[4];         // "data"
    uint32_t    data_size;          // taille des données audio
} WavHeader;

// Lit le header et vérifie que c'est un WAV PCM valide
// Retourne 0 si OK, -1 si erreur
int wav_read_header(const char *filename, WavHeader *header);

// Charge tous les échantillons normalisés entre -1.0 et +1.0
// Retourne le nombre d'échantillon, ou -1 si erreur
int wav_load_samples(const char *filename, WavHeader *header, float **samples);

#endif