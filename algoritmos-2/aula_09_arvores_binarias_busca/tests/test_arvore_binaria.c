/*
 * Testes unitários sem bibliotecas externas.
 * A implementação é incluída apenas neste executável para inspecionar invariantes
 * e substituir malloc, free e printf sem alterar os trechos preservados do PDF.
 * Cada teste verifica também que todas as alocações pertencentes a ele foram liberadas.
 */
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Guarda os blocos vivos, a saída capturada e o ponto escolhido para falhar a alocação. */
static void* blocks[1024];
static size_t live_blocks;
static int fail_after = -1;
static char output[16384];
static size_t output_length;
static unsigned int checks;
static unsigned int tests;

/* Encerra com diagnóstico quando uma condição não cumpre o comportamento esperado. */
static void check(int condition, const char* expression, int line) {
    checks++;
    /* Uma falha precisa continuar ativa mesmo em compilações com NDEBUG. */
    if (!condition) {
        fprintf(stderr, "Falha na linha %d: %s\n", line, expression);
        exit(EXIT_FAILURE);
    }
}

/* # transforma a expressão em texto; __LINE__ informa a linha que chamou a verificação. */
#define CHECK(expression) check((expression), #expression, __LINE__)

/* Simula falta de memória e registra cada alocação real feita pela implementação. */
static void* test_malloc(size_t size) {
    /* O contador zero representa exatamente o ponto de falha solicitado pelo teste. */
    if (fail_after == 0) {
        return NULL;
    }
    /* Um contador positivo adia a falha até concluir as alocações permitidas. */
    if (fail_after > 0) {
        fail_after--;
    }

    void* block = malloc(size);
    CHECK(block != NULL);
    CHECK(live_blocks < sizeof(blocks) / sizeof(blocks[0]));
    blocks[live_blocks++] = block;
    return block;
}

/* Detecta dupla liberação e blocos desconhecidos antes de chamar free de verdade. */
static void test_free(void* block) {
    /* A biblioteca C permite liberar NULL sem produzir efeitos. */
    if (block == NULL) {
        return;
    }

    size_t index = 0;
    /* Localiza o bloco entre as alocações ainda pertencentes à implementação. */
    while (index < live_blocks && blocks[index] != block) {
        index++;
    }
    CHECK(index < live_blocks);
    blocks[index] = blocks[live_blocks - 1];
    live_blocks--;
    free(block);
}

/*
 * Captura as impressões reais dos três percursos para comparar suas sequências.
 * va_list percorre argumentos variáveis; vsnprintf formata esses argumentos no vetor.
 */
static int test_printf(const char* format, ...) {
    va_list arguments;
    va_start(arguments, format);
    int written = vsnprintf(output + output_length, sizeof(output) - output_length,
                            format, arguments);
    va_end(arguments);
    CHECK(written >= 0);
    CHECK((size_t)written < sizeof(output) - output_length);
    output_length += (size_t)written;
    return written;
}

/* As substituições valem somente durante a inclusão da implementação neste teste. */
#define malloc test_malloc
#define free test_free
#define printf test_printf
#include "../ArvoreBinaria.c"
#undef malloc
#undef free
#undef printf

/* Limpa a captura sem interferir nas funções de impressão da implementação. */
static void reset_output(void) {
    output[0] = '\0';
    output_length = 0;
}

/* Cria uma árvore a partir de chaves únicas e exige sucesso em todas as inserções. */
static ArvBin* create_tree(const int values[], int count) {
    ArvBin* tree = cria_ArvBin();
    CHECK(tree != NULL);
    /* A sequência de entrada é preservada para que a forma esperada seja reproduzível. */
    for (int index = 0; index < count; index++) {
        CHECK(insere_ArvBin(tree, values[index]) == 1);
    }
    return tree;
}

/* Confere limites herdados dos ancestrais, além das comparações com filhos imediatos. */
static int check_subtree(struct NO* node, int64_t lower, int64_t upper, int budget) {
    /* Uma posição vazia encerra aquele ramo e não acrescenta nós. */
    if (node == NULL) {
        return 0;
    }
    CHECK(budget > 0);
    CHECK((int64_t)node->info > lower);
    CHECK((int64_t)node->info < upper);
    int left_count = check_subtree(node->esq, lower, node->info, budget - 1);
    int right_count = check_subtree(node->dir, node->info, upper, budget - 1);
    return 1 + left_count + right_count;
}

