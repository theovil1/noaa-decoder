#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/wav.h"

int wav_read_header(const char *filename, WavHeader *header) {

    // 1. Ouvrir le fichier en mode binaire
    FILE *f = fopen(filename, "rb");
    if (!f) {
        printf("Erreur: Impossible d'ouvrir %s\n", filename);
        return -1;
    }

    // 2. Lire les 44 octets d'un coup dans la struct
    size_t read = fread(header, sizeof(WavHeader), 1, f);
    if (read != 1) {
        printf("Erreur: impossible de lire l'en-tête\n");
        fclose(f);
        return -1;
    }

    fclose(f);

    // 3. Vérifications
    if (memcmp(header->riff_id, "RIFF", 4) != 0) {
        printf("Erreur: pas un fichier RIFF\n");
        return -1;
    }

    if (memcmp(header->wave_id, "WAVE", 4) != 0) {
        printf("Erreur: pas un fichier WAVE\n");
        return -1;
    }

    if (header->audio_format != 1) {
        printf("Erreur: fichier compressé, non géré\n");
        return -1;
    }

    return 0;
}

int wav_load_samples(const char *filename, WavHeader *header, float **samples) {

    // 1. Lire et vérifier le header
    if (wav_read_header(filename, header) != 0)
        return -1;

    // 2. Rouvrir le fichier et sauter les 44 octets du header
    FILE *f = fopen(filename, "rb");
    if (!f)
        return -1;
    fseek(f, sizeof(WavHeader), SEEK_SET);

    // 3. Nombre d'échantillons = taille des données /2 (car 16 bits = 2 octets)
    int num_samples = header->data_size / 2;

    // 4. Allouer le tableau de floats
    *samples = malloc(num_samples * sizeof(float));
    if (!*samples) {
        fclose(f);
        return -1;
    }

    // 5. Lire et normaliser chaque échantillon
    for (int i = 0; i < num_samples; i++) {
        int16_t raw;
        fread(&raw, sizeof(int16_t), 1, f);
        (*samples)[i] = raw / 32768.0f;
    }

    fclose(f);

    return num_samples;
}