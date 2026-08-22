/*
 * Solução do exercício de localização de um ninho de abelhas.
 *
 * Compilação:
 * gcc -std=c17 -Wall -Wextra -Wpedantic ninho_abelhas_solucao.c -o ninho_abelhas
 *
 * Complexidade: O(log n).
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

enum {
    SEARCH_BEGIN = 0,
    SEARCH_END = 1024,
    MAX_RELEASES = 11
};

typedef enum {
    BEE_LEFT = -1,
    NEST_FOUND = 0,
    BEE_RIGHT = 1
} BeeDirection;

static int nest_position = 0;
static int release_count = 0;

/* Simula a direção tomada pela abelha depois de sua soltura. */
BeeDirection release_bee(int position) {
    release_count++;

    if (nest_position < position) {
        return BEE_LEFT;
    }
    if (nest_position > position) {
        return BEE_RIGHT;
    }
    return NEST_FOUND;
}

/* O ninho está no intervalo [begin, end). */
static int locate_nest(int begin, int end) {
    while (begin < end) {
        const int middle = (begin + end) / 2;
        const BeeDirection direction = release_bee(middle);

        if (direction == NEST_FOUND) {
            return middle;
        }

        if (direction == BEE_LEFT) {
            end = middle;
        } else {
            begin = middle + 1;
        }
    }

    return -1;
}

static bool run_test(int expected_position) {
    nest_position = expected_position;
    release_count = 0;

    const int found_position = locate_nest(SEARCH_BEGIN, SEARCH_END);
    const bool found = found_position == expected_position;
    const bool respected_limit = release_count <= MAX_RELEASES;

    printf(
        "Ninho em %4d | resposta %4d | solturas %2d | %s\n",
        expected_position,
        found_position,
        release_count,
        found && respected_limit ? "correto" : "incorreto"
    );

    return found && respected_limit;
}

int main(void) {
    const int test_positions[] = {0, 1, 5, 511, 512, 731, 1023};
    const size_t test_count = sizeof(test_positions) / sizeof(test_positions[0]);
    bool all_tests_passed = true;

    for (size_t index = 0U; index < test_count; index++) {
        if (!run_test(test_positions[index])) {
            all_tests_passed = false;
        }
    }

    if (!all_tests_passed) {
        return EXIT_FAILURE;
    }

    puts("Todos os ninhos foram localizados dentro do limite.");
    return EXIT_SUCCESS;
}