/* Verifica ordenação global, contagem e quantidade de blocos pertencentes à árvore. */
static void check_tree(ArvBin* tree, int count) {
    CHECK(tree != NULL);
    CHECK(check_subtree(*tree, INT64_MIN, INT64_MAX, count + 1) == count);
    CHECK(totalNO_ArvBin(tree) == count);
    CHECK(live_blocks == (size_t)count + 1U);
}

/* Todas as consultas públicas aceitam uma caixa inexistente sem acessar memória. */
static void test_null_handle(void) {
    int distance = 7;
    CHECK(insere_ArvBin(NULL, 1) == 0);
    CHECK(busca_ArvBin(NULL, 1) == NULL);
    CHECK(remove_ArvBin(NULL, 1) == 0);
    CHECK(totalNO_ArvBin(NULL) == 0);
    CHECK(altura_ArvBin(NULL) == 0);
    CHECK(balanceada_ArvBin(NULL) == 0);
    CHECK(rebalanceia_ArvBin(NULL) == 0);
    CHECK(consulta_deslocamentos_ArvBin(NULL, 1, &distance) == 0);
    CHECK(distance == 7);
    preOrdem_ArvBin(NULL);
    emOrdem_ArvBin(NULL);
    posOrdem_ArvBin(NULL);
    CHECK(output_length == 0);
    libera_ArvBin(NULL);
}

/* A falha da primeira alocação não cria uma caixa nem deixa blocos vivos. */
static void test_creation_failure(void) {
    fail_after = 0;
    CHECK(cria_ArvBin() == NULL);
    fail_after = -1;
}

/* Uma caixa existente com conteúdo NULL é uma árvore válida e vazia. */
static void test_empty_tree(void) {
    ArvBin* tree = cria_ArvBin();
    check_tree(tree, 0);
    CHECK(*tree == NULL);
    CHECK(altura_ArvBin(tree) == 0);
    CHECK(balanceada_ArvBin(tree) == 1);
    CHECK(busca_ArvBin(tree, 9) == NULL);
    CHECK(remove_ArvBin(tree, 9) == 0);
    preOrdem_ArvBin(tree);
    emOrdem_ArvBin(tree);
    posOrdem_ArvBin(tree);
    CHECK(output_length == 0);
    fail_after = 0;
    CHECK(rebalanceia_ArvBin(tree) == 1);
    fail_after = -1;
    libera_ArvBin(tree);
}

/* A chave zero é um dado válido, diferente do ponteiro nulo retornado pela busca. */
static void test_single_zero_key(void) {
    const int values[] = {0};
    ArvBin* tree = create_tree(values, 1);
    CHECK(busca_ArvBin(tree, 0) == *tree);
    CHECK((*tree)->esq == NULL && (*tree)->dir == NULL);
    CHECK(altura_ArvBin(tree) == 1);
    CHECK(balanceada_ArvBin(tree) == 1);
    check_tree(tree, 1);
    libera_ArvBin(tree);
}

/* Duplicatas não modificam a raiz, os nós encontrados ou a quantidade de alocações. */
static void test_duplicate_keys(void) {
    const int values[] = {4, 9, 1, 3};
    ArvBin* tree = create_tree(values, 4);
    struct NO* root = *tree;
    struct NO* child = busca_ArvBin(tree, 1);
    CHECK(insere_ArvBin(tree, 4) == 0);
    CHECK(insere_ArvBin(tree, 1) == 0);
    CHECK(*tree == root && busca_ArvBin(tree, 1) == child);
    check_tree(tree, 4);
    libera_ArvBin(tree);
}

