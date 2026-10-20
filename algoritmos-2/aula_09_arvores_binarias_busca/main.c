/*
 * Demonstração completa da aula 09, mantendo os dados dos slides.
 * Compilar e executar nesta pasta:
 *   gcc -std=c17 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wstrict-prototypes -Werror ArvoreBinaria.c main.c -o aula_09
 *   ./aula_09
 * Testes com CMake:
 *   cmake -S . -B build
 *   cmake --build build
 *   ctest --test-dir build --output-on-failure
 */
#include <stdio.h>
#include <stdlib.h>

#include "ArvoreBinaria.h"

/* Insere uma sequência e informa falha ao chamador, que continua dono da árvore. */
static int insere_sequencia(ArvBin* raiz, const int valores[], int quantidade) {
    /* A ordem de inserção determina a forma da ABB antes de qualquer rebalanceamento. */
    for (int i = 0; i < quantidade; i++) {
        /* A demonstração usa chaves únicas, portanto zero indica uma falha. */
        if (!insere_ArvBin(raiz, valores[i])) {
            fputs("Não foi possível inserir a sequência.\n", stderr);
            return 0;
        }
    }
    return 1;
}

/* Compara os três percursos e apresenta as medidas da árvore atual. */
static void mostra_ArvBin(ArvBin* raiz) {
    printf("Nós: %d | Altura em níveis: %d | Balanceada: %d\n",
           totalNO_ArvBin(raiz), altura_ArvBin(raiz), balanceada_ArvBin(raiz));
    puts("Pré-Ordem:");
    preOrdem_ArvBin(raiz);
    puts("Em-Ordem:");
    emOrdem_ArvBin(raiz);
    puts("Pós-Ordem:");
    posOrdem_ArvBin(raiz);
}

/* Reproduz inserção, buscas e os três casos independentes de remoção dos slides. */
static int demonstra_operacoes(void) {
    /* A declaração abaixo é a mesma apresentada no slide 6. */
    ArvBin* raiz = cria_ArvBin();

    /* A caixa precisa existir antes de qualquer operação sobre sua referência interna. */
    if (raiz == NULL) {
        fputs("Não foi possível criar a árvore.\n", stderr);
        return 0;
    }

    const int valores[] = {4, 9, 1, 3};
    /* Desfaz a árvore parcial se uma inserção não puder ser concluída. */
    if (!insere_sequencia(raiz, valores, 4)) {
        libera_ArvBin(raiz);
        return 0;
    }

    puts("=== Inserção e busca: árvore dos slides ===");
    printf("Nova tentativa de inserir 1: %d (0 = duplicata)\n", insere_ArvBin(raiz, 1));
    /* O operador ?: escolhe a primeira mensagem quando a comparação é verdadeira. */
    printf("Busca por 1: %s\n", busca_ArvBin(raiz, 1) != NULL ? "encontrado" : "ausente");
    printf("Busca por 2: %s\n", busca_ArvBin(raiz, 2) != NULL ? "encontrado" : "ausente");
    mostra_ArvBin(raiz);
    libera_ArvBin(raiz);
    raiz = NULL;

    const int remover[] = {3, 1, 4};
    /* Cada caso reinicia a árvore para reproduzir exatamente os estados dos slides. */
    for (int i = 0; i < 3; i++) {
        raiz = cria_ArvBin();
        /* A caixa deve existir antes de tentar preencher o novo caso. */
        if (raiz == NULL) {
            return 0;
        }
        /* A liberação também cobre uma sequência interrompida por falta de memória. */
        if (!insere_sequencia(raiz, valores, 4)) {
            libera_ArvBin(raiz);
            return 0;
        }
        printf("\n=== Remoção independente da chave %d ===\n", remover[i]);
        /* Todos os alvos existem no estado inicial de cada demonstração. */
        if (!remove_ArvBin(raiz, remover[i])) {
            libera_ArvBin(raiz);
            return 0;
        }
        mostra_ArvBin(raiz);
        libera_ArvBin(raiz);
        raiz = NULL;
    }
    return 1;
}

/* Executa os exercícios de inserção e rebalanceamento com as sequências do PDF. */
static int demonstra_sequencia(const int valores[], int quantidade, const char* titulo) {
    ArvBin* raiz = cria_ArvBin();
    /* Sem a caixa não é possível armazenar a árvore da demonstração. */
    if (raiz == NULL) {
        return 0;
    }
    /* Interrompe e libera qualquer prefixo criado caso a sequência não seja concluída. */
    if (!insere_sequencia(raiz, valores, quantidade)) {
        libera_ArvBin(raiz);
        return 0;
    }

    printf("\n=== %s ===\n", titulo);
    mostra_ArvBin(raiz);
    int deslocamentos = 0;
    /* As duas sequências do slide 126 contêm 11, permitindo comparar o custo da busca. */
    if (consulta_deslocamentos_ArvBin(raiz, 11, &deslocamentos)) {
        printf("Deslocamentos por arestas para encontrar 11: %d\n", deslocamentos);
    }
    puts("Depois do rebalanceamento pelos meios do vetor:");
    /* A função mantém a original intacta se faltar memória para a reconstrução. */
    if (!rebalanceia_ArvBin(raiz)) {
        libera_ArvBin(raiz);
        return 0;
    }
    mostra_ArvBin(raiz);
    libera_ArvBin(raiz);
    raiz = NULL;
    return 1;
}

/* Percorre os exemplos da aula sem entrada interativa nem dependências externas. */
int main(void) {
    const int insercao[] = {4, 14, 8, 3, 15, 16, 2, 23, 42};
    const int primeira[] = {6, 3, 2, 1, 4, 5, 9, 8, 7, 10, 11};
    const int crescente[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    const int reconstruir[] = {2, 3, 4, 6, 5, 7};

    /* Cada etapa administra sua própria árvore e comunica apenas sucesso ou falha. */
    if (!demonstra_operacoes() ||
        !demonstra_sequencia(insercao, 9, "Inserções do slide 12") ||
        !demonstra_sequencia(primeira, 11, "Primeira sequência do slide 126") ||
        !demonstra_sequencia(crescente, 11, "Sequência crescente dos slides 126 e 257") ||
        !demonstra_sequencia(reconstruir, 6, "Reconstrução dos slides 233 a 256")) {
        fputs("Demonstração interrompida.\n", stderr);
        return EXIT_FAILURE;
    }

    puts("\nTodas as árvores da demonstração foram liberadas.");
    return EXIT_SUCCESS;
}
