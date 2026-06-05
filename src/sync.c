#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include "../include/sync.h"

// 39 pixels de sync sur 2080 px/ligne, à 5512.5 samples/ligne = ~103 samples
#define SAMPLES_PER_LINE 5512
#define SYNC_PATTERN_LEN 103
#define SEARCH_TOLERANCE 5
#define NCC_THRESHOLD 0.45f
#define SAMPLES_PER_LINE_F 5512.5f

static void create_sync_pattern(uint8_t *pattern) {

    // Sync A = 1040 Hz à 11025 Hz → demi-période ≈ 5.3 samples
    float half_period = 11025.0f / (1040.0f * 2.0f);
    for (int i = 0; i < SYNC_PATTERN_LEN; i++) {
        int bar = (int)(i / half_period);
        pattern[i] = (bar % 2 == 0) ? 0 : 255;
    }

}

static float correlate(const uint8_t *pixels, int pos, const uint8_t *pattern, int pattern_len) {

    float mean_s = 0, mean_p = 0;
    for (int i = 0; i < pattern_len; i++) {
        mean_s += pixels[pos + i];
        mean_p += pattern[i];
    }
    mean_s /= pattern_len;
    mean_p /= pattern_len;

    float num = 0, den_s = 0, den_p = 0;
    for (int i = 0; i < pattern_len; i++) {
        float ds = pixels[pos + i] - mean_s;
        float dp = pattern[i] - mean_p;
        num += ds * dp;
        den_s += ds * ds;
        den_p += dp * dp;
    }

    float den = sqrtf(den_s * den_p);
    if (den < 1e-6f) return 0;
    return num / den;

}

static int find_first_sync(const uint8_t *pixels, int num_pixels, const uint8_t *pattern) {

    float best_score = -1;
    int best_pos = 0;

    int search_len = SAMPLES_PER_LINE * 2;
    if (search_len > num_pixels - SYNC_PATTERN_LEN)
        search_len = num_pixels - SYNC_PATTERN_LEN;

    for (int i = 0; i < search_len; i++) {
        float s = correlate(pixels, i, pattern, SYNC_PATTERN_LEN);
        if (s > best_score) {
            best_score = s;
            best_pos = i;
        }
    }

    return best_pos;
}

static int find_sync_positions(const uint8_t *pixels, int num_pixels, int **positions) {

    uint8_t pattern[SYNC_PATTERN_LEN];
    create_sync_pattern(pattern);

    int capacity = 2000;
    *positions = malloc(capacity * sizeof(int));
    int count = 0;

    float pos = (float)find_first_sync(pixels, num_pixels, pattern);
    (*positions)[count++] = (int)pos;

    while (pos + SAMPLES_PER_LINE_F < num_pixels - SYNC_PATTERN_LEN) {
        float pred = pos + SAMPLES_PER_LINE_F;
        int expected = (int)(pred + 0.5f);

        int from = expected - SEARCH_TOLERANCE;
        int to = expected + SEARCH_TOLERANCE;
        if (from < 0) from = 0;
        if (to > num_pixels - SYNC_PATTERN_LEN) to = num_pixels - SYNC_PATTERN_LEN;

        float best_score = -1;
        int best_pos = expected;
        for (int j = from; j < to; j++) {
            float s = correlate(pixels, j, pattern, SYNC_PATTERN_LEN);
            if (s > best_score) {
                best_score = s;
                best_pos = j;
            }
        }

        if (best_score >= NCC_THRESHOLD)
            pos = (float)best_pos;
        else
            pos = pred;

        (*positions)[count++] = (int)(pos + 0.5f);
    }

    return count;

}

void sync_extract_channel(const uint8_t *image, int width, int num_lines, int channel_start, int channel_width, uint8_t *output) {

    for (int line = 0; line < num_lines; line++) {
        
        for (int col = 0; col < channel_width; col++) {
            output[line * channel_width + col] = image[line * width + channel_start + col];
        } 

    }

}

int sync_build_image(const uint8_t *pixels, int num_samples, uint8_t **image, int *width) {

    *width = 2080;

    int *positions = NULL;
    int num_lines = find_sync_positions(pixels, num_samples, &positions);

    *image = malloc(num_lines * (*width) * sizeof(uint8_t));
    if (!image) {
        free(positions);
        return -1;
    }

    float samples_per_line = 11025.0f / 2.0f;

    for (int line = 0; line < num_lines; line++) {
        int line_start = positions[line];

        for (int col = 0; col < *width; col++) {
            int src = line_start + (int)(col * samples_per_line / *width);
            if (src < num_samples) {
                (*image)[line * (*width) + col] = pixels[src];
            } else {
                (*image)[line * (*width) + col] = 0;
            }
        }

    }

    free(positions);
    return num_lines;
}