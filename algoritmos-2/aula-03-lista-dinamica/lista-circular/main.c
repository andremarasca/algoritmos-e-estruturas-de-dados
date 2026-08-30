#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "ListaCircular.h"

int main(void)
{
    /* Estes registros são reutilizados durante toda a sequência cumulativa. */
    struct aluno aluno_32 = {32, "Ana", 8.0f, 7.5f, 9.0f};
    struct aluno aluno_45 = {45, "Bruno", 6.5f, 7.0f, 8.0f};
    struct aluno aluno_18 = {18, "Carla", 9.0f, 8.5f, 9.5f};
    struct aluno aluno_25 = {25, "Diego", 7.0f, 6.0f, 8.5f};
    struct aluno aluno_encontrado;
    Lista *li = cria_lista();

    if (li == NULL) {
        fprintf(stderr, "Nao foi possivel criar a lista.\n");
        return EXIT_FAILURE;
    }

    /* Verifica o estado do descritor imediatamente após a criação. */
    assert(lista_vazia(li) == 1);
    assert(lista_cheia(li) == 0);
    assert(tamanho_lista(li) == 0);
    assert(remove_lista_inicio(li) == 0);

    printf("Lista vazia: %d\n", lista_vazia(li));
    printf("Lista cheia: %d\n", lista_cheia(li));
    printf("Tamanho inicial: %d\n\n", tamanho_lista(li));

    printf("Inserindo a matricula 18 no inicio.\n");
    if (insere_lista_inicio(li, aluno_18) == 0) {
        fprintf(stderr, "Nao foi possivel inserir a matricula 18.\n");
        libera_lista(li);
        return EXIT_FAILURE;
    }

    printf("Inserindo a matricula 32 no final.\n");
    if (insere_lista_final(li, aluno_32) == 0) {
        fprintf(stderr, "Nao foi possivel inserir a matricula 32.\n");
        libera_lista(li);
        return EXIT_FAILURE;
    }

    printf("Inserindo a matricula 45 no final.\n");
    if (insere_lista_final(li, aluno_45) == 0) {
        fprintf(stderr, "Nao foi possivel inserir a matricula 45.\n");
        libera_lista(li);
        return EXIT_FAILURE;
    }

    printf("Inserindo a matricula 25 no final.\n");
    if (insere_lista_final(li, aluno_25) == 0) {
        fprintf(stderr, "Nao foi possivel inserir a matricula 25.\n");
        libera_lista(li);
        return EXIT_FAILURE;
    }

    assert(lista_vazia(li) == 0);
    assert(tamanho_lista(li) == 4);

    printf("\nLista circular 18, 32, 45, 25:\n");
    imprime_lista(li);
    printf("Tamanho: %d\n\n", tamanho_lista(li));

    printf("Consultando a matricula 45.\n");
    if (consulta_lista(li, 45, &aluno_encontrado) == 1) {
        assert(aluno_encontrado.matricula == 45);
        printf(
            "Aluno consultado: %d - %s\n\n",
            aluno_encontrado.matricula,
            aluno_encontrado.nome
        );
    } else {
        printf("A matricula 45 nao foi encontrada.\n\n");
    }

    printf("Removendo o primeiro aluno.\n");
    if (remove_lista_inicio(li) == 0) {
        fprintf(stderr, "Nao foi possivel remover o primeiro aluno.\n");
        libera_lista(li);
        return EXIT_FAILURE;
    }

    printf("Removendo o ultimo aluno.\n");
    if (remove_lista_final(li) == 0) {
        fprintf(stderr, "Nao foi possivel remover o ultimo aluno.\n");
        libera_lista(li);
        return EXIT_FAILURE;
    }

    assert(tamanho_lista(li) == 2);

    printf("\nLista apos remover o inicio e o final:\n");
    imprime_lista(li);

    printf("Removendo a matricula 32.\n");
    if (remove_lista(li, 32) == 0) {
        fprintf(stderr, "Nao foi possivel remover a matricula 32.\n");
        libera_lista(li);
        return EXIT_FAILURE;
    }

    assert(tamanho_lista(li) == 1);

    printf("Inserindo a matricula 25 de forma ordenada.\n");
    if (insere_lista_ordenada(li, aluno_25) == 0) {
        fprintf(stderr, "Nao foi possivel inserir a matricula 25.\n");
        libera_lista(li);
        return EXIT_FAILURE;
    }

    assert(tamanho_lista(li) == 2);
    assert(consulta_lista(li, 25, &aluno_encontrado) == 1);
    assert(consulta_lista(li, 99, &aluno_encontrado) == 0);

    printf("\nLista apos remover 32 e inserir 25 ordenadamente:\n");
    imprime_lista(li);

    /* libera_lista libera todos os nós e, por último, o descritor. */
    libera_lista(li);
    return EXIT_SUCCESS;
}
