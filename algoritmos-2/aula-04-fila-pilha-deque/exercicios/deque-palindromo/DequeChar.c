#include <stdlib.h>

#include "DequeChar.h"

struct elemento {
    char caractere;
    struct elemento *ant;
    struct elemento *prox;
};

typedef struct elemento Elem;

struct deque {
    Elem *inicio;
    Elem *fim;
    int qtd;
};

Deque *cria_Deque(void)
{
    Deque *de = malloc(sizeof(Deque));
    if (de != NULL) {
        de->inicio = NULL;
        de->fim = NULL;
        de->qtd = 0;
    }
    return de;
}

void libera_Deque(Deque *de)
{
    if (de != NULL) {
        while (de->inicio != NULL) {
            remove_inicio_Deque(de);
        }
        free(de);
    }
}

int insere_final_Deque(Deque *de, char caractere)
{
    if (de == NULL) {
        return 0;
    }

    Elem *no = malloc(sizeof(Elem));
    if (no == NULL) {
        return 0;
    }

    no->caractere = caractere;
    no->ant = de->fim;
    no->prox = NULL;
    if (de->fim == NULL) {
        de->inicio = no;
    } else {
        de->fim->prox = no;
    }
    de->fim = no;
    de->qtd++;
    return 1;
}

int remove_inicio_Deque(Deque *de)
{
    if (de == NULL || de->inicio == NULL) {
        return 0;
    }

    Elem *no = de->inicio;
    de->inicio = no->prox;
    if (de->inicio == NULL) {
        de->fim = NULL;
    } else {
        de->inicio->ant = NULL;
    }
    free(no);
    de->qtd--;
    return 1;
}

int remove_final_Deque(Deque *de)
{
    if (de == NULL || de->fim == NULL) {
        return 0;
    }

    Elem *no = de->fim;
    de->fim = no->ant;
    if (de->fim == NULL) {
        de->inicio = NULL;
    } else {
        de->fim->prox = NULL;
    }
    free(no);
    de->qtd--;
    return 1;
}

int consulta_inicio_Deque(Deque *de, char *caractere)
{
    if (de == NULL || de->inicio == NULL || caractere == NULL) {
        return 0;
    }

    *caractere = de->inicio->caractere;
    return 1;
}

int consulta_final_Deque(Deque *de, char *caractere)
{
    if (de == NULL || de->fim == NULL || caractere == NULL) {
        return 0;
    }

    *caractere = de->fim->caractere;
    return 1;
}

int tamanho_Deque(Deque *de)
{
    return de == NULL ? 0 : de->qtd;
}
