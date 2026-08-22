/*
 * Merge Sort com um único vetor auxiliar.
 *
 * Compilação:
 * gcc -std=c17 -Wall -Wextra -Wpedantic merge_sort.c -o merge_sort
 *
 * Complexidade: O(n log n) em tempo e O(n) em memória auxiliar.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/* Combina as duas metades ordenadas do intervalo [begin, end). */
static void merge(
    int values[],
    int auxiliary[],
    size_t begin,
    size_t middle,
    size_t end
) {
    size_t left = begin;
    size_t right = middle;
    size_t destination = begin;

    while (left < middle && right < end) {
        if (values[left] <= values[right]) {
            auxiliary[destination] = values[left];
            left++;
        } else {
            auxiliary[destination] = values[right];
            right++;
        }
        destination++;
    }

    while (left < middle) {
        auxiliary[destination] = values[left];
        left++;
        destination++;
    }

    while (right < end) {
        auxiliary[destination] = values[right];
        right++;
        destination++;
    }

    for (size_t index = begin; index < end; index++) {
        values[index] = auxiliary[index];
    }
}

static void merge_sort_range(
    int values[],
    int auxiliary[],
    size_t begin,
    size_t end
) {
    if (end - begin < 2U) {
        return;
    }

    const size_t middle = (begin + end) / 2U;
    merge_sort_range(values, auxiliary, begin, middle);
    merge_sort_range(values, auxiliary, middle, end);
    merge(values, auxiliary, begin, middle, end);
}

static bool merge_sort(int values[], size_t count) {
    if (count == 0U) {
        return true;
    }
    if (values == NULL) {
        return false;
    }
    if (count == 1U) {
        return true;
    }

    int *auxiliary = malloc(count * sizeof(*auxiliary));
    if (auxiliary == NULL) {
        return false;
    }

    merge_sort_range(values, auxiliary, 0U, count);
    free(auxiliary);
    return true;
}

#if !defined(MERGE_SORT_TEST)
static void print_array(const int values[], size_t count) {
    for (size_t index = 0U; index < count; index++) {
        printf("%d%s", values[index], index + 1U == count ? "\n" : " ");
    }
}

int main(void) {
    int values[] = {38, 27, 43, 3, 9, 82, 10};
    const size_t count = sizeof(values) / sizeof(values[0]);

    printf("Antes:  ");
    print_array(values, count);

    if (!merge_sort(values, count)) {
        fputs("Não foi possível reservar memória para a ordenação.\n", stderr);
        return EXIT_FAILURE;
    }

    printf("Depois: ");
    print_array(values, count);
    return EXIT_SUCCESS;
}
#endif
