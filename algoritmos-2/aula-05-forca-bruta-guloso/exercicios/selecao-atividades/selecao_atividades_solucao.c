/*
 * Demonstra soluções por força bruta e algoritmo guloso para o problema de
 * seleção de atividades. Cada atividade ocupa [início, término), portanto
 * outra atividade pode começar exatamente quando a anterior termina.
 *
 * A força bruta examina todos os subconjuntos e custa O(n^2 * 2^n). O
 * algoritmo guloso escolhe repetidamente a atividade compatível que termina
 * primeiro e custa O(n log n), incluindo a ordenação.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* O crescimento exponencial limita a força bruta a uma demonstração pequena. */
#define MAX_ACTIVITIES 20U

/* Representa cada atividade como um intervalo associado a um identificador. */
typedef struct {
    const char *name;
    int start;
    int finish;
} activity;

/* Determina se duas atividades podem pertencer à mesma programação. */
static bool are_compatible(const activity *first, const activity *second) {
    return first->finish <= second->start || second->finish <= first->start;
}

/*
 * Rejeita um subconjunto candidato assim que encontra um par sobreposto.
 * A máscara usa um bit para cada atividade: 1 significa selecionada e 0,
 * ausente. O bit mais à direita representa activities[0].
 */
static bool subset_is_compatible(const activity activities[], size_t count, uint64_t mask) {
    /* Usa cada atividade selecionada como referência para procurar conflitos. */
    for (size_t first = 0U; first < count; first++) {
        /*
         * UINT64_C(1) representa o valor 1 como inteiro sem sinal de 64 bits.
         * O operador << desloca esse único bit 1 até a posição da atividade.
         * Por exemplo, deslocar duas posições transforma 0001 em 0100.
         */
        const uint64_t first_bit = UINT64_C(1) << first;

        /*
         * O operador & verifica o bit correspondente dentro da máscara.
         * Resultado zero indica que a atividade não pertence ao subconjunto.
         */
        if ((mask & first_bit) == 0U) {
            continue;
        }

        /* Verifica cada par uma única vez, sem comparar a atividade consigo mesma. */
        for (size_t second = first + 1U; second < count; second++) {
            const uint64_t second_bit = UINT64_C(1) << second;

            /* Um único conflito basta para invalidar todo o subconjunto. */
            if ((mask & second_bit) != 0U &&
                !are_compatible(&activities[first], &activities[second])) {
                return false;
            }
        }
    }

    return true;
}

/* Calcula o tamanho de um subconjunto representado por uma máscara binária. */
static size_t count_selected_activities(uint64_t mask) {
    size_t count = 0U;

    /*
     * Percorre a máscara da direita para a esquerda. A expressão mask & 1
     * lê o bit atual, enquanto >>= descarta esse bit e aproxima o próximo.
     */
    while (mask != 0U) {
        count += (size_t)(mask & UINT64_C(1));
        mask >>= 1U;
    }

    return count;
}

/* Materializa o subconjunto vencedor preservando a ordem original das atividades. */
static size_t copy_subset(const activity activities[], size_t count, uint64_t mask,
                          activity selected[]) {
    size_t selected_count = 0U;

    /* Percorre a entrada para reconstruir a solução indicada pela máscara. */
    for (size_t index = 0U; index < count; index++) {
        const uint64_t bit = UINT64_C(1) << index;

        /* Copia somente as atividades que pertencem ao subconjunto vencedor. */
        if ((mask & bit) != 0U) {
            selected[selected_count] = activities[index];
            selected_count++;
        }
    }

    return selected_count;
}

/* Examina todos os subconjuntos e mantém a maior solução sem sobreposições. */
static size_t select_brute_force(const activity activities[], size_t count,
                                 activity selected[]) {
    /* Deslocar o bit 1 por count posições calcula 2^count subconjuntos. */
    const uint64_t subset_count = UINT64_C(1) << count;
    size_t best_count = 0U;
    uint64_t best_mask = 0U;

    /* Enumera todas as combinações possíveis de atividades. */
    for (uint64_t mask = 0U; mask < subset_count; mask++) {
        /* Descarta combinações inviáveis antes de compará-las com a melhor solução. */
        if (!subset_is_compatible(activities, count, mask)) {
            continue;
        }

        const size_t candidate_count = count_selected_activities(mask);

        /* Mantém a maior solução viável encontrada durante a busca completa. */
        if (candidate_count > best_count) {
            best_count = candidate_count;
            best_mask = mask;
        }
    }

    return copy_subset(activities, count, best_mask, selected);
}

/* Estabelece a ordem de término crescente exigida pela escolha gulosa. */
static int compare_by_finish(const void *left, const void *right) {
    const activity *first = left;
    const activity *second = right;

    /* Prioriza o menor término, que constitui o critério central do método guloso. */
    if (first->finish != second->finish) {
        return first->finish < second->finish ? -1 : 1;
    }

    /* Resolve empates pelo início para tornar a ordenação previsível. */
    if (first->start != second->start) {
        return first->start < second->start ? -1 : 1;
    }

    /* Usa o identificador apenas como último desempate determinístico. */
    return strcmp(first->name, second->name);
}

/* Constrói a solução aceitando cada atividade compatível que termina primeiro. */
static size_t select_greedy(const activity activities[], size_t count,
                            activity selected[]) {
    activity sorted[MAX_ACTIVITIES];

    /* Cria uma visão independente que pode ser ordenada sem alterar a entrada. */
    for (size_t index = 0U; index < count; index++) {
        sorted[index] = activities[index];
    }

    /* Prepara a sequência segundo o critério que sustenta a escolha gulosa. */
    qsort(sorted, count, sizeof(sorted[0]), compare_by_finish);

    size_t selected_count = 0U;

    /* Percorre a ordem gulosa e decide localmente sobre cada atividade. */
    for (size_t index = 0U; index < count; index++) {
        const bool is_first = selected_count == 0U;
        const bool starts_after_last =
            !is_first && sorted[index].start >= selected[selected_count - 1U].finish;

        /* Aceita a atividade somente quando ela preserva a viabilidade da solução. */
        if (is_first || starts_after_last) {
            selected[selected_count] = sorted[index];
            selected_count++;
        }
    }

    return selected_count;
}

/* Apresenta a solução como uma sequência cronológica de intervalos selecionados. */
static void print_selection(const char *label, const activity selected[], size_t count) {
    printf("%s (%zu atividades):\n", label, count);

    /* Exibe todas as escolhas para permitir a comparação visual dos resultados. */
    for (size_t index = 0U; index < count; index++) {
        printf("  %s: [%d, %d)\n", selected[index].name, selected[index].start,
               selected[index].finish);
    }
}

int main(void) {
    /* A entrada fora de ordem evidencia a etapa de preparação do método guloso. */
    const activity schedule[] = {
        {"K", 5, 9},
        {"L", 1, 3},
        {"M", 3, 5},
        {"N", 0, 7},
        {"O", 5, 7},
        {"P", 8, 10},
    };
    const size_t schedule_count = sizeof(schedule) / sizeof(schedule[0]);
    activity brute_selected[MAX_ACTIVITIES];
    activity greedy_selected[MAX_ACTIVITIES];

    /* A mesma instância permite comparar diretamente as duas estratégias. */
    const size_t brute_count =
        select_brute_force(schedule, schedule_count, brute_selected);
    const size_t greedy_count = select_greedy(schedule, schedule_count, greedy_selected);

    print_selection("Força bruta", brute_selected, brute_count);
    putchar('\n');
    print_selection("Guloso", greedy_selected, greedy_count);

    return EXIT_SUCCESS;
}
