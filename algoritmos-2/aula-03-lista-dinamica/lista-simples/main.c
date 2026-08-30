#include <stdio.h>
#include <stdlib.h>

#include "ListaDinamica.h"

int main(void)
{
    struct aluno aluno_32 = {32, "Ana", 8.0f, 7.5f, 9.0f};
    struct aluno aluno_45 = {45, "Bruno", 6.5f, 7.0f, 8.0f};
    struct aluno aluno_18 = {18, "Carla", 9.0f, 8.5f, 9.5f};
    struct aluno aluno_25 = {25, "Diego", 7.0f, 6.0f, 8.5f};
    Lista *li = cria_lista();

    if (li == NULL) {
        fprintf(stderr, "Nao foi possivel criar a lista.\n");
        return EXIT_FAILURE;
    }

    printf("Inserindo as matriculas 32, 45, 18 e 25.\n");

    if (insere_lista_ordenada(li, aluno_32) == 0) {
        fprintf(stderr, "Nao foi possivel inserir a matricula 32.\n");
        libera_lista(li);
        return EXIT_FAILURE;
    }

    if (insere_lista_ordenada(li, aluno_45) == 0) {
        fprintf(stderr, "Nao foi possivel inserir a matricula 45.\n");
        libera_lista(li);
        return EXIT_FAILURE;
    }

    if (insere_lista_ordenada(li, aluno_18) == 0) {
        fprintf(stderr, "Nao foi possivel inserir a matricula 18.\n");
        libera_lista(li);
        return EXIT_FAILURE;
    }

    if (insere_lista_ordenada(li, aluno_25) == 0) {
        fprintf(stderr, "Nao foi possivel inserir a matricula 25.\n");
        libera_lista(li);
        return EXIT_FAILURE;
    }

    printf("\nLista ordenada apos as insercoes:\n");
    imprime_lista(li);

    printf("Removendo a matricula 32.\n");
    if (remove_lista(li, 32) == 0) {
        fprintf(stderr, "Nao foi possivel remover a matricula 32.\n");
        libera_lista(li);
        return EXIT_FAILURE;
    }

    printf("Removendo a matricula 18.\n");
    if (remove_lista(li, 18) == 0) {
        fprintf(stderr, "Nao foi possivel remover a matricula 18.\n");
        libera_lista(li);
        return EXIT_FAILURE;
    }

    printf("\nLista apos as remocoes:\n");
    imprime_lista(li);

    printf("Tentando remover a matricula inexistente 99.\n");
    if (remove_lista(li, 99) == 0) {
        printf("A matricula 99 nao foi encontrada.\n");
    }

    libera_lista(li);
    return EXIT_SUCCESS;
}
