#include <stdlib.h>

#include "FilaVisitante.h"

struct elemento {
    struct visitante dados;
    struct elemento *prox;
};

typedef struct elemento Elem;

struct fila {
    Elem *inicio;
    Elem *fim;
    int qtd;
};

Fila *cria_Fila(void)
{
    Fila *fi = malloc(sizeof(Fila));
    if (fi != NULL) {
        fi->inicio = NULL;
        fi->fim = NULL;
        fi->qtd = 0;
    }
    return fi;
}

void libera_Fila(Fila *fi)
{
    if (fi != NULL) {
        while (fi->inicio != NULL) {
            remove_Fila(fi);
        }
        free(fi);
    }
}

int insere_Fila(Fila *fi, struct visitante visitante)
{
    if (fi == NULL) {
        return 0;
    }

    Elem *no = malloc(sizeof(Elem));
    if (no == NULL) {
        return 0;
    }

    no->dados = visitante;
    no->prox = NULL;
    if (fi->fim == NULL) {
        fi->inicio = no;
    } else {
        fi->fim->prox = no;
    }
    fi->fim = no;
    fi->qtd++;
    return 1;
}

int remove_Fila(Fila *fi)
{
    if (fi == NULL || fi->inicio == NULL) {
        return 0;
    }

    Elem *no = fi->inicio;
    fi->inicio = no->prox;
    if (fi->inicio == NULL) {
        fi->fim = NULL;
    }
    free(no);
    fi->qtd--;
    return 1;
}

int consulta_Fila(Fila *fi, struct visitante *visitante)
{
    if (fi == NULL || fi->inicio == NULL || visitante == NULL) {
        return 0;
    }

    *visitante = fi->inicio->dados;
    return 1;
}

int fila_vazia(Fila *fi)
{
    return fi == NULL || fi->inicio == NULL;
}
