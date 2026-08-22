/*
 * Busca binária em um vetor ordenado.
 *
 * Compilação:
 * gcc -std=c17 -Wall -Wextra -Wpedantic busca_binaria.c -o busca_binaria
 *
 * Complexidade: O(log n).
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

static int binary_search(const int keys[], size_t count, int target) {
    size_t begin = 0U;
    size_t end = count;

    while (begin < end) {
        const size_t middle = (begin + end) / 2U;

        if (keys[middle] == target) {
            return (int)middle;
        }

        if (target < keys[middle]) {
            end = middle;
        } else {
            begin = middle + 1U;
        }
    }

    return -1;
}

int main(void) {
    const int keys[] = {3, 8, 12, 17, 25, 31, 42, 56};
    const size_t count = sizeof(keys) / sizeof(keys[0]);
    const int target = 42;
    const int position = binary_search(keys, count, target);

    if (position == -1) {
        printf("Chave %d não encontrada.\n", target);
        return EXIT_SUCCESS;
    }

    printf("Chave %d encontrada na posição %d.\n", target, position);
    return EXIT_SUCCESS;
}
