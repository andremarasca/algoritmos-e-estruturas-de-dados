#include <stdio.h>
#include <stdlib.h>

#include "ListaDinamica.h"

struct elemento {
    struct aluno dados;
    struct elemento *prox;
};

typedef struct elemento Elem;

Lista *cria_lista(void)
{
    Lista *li = (Lista *)malloc(sizeof(Lista));
    if (li != NULL) {
        *li = NULL;
    }
    return li;
}

void libera_lista(Lista *li)
{
    if (li != NULL) {
        Elem *no;
        while (*li != NULL) {
            no = *li;
            *li = (*li)->prox;
            free(no);
        }
        free(li);
    }
}

int insere_lista_ordenada(Lista *li, struct aluno al)
{
    if (li == NULL) {
        return 0;
    }

    Elem *no = (Elem *)malloc(sizeof(Elem));
    if (no == NULL) {
        return 0;
    }

    no->dados = al;
    if (*li == NULL) {
        no->prox = NULL;
        *li = no;
        return 1;
    } else {
        Elem *ant;
        Elem *atual = *li;

        while (atual != NULL
               && atual->dados.matricula < al.matricula) {
            ant = atual;
            atual = atual->prox;
        }

        if (atual == *li) {
            no->prox = *li;
            *li = no;
        } else {
            no->prox = atual;
            ant->prox = no;
        }

        return 1;
    }
}

int remove_lista(Lista *li, int mat)
{
    if (li == NULL) {
        return 0;
    }
    if (*li == NULL) {
        return 0;
    }

    Elem *ant;
    Elem *no = *li;
    while (no != NULL && no->dados.matricula != mat) {
        ant = no;
        no = no->prox;
    }

    if (no == NULL) {
        return 0;
    }

    if (no == *li) {
        *li = no->prox;
    } else {
        ant->prox = no->prox;
    }

    free(no);
    return 1;
}

void imprime_lista(Lista *li)
{
    if (li == NULL) {
        return;
    }

    Elem *no = *li;
    while (no != NULL) {
        printf("Matricula: %d\n", no->dados.matricula);
        printf("Nome: %s\n", no->dados.nome);
        printf(
            "Notas: %f %f %f\n",
            no->dados.n1,
            no->dados.n2,
            no->dados.n3
        );
        printf("------------------------------\n");
        no = no->prox;
    }
}
