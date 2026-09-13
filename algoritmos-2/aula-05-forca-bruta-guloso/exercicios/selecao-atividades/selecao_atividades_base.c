/*
 * Seleção da maior quantidade de atividades sem sobreposição.
 *
 * Exercício 11: preencher as lacunas 11A e 11B.
 * Exercício 12: manter o exercício 11 e preencher a lacuna 12.
 * As demais funções já estão prontas. Os comentários explicam seu papel.
 *
 * Para compilar, substitua ARQUIVO.c pelo nome do arquivo usado:
 * clang -std=c17 -Wall -Wextra -Wpedantic -Wconversion -Wshadow
 *     -Wstrict-prototypes -Werror ARQUIVO.c -o selecao_atividades
 * Digite o comando acima em uma única linha. Depois, execute selecao_atividades.
 *
 * Cada atividade ocupa [start, finish). Início igual ao término de outra é permitido.
 * Força bruta: O(n² 2^n). Guloso: varredura O(n), além do custo da ordenação.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Limite didático: a força bruta duplica os subconjuntos a cada nova atividade.
// A máscara usa 64 bits, mas restringimos a entrada a 20 para limitar o trabalho.
#define MAX_ACTIVITIES 20U

/* Uma atividade: nome, instante de início e instante de término. */
typedef struct {
    const char *name; // Texto apenas consultado, sem alocação ou liberação neste programa.
    int start;        // Início incluído no intervalo.
    int finish;       // Término excluído do intervalo, sempre maior que start.
} activity;

/* O retorno informa sucesso, argumento inválido, intervalo inválido ou excesso de entrada. */
typedef enum {
    SELECTION_OK,
    SELECTION_INVALID_ARGUMENT,
    SELECTION_INVALID_INTERVAL,
    SELECTION_TOO_MANY_ACTIVITIES
} selection_status;

/* Verifica os dados antes de qualquer seleção. Uma entrada vazia é válida. */
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

/* Compatibilidade do exercício 5: uma termina antes ou no início da outra.
 * O teste funciona mesmo quando as atividades chegam fora de ordem cronológica. */
static bool are_compatible(const activity *first, const activity *second) {
    return first->finish <= second->start || second->finish <= first->start;
}

/*
 * Verifica se o subconjunto representado por mask possui algum conflito.
 * O bit de índice 0 representa activities[0], o de índice 1 representa activities[1].
 * Exemplo: 0101 seleciona as posições 0 e 2, contando da direita para a esquerda.
 * uint64_t é um inteiro sem sinal de 64 bits. UINT64_C(1) é o valor 1 desse tipo.
 * A expressão 1 << índice desloca o bit 1 até a posição da atividade.
 * mask & bit isola essa posição: zero significa ausente, diferente de zero significa presente.
 */
static bool subset_is_compatible(const activity activities[], size_t count, uint64_t mask) {
    for (size_t first = 0U; first < count; first++) {
        const uint64_t first_bit = UINT64_C(1) << first;
        // Uma atividade ausente do subconjunto não precisa participar das comparações.
        if ((mask & first_bit) == 0U) {
            continue;
        }

        // Começar em first + 1 evita comparar uma atividade consigo ou repetir um par.
        for (size_t second = first + 1U; second < count; second++) {
            const uint64_t second_bit = UINT64_C(1) << second;
            if ((mask & second_bit) != 0U) {
                /* INÍCIO DA LACUNA 11A
                 * As duas atividades pertencem ao subconjunto.
                 * Se are_compatible devolver false, devolva false para rejeitar o subconjunto.
                 * Substitua a linha provisória abaixo pela sua verificação.
                 */
                (void)are_compatible(&activities[first], &activities[second]);
                /* FIM DA LACUNA 11A */
            }
        }
    }

    return true;
}

/* Conta quantas atividades estão presentes, sem alterar a máscara recebida pelo chamador. */
static size_t count_selected_bits(uint64_t mask) {
    size_t count = 0U;

    while (mask != 0U) {
        // O bit da direita vale 0 ou 1. Após somar, deslocamos a máscara para ler o próximo.
        count += (size_t)(mask & UINT64_C(1));
        mask >>= 1U;
    }

    return count;
}

/* Converte a melhor máscara em um vetor de atividades, preservando a ordem da entrada. */
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

/*
 * Busca completa: verifica os subconjuntos e guarda o maior que não possui conflitos.
 * activities/count: vetor de entrada e quantidade de atividades.
 * selected: vetor de saída com espaço para count atividades, fornecido pelo chamador.
 * selected_count: endereço onde gravamos a quantidade escolhida.
 * Não altera a entrada nem aloca memória. Os nomes continuam pertencendo à entrada.
 */
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

    // Deslocar 1 por count posições produz 2^count, a quantidade de subconjuntos.
    const uint64_t subset_count = UINT64_C(1) << count;
    size_t best_count = 0U;  // Quantidade da melhor solução já encontrada.
    uint64_t best_mask = 0U; // Subconjunto dessa solução, inicialmente vazio.

    // A enumeração já está pronta: inclui o vazio e termina no subconjunto com todos os bits 1.
    for (uint64_t mask = 0U; mask < subset_count; mask++) {
        if (!subset_is_compatible(activities, count, mask)) {
            continue;
        }

        const size_t candidate_count = count_selected_bits(mask);

        /* INÍCIO DA LACUNA 11B
         * candidate_count é a quantidade de atividades da máscara viável atual.
         * Se ela superar best_count, atualize best_count e best_mask juntos.
         * Em empate, mantenha a solução anterior.
         * As linhas com (void) apenas evitam avisos enquanto a lacuna estiver vazia.
         * Substitua essas linhas pela sua implementação.
         */
        (void)candidate_count;
        (void)best_count;
        /* FIM DA LACUNA 11B */
    }

    // Após examinar tudo, materializamos apenas o melhor subconjunto.
    *selected_count = copy_subset(activities, count, best_mask, selected);
    return SELECTION_OK;
}