/* A inserção interrompida não perde a árvore que já estava criada. */
static void test_insertion_failure(void) {
    ArvBin* tree = cria_ArvBin();
    CHECK(tree != NULL);
    fail_after = 0;
    CHECK(insere_ArvBin(tree, 4) == 0);
    CHECK(*tree == NULL);
    fail_after = -1;
    CHECK(insere_ArvBin(tree, 4) == 1);
    struct NO* root = *tree;
    fail_after = 0;
    CHECK(insere_ArvBin(tree, 3) == 0);
    fail_after = -1;
    CHECK(*tree == root);
    check_tree(tree, 1);
    libera_ArvBin(tree);
}

/* Busca devolve a referência correta e termina em NULL quando a chave não existe. */
static void test_search(void) {
    const int values[] = {45, 20, 70, 30, 55};
    ArvBin* tree = create_tree(values, 5);
    CHECK(busca_ArvBin(tree, 55) == (*tree)->dir->esq);
    CHECK(busca_ArvBin(tree, 45) == *tree);
    CHECK(busca_ArvBin(tree, 25) == NULL);
    CHECK(busca_ArvBin(tree, 100) == NULL);
    check_tree(tree, 5);
    libera_ArvBin(tree);
}

/* Comparações diretas permitem chaves negativas e os dois extremos de int. */
static void test_integer_limits(void) {
    const int values[] = {0, INT_MIN, INT_MAX, -1, 1};
    ArvBin* tree = create_tree(values, 5);
    CHECK(busca_ArvBin(tree, INT_MIN) != NULL);
    CHECK(busca_ArvBin(tree, INT_MAX) != NULL);
    CHECK(rebalanceia_ArvBin(tree) == 1);
    check_tree(tree, 5);
    CHECK(remove_ArvBin(tree, INT_MIN) == 1);
    CHECK(remove_ArvBin(tree, INT_MAX) == 1);
    check_tree(tree, 3);
    libera_ArvBin(tree);
}

/* Confere a saída real das três funções, preservando a impressão de uma chave por linha. */
static void test_traversals(void) {
    const int values[] = {36, 16, 58, 28, 44, 72};
    ArvBin* tree = create_tree(values, 6);
    preOrdem_ArvBin(tree);
    CHECK(strcmp(output, "36\n16\n28\n58\n44\n72\n") == 0);
    reset_output();
    emOrdem_ArvBin(tree);
    CHECK(strcmp(output, "16\n28\n36\n44\n58\n72\n") == 0);
    reset_output();
    posOrdem_ArvBin(tree);
    CHECK(strcmp(output, "28\n16\n44\n72\n58\n36\n") == 0);
    check_tree(tree, 6);
    libera_ArvBin(tree);
}

/* Quantidade e altura precisam combinar subárvores de maneiras diferentes. */
static void test_count_and_height(void) {
    const int values[] = {24, 12, 42, 17, 15};
    ArvBin* tree = create_tree(values, 5);
    CHECK(totalNO_ArvBin(tree) == 5);
    CHECK(altura_ArvBin(tree) == 4);
    CHECK(balanceada_ArvBin(tree) == 0);
    libera_ArvBin(tree);
}

/* A remoção do único nó mantém a caixa disponível para novas inserções. */
static void test_remove_single_root(void) {
    const int values[] = {7};
    ArvBin* tree = create_tree(values, 1);
    CHECK(remove_ArvBin(tree, 7) == 1);
    CHECK(*tree == NULL);
    CHECK(remove_ArvBin(tree, 7) == 0);
    CHECK(insere_ArvBin(tree, 8) == 1);
    check_tree(tree, 1);
    libera_ArvBin(tree);
}

/* Uma folha pode ocupar tanto o vínculo esquerdo como o direito de seu pai. */
static void test_remove_leaves(void) {
    const int values[] = {30, 14, 47};
    ArvBin* tree = create_tree(values, 3);
    CHECK(remove_ArvBin(tree, 14) == 1);
    CHECK((*tree)->esq == NULL);
    check_tree(tree, 2);
    CHECK(remove_ArvBin(tree, 47) == 1);
    CHECK((*tree)->dir == NULL);
    check_tree(tree, 1);
    libera_ArvBin(tree);
}

