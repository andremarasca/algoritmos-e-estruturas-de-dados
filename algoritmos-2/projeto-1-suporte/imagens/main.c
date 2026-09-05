/*
 * Image support example: load an image, invert RGB pixels, save a PNG.
 * Download this entire directory, including vendor/, before compiling.
 *
 * Build (GCC or Clang, from this directory):
 *   gcc -std=c17 -Wall -Wextra -Wpedantic -isystem vendor main.c image_codecs.c -o image_pixels -lm
 * Run on Windows:
 *   .\image_pixels.exe input.png output.png
 * Run on Linux/macOS:
 *   ./image_pixels input.png output.png
 *
 * Input: PNG, JPEG, BMP or another format supported by stb_image.
 * Output: always PNG, regardless of the chosen file name.
 * Images use row-major RGBA bytes. Alpha is preserved, not inverted.
 * Replace transform_pixels() to experiment with a different operation.
 * This example does not identify regions or implement project solutions.
 */

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <stb_image.h>
#include <stb_image_write.h>

typedef struct {
    int width;
    int height;
    unsigned char *pixels;
} image;

/* On success, output owns the pixels until image_destroy() is called. */
static bool image_load(const char *path, image *output) {
    int source_channels = 0;
    output->pixels = stbi_load(path, &output->width, &output->height,
                              &source_channels, STBI_rgb_alpha);
    if (output->pixels == NULL) {
        const char *reason = stbi_failure_reason();
        fprintf(stderr, "Cannot read image: %s\n", reason != NULL ? reason : "unknown error");
        return false;
    }

    if (output->width <= 0 || output->height <= 0 || output->width > INT_MAX / 4) {
        fprintf(stderr, "Unsupported image dimensions.\n");
        stbi_image_free(output->pixels);
        *output = (image){0};
        return false;
    }
    return true;
}

/* Valid coordinates: 0 <= row < height and 0 <= column < width. */
static unsigned char *image_pixel_at(image *picture, int row, int column) {
    size_t pixel_index = (size_t)row * (size_t)picture->width + (size_t)column;
    return picture->pixels + pixel_index * 4U;
}

static void transform_pixels(image *picture) {
    for (int row = 0; row < picture->height; row++) {
        for (int column = 0; column < picture->width; column++) {
            unsigned char *pixel = image_pixel_at(picture, row, column);

            /* Channels: pixel[0] = red, [1] = green, [2] = blue, [3] = alpha.
             * Values range from 0 to 255. The top-left pixel is (0, 0).
             * This operation inverts each color channel independently.
             */
            pixel[0] = (unsigned char)(255U - pixel[0]);
            pixel[1] = (unsigned char)(255U - pixel[1]);
            pixel[2] = (unsigned char)(255U - pixel[2]);
        }
    }
}

static bool image_save_png(const char *path, const image *picture) {
    int saved = stbi_write_png(path, picture->width, picture->height,
                              STBI_rgb_alpha, picture->pixels, picture->width * 4);
    if (saved == 0) {
        fprintf(stderr, "Cannot save PNG: %s\n", path);
        return false;
    }
    return true;
}

static void image_destroy(image *picture) {
    stbi_image_free(picture->pixels);
    *picture = (image){0};
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s input_image output.png\n", argv[0]);
        return EXIT_FAILURE;
    }

    image picture = {0};
    if (!image_load(argv[1], &picture)) {
        return EXIT_FAILURE;
    }

    transform_pixels(&picture);
    bool saved = image_save_png(argv[2], &picture);
    if (saved) {
        printf("Saved %d x %d pixels to %s\n", picture.width, picture.height, argv[2]);
    }
    image_destroy(&picture);
    return saved ? EXIT_SUCCESS : EXIT_FAILURE;
}
