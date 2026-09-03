#include <stdlib.h>

#include "PilhaChar.h"

struct elemento {
    char caractere;
    struct elemento *prox;
};

typedef struct elemento Elem;

Pilha *cria_Pilha(void)
{
    Pilha *pi = malloc(sizeof(Pilha));
    if (pi != NULL) {
        *pi = NULL;
    }
    return pi;
}

void libera_Pilha(Pilha *pi)
{
    if (pi != NULL) {
        Elem *no;
        while (*pi != NULL) {
            no = *pi;
            *pi = (*pi)->prox;
            free(no);
        }
        free(pi);
    }
}

int insere_Pilha(Pilha *pi, char caractere)
{
    if (pi == NULL) {
        return 0;
    }

    Elem *no = malloc(sizeof(Elem));
    if (no == NULL) {
        return 0;
    }

    no->caractere = caractere;
    no->prox = *pi;
    *pi = no;
    return 1;
}

int remove_Pilha(Pilha *pi)
{
    if (pi == NULL || *pi == NULL) {
        return 0;
    }

    Elem *no = *pi;
    *pi = no->prox;
    free(no);
    return 1;
}

int consulta_Pilha(Pilha *pi, char *caractere)
{
    if (pi == NULL || *pi == NULL || caractere == NULL) {
        return 0;
    }

    *caractere = (*pi)->caractere;
    return 1;
}