/* Sem esquerda, a implementação do PDF promove diretamente o filho direito. */
static void test_remove_root_with_right_child(void) {
    const int values[] = {1, 3, 4};
    ArvBin* tree = create_tree(values, 3);
    struct NO* replacement = busca_ArvBin(tree, 3);
    CHECK(remove_ArvBin(tree, 1) == 1);
    CHECK(*tree == replacement && (*tree)->dir->info == 4);
    check_tree(tree, 2);
    libera_ArvBin(tree);
}

/* Com esquerda, o PDF usa seu maior nó, mesmo quando o nó removido só possui um filho. */
static void test_remove_root_with_left_subtree(void) {
    const int values[] = {52, 24, 36};
    ArvBin* tree = create_tree(values, 3);
    struct NO* replacement = busca_ArvBin(tree, 36);
    CHECK(remove_ArvBin(tree, 52) == 1);
    CHECK(*tree == replacement && (*tree)->esq->info == 24);
    CHECK((*tree)->dir == NULL);
    check_tree(tree, 2);
    libera_ArvBin(tree);
}

/* A substituição de um nó interno mantém o vínculo do pai e os descendentes restantes. */
static void test_remove_internal_single_child(void) {
    const int values[] = {40, 20, 60, 30, 35};
    ArvBin* tree = create_tree(values, 5);
    CHECK(remove_ArvBin(tree, 20) == 1);
    CHECK((*tree)->esq->info == 30 && (*tree)->esq->dir->info == 35);
    check_tree(tree, 4);
    libera_ArvBin(tree);
}

/* Um predecessor que já é filho direto precisa conservar sua própria esquerda. */
static void test_remove_direct_predecessor(void) {
    const int values[] = {20, 10, 30, 5};
    ArvBin* tree = create_tree(values, 4);
    struct NO* replacement = busca_ArvBin(tree, 10);
    CHECK(remove_ArvBin(tree, 20) == 1);
    CHECK(*tree == replacement);
    CHECK((*tree)->esq->info == 5 && (*tree)->dir->info == 30);
    check_tree(tree, 3);
    libera_ArvBin(tree);
}

/* A promoção de um predecessor profundo não pode perder seu filho esquerdo. */
static void test_remove_deep_predecessor(void) {
    const int values[] = {64, 32, 88, 16, 48, 40, 80};
    ArvBin* tree = create_tree(values, 7);
    struct NO* replacement = busca_ArvBin(tree, 48);
    struct NO* preserved = busca_ArvBin(tree, 40);
    CHECK(remove_ArvBin(tree, 64) == 1);
    CHECK(*tree == replacement);
    CHECK((*tree)->esq->dir == preserved);
    CHECK((*tree)->dir->info == 88 && (*tree)->dir->esq->info == 80);
    check_tree(tree, 6);
    libera_ArvBin(tree);
}

/* Chave ausente não modifica a raiz nem a quantidade de blocos vivos. */
static void test_remove_absent(void) {
    const int values[] = {4, 9, 1, 3};
    ArvBin* tree = create_tree(values, 4);
    struct NO* root = *tree;
    CHECK(remove_ArvBin(tree, 2) == 0);
    CHECK(*tree == root);
    check_tree(tree, 4);
    libera_ArvBin(tree);
}

/* O critério local precisa ser aplicado também abaixo de uma raiz equilibrada. */
static void test_descendant_imbalance(void) {
    const int values[] = {40, 20, 60, 10, 5, 70, 80};
    ArvBin* tree = create_tree(values, 7);
    CHECK(altura_ArvBin(&((*tree)->esq)) == altura_ArvBin(&((*tree)->dir)));
    CHECK(balanceada_ArvBin(tree) == 0);
    CHECK(rebalanceia_ArvBin(tree) == 1);
    CHECK(balanceada_ArvBin(tree) == 1);
    check_tree(tree, 7);
    libera_ArvBin(tree);
}