/*
 * Critério usado por qsort: retorno negativo coloca a primeira atividade antes da segunda.
 * Primeiro comparamos o término. Se empatar, o menor início vem primeiro.
 * Exemplo: [0, 7) vem antes de [5, 7). Se início e término empatarem, usamos o nome.
 * O desempate só torna a ordem previsível. O critério principal continua sendo o término.
 */
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

/*
 * Exercício 12: seleção gulosa. Recebe os mesmos parâmetros da força bruta.
 * selected deve ter espaço para count atividades, e selected_count recebe a quantidade.
 * A validação e a ordenação estão prontas. Falta decidir quais atividades aceitar.
 */
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

    // Ordenamos uma cópia para preservar os dados usados pela força bruta.
    activity sorted[MAX_ACTIVITIES];
    for (size_t index = 0U; index < count; index++) {
        sorted[index] = activities[index];
    }
    qsort(sorted, count, sizeof(sorted[0]), compare_by_finish);

    /* INÍCIO DA LACUNA 12
     * sorted já está em ordem de término. Percorra esse vetor uma única vez.
     * Use has_selection para indicar se já aceitou alguma atividade.
     * Use last_finish para guardar o término da última atividade aceita.
     * Aceite a primeira e, depois, somente as que começam em last_finish ou após ele.
     * Ao aceitar, copie para selected, aumente *selected_count e atualize os dois estados.
     * Ao rejeitar, preserve a seleção e last_finish.
     */
    /* FIM DA LACUNA 12 */
    return SELECTION_OK;
}

/* Confere os pares da resposta produzida. Ter a quantidade correta não basta: ela deve ser viável.
 */
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

/* Mostra a estratégia, a quantidade escolhida e os nomes na ordem do vetor de saída. */
static void print_selection(const char *label, const activity selected[], size_t count) {
    printf("%s (%zu):", label, count);
    for (size_t index = 0U; index < count; index++) {
        printf(" %s", selected[index].name);
    }
    putchar('\n');
}

/* Teste já pronto: executa as duas estratégias e confere quantidade e ausência de conflitos. */
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
        fprintf(stderr, "Teste falhou: %s\n", name);
    }
    return passed;
}

/* Início igual ao término não forma uma atividade válida. Ambas as estratégias devem rejeitar. */
static bool run_invalid_interval_test(void) {
    const activity invalid[] = {{"Inválida", 4, 4}};
    activity selected[MAX_ACTIVITIES];
    size_t selected_count = 0U;

    const selection_status brute_status =
        select_brute_force(invalid, 1U, selected, &selected_count);
    const selection_status greedy_status = select_greedy(invalid, 1U, selected, &selected_count);
    const bool passed =
        brute_status == SELECTION_INVALID_INTERVAL && greedy_status == SELECTION_INVALID_INTERVAL;

    if (!passed) {
        fputs("Teste falhou: intervalo inválido\n", stderr);
    }
    return passed;
}

/* Uma entrada com 21 atividades deve ser rejeitada antes de enumerar ou copiar seus dados. */
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
        fputs("Teste falhou: limite de atividades\n", stderr);
    }
    return passed;
}

/*
 * Demonstração e testes já prontos. Não é necessário alterar esta função.
 * Após o exercício 11, confira a linha Força bruta. O guloso ainda estará incompleto.
 * Após o exercício 12, as duas linhas e os sete testes devem estar corretos.
 */
int main(void) {
    // Mesma entrada do exercício 7, propositalmente fora da ordem de término.
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
        fputs("Não foi possível processar a demonstração.\n", stderr);
        return EXIT_FAILURE;
    }

    print_selection("Força bruta", brute_selected, brute_count);
    print_selection("Guloso", greedy_selected, greedy_count);

    // Casos pequenos e independentes: uma atividade, limites encostados e sobreposição total.
    const activity single[] = {{"Única", 2, 5}};
    const activity touching[] = {{"T1", 0, 2}, {"T2", 2, 4}, {"T3", 4, 6}};
    const activity overlapping[] = {{"X1", 0, 5}, {"X2", 1, 4}, {"X3", 2, 3}};

    // Cada função devolve true quando o teste passa. Somamos 1 por teste aprovado.
    size_t passed_count = 0U;
    passed_count += run_valid_test("normal", schedule, schedule_count, 4U) ? 1U : 0U;
    passed_count += run_valid_test("vazio", NULL, 0U, 0U) ? 1U : 0U;
    passed_count += run_valid_test("uma atividade", single, 1U, 1U) ? 1U : 0U;
    passed_count += run_valid_test("limites encostados", touching, 3U, 3U) ? 1U : 0U;
    passed_count += run_valid_test("sobreposição total", overlapping, 3U, 1U) ? 1U : 0U;
    passed_count += run_activity_limit_test() ? 1U : 0U;
    passed_count += run_invalid_interval_test() ? 1U : 0U;

    const size_t test_count = 7U;
    printf("Testes: %zu/%zu aprovados\n", passed_count, test_count);
    return passed_count == test_count ? EXIT_SUCCESS : EXIT_FAILURE;
}
