/*
 * Exercise base for brute-force and greedy activity selection.
 *
 * Complete select_brute_force and select_greedy. The remaining functions define
 * the data model, validation, comparison rules, demonstration, and automated checks.
 *
 * Compilation:
 * clang -std=c17 -Wall -Wextra -Wpedantic -Wconversion -Wshadow \
 *     -Wstrict-prototypes -Werror selecao_atividades_base.c -o selecao_atividades
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ACTIVITIES 20U

typedef struct {
    const char *name;
    int start;
    int finish;
} activity;

typedef enum {
    SELECTION_OK,
    SELECTION_INVALID_ARGUMENT,
    SELECTION_INVALID_INTERVAL,
    SELECTION_TOO_MANY_ACTIVITIES
} selection_status;

static selection_status validate_activities(const activity activities[], size_t count) {
    if (count > MAX_ACTIVITIES) {
        return SELECTION_TOO_MANY_ACTIVITIES;
    }
    if (count > 0U && activities == NULL) {
        return SELECTION_INVALID_ARGUMENT;
    }

    for (size_t index = 0U; index < count; index++) {
        if (activities[index].name == NULL) {
            return SELECTION_INVALID_ARGUMENT;
        }
        if (activities[index].start >= activities[index].finish) {
            return SELECTION_INVALID_INTERVAL;
        }
    }

    return SELECTION_OK;
}

/* Two half-open intervals are compatible when one finishes before the other starts. */
static bool are_compatible(const activity *first, const activity *second) {
    return first->finish <= second->start || second->finish <= first->start;
}

static bool subset_is_compatible(const activity activities[], size_t count, uint64_t mask) {
    for (size_t first = 0U; first < count; first++) {
        const uint64_t first_bit = UINT64_C(1) << first;
        if ((mask & first_bit) == 0U) {
            continue;
        }

        for (size_t second = first + 1U; second < count; second++) {
            const uint64_t second_bit = UINT64_C(1) << second;
            if ((mask & second_bit) != 0U) {
                /* TODO: reject the subset when this selected pair overlaps. */
                (void)are_compatible(&activities[first], &activities[second]);
            }
        }
    }

    return true;
}

static size_t count_selected_bits(uint64_t mask) {
    size_t count = 0U;

    while (mask != 0U) {
        count += (size_t)(mask & UINT64_C(1));
        mask >>= 1U;
    }

    return count;
}

static size_t copy_subset(const activity activities[], size_t count, uint64_t mask,
                          activity selected[]) {
    size_t selected_count = 0U;

    for (size_t index = 0U; index < count; index++) {
        const uint64_t bit = UINT64_C(1) << index;
        if ((mask & bit) != 0U) {
            selected[selected_count] = activities[index];
            selected_count++;
        }
    }

    return selected_count;
}

/* The output array must have room for count activities. */
static selection_status select_brute_force(const activity activities[], size_t count,
                                           activity selected[], size_t *selected_count) {
    if (selected_count == NULL || (count > 0U && selected == NULL)) {
        return SELECTION_INVALID_ARGUMENT;
    }

    *selected_count = 0U;
    const selection_status status = validate_activities(activities, count);
    if (status != SELECTION_OK || count == 0U) {
        return status;
    }

    const uint64_t subset_count = UINT64_C(1) << count;
    size_t best_count = 0U;
    uint64_t best_mask = 0U;

    for (uint64_t mask = 0U; mask < subset_count; mask++) {
        if (!subset_is_compatible(activities, count, mask)) {
            continue;
        }

        const size_t candidate_count = count_selected_bits(mask);

        /* TODO: keep mask when this candidate is better than the current best. */
        (void)candidate_count;
    }

    *selected_count = copy_subset(activities, count, best_mask, selected);
    (void)best_count;
    return SELECTION_OK;
}

static int compare_by_finish(const void *left, const void *right) {
    const activity *first = left;
    const activity *second = right;

    if (first->finish != second->finish) {
        return first->finish < second->finish ? -1 : 1;
    }
    if (first->start != second->start) {
        return first->start < second->start ? -1 : 1;
    }
    return strcmp(first->name, second->name);
}

/* The output array must have room for count activities. */
static selection_status select_greedy(const activity activities[], size_t count,
                                      activity selected[], size_t *selected_count) {
    if (selected_count == NULL || (count > 0U && selected == NULL)) {
        return SELECTION_INVALID_ARGUMENT;
    }

    *selected_count = 0U;
    const selection_status status = validate_activities(activities, count);
    if (status != SELECTION_OK || count == 0U) {
        return status;
    }

    activity sorted[MAX_ACTIVITIES];
    for (size_t index = 0U; index < count; index++) {
        sorted[index] = activities[index];
    }
    qsort(sorted, count, sizeof(sorted[0]), compare_by_finish);

    /* TODO: select compatible activities in finish-time order. */
    return SELECTION_OK;
}