/* Reproduz exatamente a escolha dos meios no exemplo de seis chaves dos slides. */
static void test_rebalance_even(void) {
    const int values[] = {2, 3, 4, 6, 5, 7};
    ArvBin* tree = create_tree(values, 6);
    ArvBin* handle = tree;
    CHECK(altura_ArvBin(tree) == 5);
    CHECK(rebalanceia_ArvBin(tree) == 1);
    CHECK(tree == handle);
    CHECK((*tree)->info == 4);
    CHECK((*tree)->esq->info == 2 && (*tree)->esq->dir->info == 3);
    CHECK((*tree)->dir->info == 6);
    CHECK((*tree)->dir->esq->info == 5 && (*tree)->dir->dir->info == 7);
    CHECK(altura_ArvBin(tree) == 3 && balanceada_ArvBin(tree) == 1);
    check_tree(tree, 6);
    libera_ArvBin(tree);
}

/* Tamanhos pares, ímpares e unitários preservam as chaves e atingem a menor altura. */
static void test_rebalance_sizes(void) {
    /* Cada tamanho começa com uma cadeia e depois reconstrói duas vezes. */
    for (int count = 1; count <= 31; count++) {
        ArvBin* tree = cria_ArvBin();
        CHECK(tree != NULL);
        /* A ordem crescente produz uma altura inicial conhecida. */
        for (int value = 1; value <= count; value++) {
            CHECK(insere_ArvBin(tree, value) == 1);
        }
        CHECK(altura_ArvBin(tree) == count);
        CHECK(rebalanceia_ArvBin(tree) == 1);
        CHECK(rebalanceia_ArvBin(tree) == 1);
        int minimum_height = 0;
        int capacity = 0;
        /* Uma árvore de mais um nível comporta duas subárvores da capacidade anterior. */
        while (capacity < count) {
            capacity = 2 * capacity + 1;
            minimum_height++;
        }
        CHECK(altura_ArvBin(tree) == minimum_height);
        CHECK(balanceada_ArvBin(tree) == 1);
        /* A reconstrução deve preservar todas as chaves inseridas, sem criar outras. */
        for (int value = 1; value <= count; value++) {
            CHECK(busca_ArvBin(tree, value) != NULL);
        }
        check_tree(tree, count);
        libera_ArvBin(tree);
        CHECK(live_blocks == 0);
    }
}

/* Falha em cada alocação da reconstrução deve desfazer somente a tentativa parcial. */
static void test_rebalance_allocation_failures(void) {
    const int values[] = {2, 3, 4, 6, 5, 7};
    /* São oito alocações: vetor, caixa nova e seis nós. */
    for (int allowed = 0; allowed < 8; allowed++) {
        ArvBin* tree = create_tree(values, 6);
        struct NO* root = *tree;
        struct NO* last = busca_ArvBin(tree, 7);
        fail_after = allowed;
        CHECK(rebalanceia_ArvBin(tree) == 0);
        fail_after = -1;
        CHECK(*tree == root && busca_ArvBin(tree, 7) == last);
        CHECK(altura_ArvBin(tree) == 5);
        check_tree(tree, 6);
        libera_ArvBin(tree);
        CHECK(live_blocks == 0);
    }
}

/* Reproduz a sequência de inserções do slide 12. */
static void test_slide_insertion(void) {
    const int values[] = {4, 14, 8, 3, 15, 16, 2, 23, 42};
    ArvBin* tree = create_tree(values, 9);
    CHECK(altura_ArvBin(tree) == 6);
    emOrdem_ArvBin(tree);
    CHECK(strcmp(output, "2\n3\n4\n8\n14\n15\n16\n23\n42\n") == 0);
    check_tree(tree, 9);
    libera_ArvBin(tree);
}

