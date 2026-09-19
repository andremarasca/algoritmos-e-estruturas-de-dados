/*
 * Troco mínimo resolvido por programação dinâmica com tabulação.
 *
 * Estado: minimum_coins[value] guarda a menor quantidade de moedas que forma
 * exatamente value.
 * Transição: minimum_coins[value] = 1 + minimum_coins[value - coin].
 * Decisão: last_coin[value] guarda o índice da moeda usada na melhor candidata.
 * Complexidade: O(amount * coin_count) em tempo e O(amount) em memória auxiliar.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Reúne a sequência de moedas reconstruída e a quantidade de elementos. */
typedef struct {
    int *coins;
    size_t count;
} coin_change_result;

/* Distingue solução, impossibilidade, entrada inválida e falha de memória. */
typedef enum {
    COIN_CHANGE_SOLVED,
    COIN_CHANGE_IMPOSSIBLE,
    COIN_CHANGE_INVALID_INPUT,
    COIN_CHANGE_NO_MEMORY
} coin_change_status;

/* Libera a sequência pertencente ao resultado e restaura seu estado vazio. */
static void coin_change_result_destroy(coin_change_result *result) {
    /* Evita acessar um resultado inexistente durante a liberação. */
    if (result != NULL) {
        free(result->coins);
        result->coins = NULL;
        result->count = 0U;
    }
}

/* Verifica se o vetor existe e contém somente valores nominais positivos. */
static bool coins_are_valid(const int coins[], size_t coin_count) {
    /* Rejeita a ausência do vetor e a ausência de tipos de moeda. */
    if (coins == NULL || coin_count == 0U) {
        return false;
    }

    /* Examina todos os tipos porque qualquer moeda não positiva quebra a redução do estado. */
    for (size_t index = 0U; index < coin_count; index++) {
        /* Garante que cada escolha produza um restante estritamente menor. */
        if (coins[index] <= 0) {
            return false;
        }
    }

    return true;
}

/*
 * Calcula o troco mínimo por tabulação e reconstrói uma solução.
 * A função aloca result->coins, que deve ser liberado por
 * coin_change_result_destroy quando o retorno for COIN_CHANGE_SOLVED.
 */
static coin_change_status solve_coin_change(const int coins[], size_t coin_count, size_t amount,
                                            coin_change_result *result) {
    /*
     * Rejeita entradas inválidas e tamanhos que poderiam estourar o cálculo dos
     * bytes usados nos dois vetores. SIZE_MAX é o maior valor de size_t.
     */
    if (!coins_are_valid(coins, coin_count) || result == NULL ||
        amount > (SIZE_MAX / sizeof(int)) - 1U || amount > (SIZE_MAX / sizeof(size_t)) - 1U) {
        return COIN_CHANGE_INVALID_INPUT;
    }

    result->coins = NULL;
    result->count = 0U;

    size_t *minimum_coins = malloc((amount + 1U) * sizeof(*minimum_coins));
    size_t *last_coin = malloc((amount + 1U) * sizeof(*last_coin));

    /* Interrompe o cálculo se qualquer tabela necessária não puder ser criada. */
    if (minimum_coins == NULL || last_coin == NULL) {
        free(minimum_coins);
        free(last_coin);
        return COIN_CHANGE_NO_MEMORY;
    }

    const size_t unreachable = SIZE_MAX;
    minimum_coins[0] = 0U;
    last_coin[0] = unreachable;

    /* Preenche os estados em ordem crescente para que cada dependência já esteja pronta. */
    for (size_t value = 1U; value <= amount; value++) {
        minimum_coins[value] = unreachable;
        last_coin[value] = unreachable;

        /* Testa cada tipo de moeda como a última escolha da solução do estado atual. */
        for (size_t coin_index = 0U; coin_index < coin_count; coin_index++) {
            const size_t coin = (size_t)coins[coin_index];

            /* Descarta moedas que não cabem e restos que não possuem solução. */
            if (coin <= value && minimum_coins[value - coin] != unreachable) {
                const size_t candidate = minimum_coins[value - coin] + 1U;

                /* Guarda somente uma candidata melhor e preserva a primeira em caso de empate. */
                if (candidate < minimum_coins[value]) {
                    minimum_coins[value] = candidate;
                    last_coin[value] = coin_index;
                }
            }
        }
    }

    /* Encerra sem reconstrução quando nenhuma combinação forma o valor solicitado. */
    if (minimum_coins[amount] == unreachable) {
        free(minimum_coins);
        free(last_coin);
        return COIN_CHANGE_IMPOSSIBLE;
    }

    result->count = minimum_coins[amount];

    /* Reserva a saída somente quando a solução contém pelo menos uma moeda. */
    if (result->count > 0U) {
        result->coins = malloc(result->count * sizeof(*result->coins));

        /* Desfaz o resultado parcial se não houver memória para a sequência de moedas. */
        if (result->coins == NULL) {
            result->count = 0U;
            free(minimum_coins);
            free(last_coin);
            return COIN_CHANGE_NO_MEMORY;
        }
    }

    size_t remaining = amount;

    /* Segue as decisões registradas até reconstruir todas as moedas da solução ótima. */
    for (size_t index = 0U; index < result->count; index++) {
        const size_t coin_index = last_coin[remaining];
        const int coin = coins[coin_index];
        result->coins[index] = coin;
        remaining -= (size_t)coin;
    }

    free(minimum_coins);
    free(last_coin);
    return COIN_CHANGE_SOLVED;
}

/* Apresenta o resultado de uma instância válida usada na demonstração. */
static int print_solution(const int coins[], size_t coin_count, size_t amount) {
    coin_change_result result = {NULL, 0U};
    const coin_change_status status = solve_coin_change(coins, coin_count, amount, &result);

    /* Informa separadamente quando o valor não pode ser formado pelas moedas disponíveis. */
    if (status == COIN_CHANGE_IMPOSSIBLE) {
        printf("Valor solicitado: %zu\nResultado: impossivel\n", amount);
        return EXIT_SUCCESS;
    }

    /* Trata entradas inválidas e falhas de memória como falhas de execução. */
    if (status != COIN_CHANGE_SOLVED) {
        fputs("Nao foi possivel calcular o troco.\n", stderr);
        return EXIT_FAILURE;
    }

    printf("Valor solicitado: %zu\nQuantidade minima: %zu\nMoedas:", amount, result.count);

    /* Exibe cada moeda na mesma ordem em que a reconstrução percorreu as decisões. */
    for (size_t index = 0U; index < result.count; index++) {
        printf(" %d", result.coins[index]);
    }

    printf("\n");
    coin_change_result_destroy(&result);
    return EXIT_SUCCESS;
}

/* Executa a instância principal usada nos exercícios da aula prática. */
int main(void) {
    const int coins[] = {3, 4};
    const size_t coin_count = sizeof(coins) / sizeof(coins[0]);
    const size_t amount = 11U;

    return print_solution(coins, coin_count, amount);
}