static bool selection_is_compatible(const activity selected[], size_t count) {
    for (size_t first = 0U; first < count; first++) {
        for (size_t second = first + 1U; second < count; second++) {
            if (!are_compatible(&selected[first], &selected[second])) {
                return false;
            }
        }
    }

    return true;
}

static void print_selection(const char *label, const activity selected[], size_t count) {
    printf("%s (%zu):", label, count);
    for (size_t index = 0U; index < count; index++) {
        printf(" %s", selected[index].name);
    }
    putchar('\n');
}

static bool run_valid_test(const char *name, const activity activities[], size_t count,
                           size_t expected_count) {
    activity brute_selected[MAX_ACTIVITIES];
    activity greedy_selected[MAX_ACTIVITIES];
    size_t brute_count = 0U;
    size_t greedy_count = 0U;

    const selection_status brute_status =
        select_brute_force(activities, count, brute_selected, &brute_count);
    const selection_status greedy_status =
        select_greedy(activities, count, greedy_selected, &greedy_count);

    const bool passed = brute_status == SELECTION_OK && greedy_status == SELECTION_OK &&
                        brute_count == expected_count && greedy_count == expected_count &&
                        selection_is_compatible(brute_selected, brute_count) &&
                        selection_is_compatible(greedy_selected, greedy_count);

    if (!passed) {
        fprintf(stderr, "Test failed: %s\n", name);
    }
    return passed;
}

static bool run_invalid_interval_test(void) {
    const activity invalid[] = {{"Invalid", 4, 4}};
    activity selected[MAX_ACTIVITIES];
    size_t selected_count = 0U;

    const selection_status brute_status =
        select_brute_force(invalid, 1U, selected, &selected_count);
    const selection_status greedy_status = select_greedy(invalid, 1U, selected, &selected_count);
    const bool passed =
        brute_status == SELECTION_INVALID_INTERVAL && greedy_status == SELECTION_INVALID_INTERVAL;

    if (!passed) {
        fputs("Test failed: invalid interval\n", stderr);
    }
    return passed;
}

static bool run_activity_limit_test(void) {
    activity too_many[MAX_ACTIVITIES + 1U];
    activity selected[MAX_ACTIVITIES];
    size_t selected_count = 0U;

    for (size_t index = 0U; index < MAX_ACTIVITIES + 1U; index++) {
        too_many[index] = (activity){"T", 0, 1};
    }

    const selection_status brute_status =
        select_brute_force(too_many, MAX_ACTIVITIES + 1U, selected, &selected_count);
    const selection_status greedy_status =
        select_greedy(too_many, MAX_ACTIVITIES + 1U, selected, &selected_count);
    const bool passed = brute_status == SELECTION_TOO_MANY_ACTIVITIES &&
                        greedy_status == SELECTION_TOO_MANY_ACTIVITIES;

    if (!passed) {
        fputs("Test failed: activity limit\n", stderr);
    }
    return passed;
}

int main(void) {
    const activity schedule[] = {{"K", 5, 9}, {"L", 1, 3}, {"M", 3, 5},
                                 {"N", 0, 7}, {"O", 5, 7}, {"P", 8, 10}};
    const size_t schedule_count = sizeof(schedule) / sizeof(schedule[0]);
    activity brute_selected[MAX_ACTIVITIES];
    activity greedy_selected[MAX_ACTIVITIES];
    size_t brute_count = 0U;
    size_t greedy_count = 0U;

    const selection_status brute_status =
        select_brute_force(schedule, schedule_count, brute_selected, &brute_count);
    const selection_status greedy_status =
        select_greedy(schedule, schedule_count, greedy_selected, &greedy_count);
    if (brute_status != SELECTION_OK || greedy_status != SELECTION_OK) {
        fputs("Could not process the demonstration.\n", stderr);
        return EXIT_FAILURE;
    }

    print_selection("Brute force", brute_selected, brute_count);
    print_selection("Greedy", greedy_selected, greedy_count);

    const activity single[] = {{"Only", 2, 5}};
    const activity touching[] = {{"T1", 0, 2}, {"T2", 2, 4}, {"T3", 4, 6}};
    const activity overlapping[] = {{"X1", 0, 5}, {"X2", 1, 4}, {"X3", 2, 3}};

    size_t passed_count = 0U;
    passed_count += run_valid_test("normal", schedule, schedule_count, 4U) ? 1U : 0U;
    passed_count += run_valid_test("empty", NULL, 0U, 0U) ? 1U : 0U;
    passed_count += run_valid_test("single", single, 1U, 1U) ? 1U : 0U;
    passed_count += run_valid_test("touching", touching, 3U, 3U) ? 1U : 0U;
    passed_count += run_valid_test("overlapping", overlapping, 3U, 1U) ? 1U : 0U;
    passed_count += run_activity_limit_test() ? 1U : 0U;
    passed_count += run_invalid_interval_test() ? 1U : 0U;

    const size_t test_count = 7U;
    printf("Tests: %zu/%zu passed\n", passed_count, test_count);
    return passed_count == test_count ? EXIT_SUCCESS : EXIT_FAILURE;
}
