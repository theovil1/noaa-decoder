#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/wav.h"
#include "../include/demod.h"
#include "../include/sync.h"
#include "../include/bmp.h"
#include "../include/enhance.h"

int main(int argc, char *argv[]) {

    // Gestion des arguments
    if (argc < 2) {
        printf("Usage: ./decoder input.wav [-o output.bmp] [-c A|B|AB] [--enhance] [--median]\n");
        return 1;
    }

    // Arguments par défaut
    const char *output = "output.bmp";
    char channel = 'X'; // X = image complète
    int do_enhance = 0;
    int do_median = 0;

    // Parser les arguments
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output = argv[++i];
        } else if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
            i++;
            if (strcmp(argv[i], "A") == 0) channel = 'A';
            else if (strcmp(argv[i], "B") == 0) channel = 'B';
            else channel = 'X';
        } else if (strcmp(argv[i], "--enhance") == 0) {
            do_enhance = 1;
        } else if (strcmp(argv[i], "--median") == 0) {
            do_median = 1;
        }
    }

    // Chargement des samples
    WavHeader header;
    float *samples = NULL;
    int num_samples = wav_load_samples(argv[1], &header, &samples);

    if (num_samples < 0) {
        return 1;
    }

    // Démodulation
    float *envelope = demod_envelope(samples, num_samples, 5);
    uint8_t *pixels = demod_to_pixels(envelope, num_samples);

    // Découper en ligne
    uint8_t *image = NULL;
    int img_width;
    int num_lines = sync_build_image(pixels, num_samples, &image, &img_width);

    if (channel == 'A' || channel == 'B') {
        int ch_width = 909;
        int ch_start = (channel == 'A') ? 86 : 1126;
        uint8_t *ch = malloc(num_lines * ch_width);
        sync_extract_channel(image, img_width, num_lines, ch_start, ch_width, ch);

        if (do_median) enhance_median(ch, ch_width, num_lines);
        if (do_enhance) enhance_contrast(ch, num_lines * ch_width);

        write_bmp(output, ch, ch_width, num_lines);
        free(ch);
    } else {
        if (do_median) enhance_median(image, img_width, num_lines);
        if (do_enhance) enhance_contrast(image, num_lines * img_width);

        write_bmp(output, image, img_width, num_lines);
    }

    printf("Image sauvegardée: %s\n", output);

    free(image);
    free(pixels);
    free(envelope);
    free(samples);
    return 0;

}