/* Diferencia arestas percorridas de visitas a nós nas duas árvores do slide 126. */
static void test_search_distances(void) {
    const int first[] = {6, 3, 2, 1, 4, 5, 9, 8, 7, 10, 11};
    const int ascending[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    ArvBin* tree = create_tree(first, 11);
    int distance = -1;
    CHECK(consulta_deslocamentos_ArvBin(tree, 11, &distance) == 1 && distance == 3);
    CHECK(consulta_deslocamentos_ArvBin(tree, 6, &distance) == 1 && distance == 0);
    CHECK(consulta_deslocamentos_ArvBin(tree, 99, &distance) == 0);
    CHECK(consulta_deslocamentos_ArvBin(tree, 11, NULL) == 0);
    libera_ArvBin(tree);
    tree = create_tree(ascending, 11);
    CHECK(consulta_deslocamentos_ArvBin(tree, 11, &distance) == 1 && distance == 10);
    CHECK(rebalanceia_ArvBin(tree) == 1);
    CHECK(altura_ArvBin(tree) == 4);
    check_tree(tree, 11);
    libera_ArvBin(tree);
}

/* Compara 2.000 operações reproduzíveis com um conjunto independente de 64 chaves. */
static void test_mixed_operations(void) {
    ArvBin* tree = cria_ArvBin();
    CHECK(tree != NULL);
    int present[64] = {0};
    int count = 0;
    uint32_t state = UINT32_C(12345);
    /* A semente fixa produz a mesma sequência de operações em todas as execuções. */
    for (int step = 0; step < 2000; step++) {
        state = state * UINT32_C(1664525) + UINT32_C(1013904223);
        int index = (int)((state >> 16) % UINT32_C(64));
        int value = index - 32;
        /* Alterna inserções e remoções, incluindo duplicatas e valores ausentes. */
        if ((state & UINT32_C(1)) != 0) {
            CHECK(insere_ArvBin(tree, value) == !present[index]);
            count += !present[index];
            present[index] = 1;
        /* A remoção deve concordar com a presença registrada no modelo externo. */
        } else {
            CHECK(remove_ArvBin(tree, value) == present[index]);
            count -= present[index];
            present[index] = 0;
        }
        check_tree(tree, count);
        /* As buscas também precisam concordar com todas as chaves do conjunto. */
        for (int candidate = 0; candidate < 64; candidate++) {
            CHECK((busca_ArvBin(tree, candidate - 32) != NULL) == present[candidate]);
        }
    }
    libera_ArvBin(tree);
}

/* Recebe o endereço de uma função de teste e confere o isolamento de seus recursos. */
static void run_test(void (*test)(void), const char* name) {
    CHECK(live_blocks == 0);
    reset_output();
    fail_after = -1;
    test();
    CHECK(live_blocks == 0);
    tests++;
    printf("OK: %s\n", name);
}

/* Executa todos os casos, mantendo os testes e as demonstrações em programas separados. */
int main(void) {
    run_test(test_null_handle, "caixa inexistente");
    run_test(test_creation_failure, "falha de criação");
    run_test(test_empty_tree, "árvore vazia");
    run_test(test_single_zero_key, "nó único e chave zero");
    run_test(test_duplicate_keys, "duplicatas");
    run_test(test_insertion_failure, "falha de inserção");
    run_test(test_search, "busca presente e ausente");
    run_test(test_integer_limits, "limites de int");
    run_test(test_traversals, "três percursos");
    run_test(test_count_and_height, "contagem e altura");
    run_test(test_remove_single_root, "remoção da única raiz");
    run_test(test_remove_leaves, "remoção de folhas dos dois lados");
    run_test(test_remove_root_with_right_child, "raiz com filho direito");
    run_test(test_remove_root_with_left_subtree, "raiz com subárvore esquerda");
    run_test(test_remove_internal_single_child, "remoção interna com um filho");
    run_test(test_remove_direct_predecessor, "predecessor direto");
    run_test(test_remove_deep_predecessor, "predecessor com filho esquerdo");
    run_test(test_remove_absent, "remoção ausente");
    run_test(test_descendant_imbalance, "desequilíbrio em descendentes");
    run_test(test_rebalance_even, "reconstrução do PDF");
    run_test(test_rebalance_sizes, "reconstrução de 1 a 31 nós");
    run_test(test_rebalance_allocation_failures, "falhas em todas as alocações da reconstrução");
    run_test(test_slide_insertion, "inserções do slide 12");
    run_test(test_search_distances, "deslocamentos do slide 126");
    run_test(test_mixed_operations, "2.000 operações com modelo independente");
    printf("\n%u testes e %u verificações passaram, sem alocações pendentes.\n", tests, checks);
    return EXIT_SUCCESS;
}
