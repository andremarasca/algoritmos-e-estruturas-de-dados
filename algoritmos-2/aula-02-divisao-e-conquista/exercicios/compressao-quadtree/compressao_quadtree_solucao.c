/*
 * Solução do exercício de compressão de uma imagem binária por quadtree.
 *
 * Compilação:
 * gcc -std=c17 -Wall -Wextra -Wpedantic compressao_quadtree_solucao.c -o compressao_quadtree
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

enum {
    IMAGE_SIZE = 4
};

static bool region_is_uniform(
    const int image[][IMAGE_SIZE],
    size_t row,
    size_t column,
    size_t size,
    int *color
) {
    *color = image[row][column];

    for (size_t current_row = row; current_row < row + size; current_row++) {
        for (
            size_t current_column = column;
            current_column < column + size;
            current_column++
        ) {
            if (image[current_row][current_column] != *color) {
                return false;
            }
        }
    }

    return true;
}

static void encode_region(
    const int image[][IMAGE_SIZE],
    size_t row,
    size_t column,
    size_t size
) {
    int color = 0;

    if (region_is_uniform(image, row, column, size, &color)) {
        printf("%d", color);
        return;
    }

    const size_t half = size / 2U;

    putchar('(');
    encode_region(image, row, column, half);
    encode_region(image, row, column + half, half);
    encode_region(image, row + half, column, half);
    encode_region(image, row + half, column + half, half);
    putchar(')');
}

int main(void) {
    const int image[IMAGE_SIZE][IMAGE_SIZE] = {
        {0, 0, 1, 1},
        {0, 0, 1, 1},
        {0, 1, 1, 1},
        {0, 0, 1, 1}
    };

    printf("Codificação: ");
    encode_region(image, 0U, 0U, IMAGE_SIZE);
    putchar('\n');
    return EXIT_SUCCESS;
}
