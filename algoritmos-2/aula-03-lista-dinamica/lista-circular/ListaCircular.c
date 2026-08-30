#include <stdio.h>
#include <stdlib.h>

#include "ListaCircular.h"

struct elemento {
    struct aluno dados;
    struct elemento *prox;
};

typedef struct elemento Elem;

struct descritor {
    Elem *inicio;
    Elem *fim;
    int qtd;
};

Lista *cria_lista(void)
{
    Lista *li = (Lista *)malloc(sizeof(Lista));
    if (li != NULL) {
        li->inicio = NULL;
        li->fim = NULL;
        li->qtd = 0;
    }
    return li;
}

void libera_lista(Lista *li)
{
    if (li != NULL) {
        if (li->fim != NULL) {
            li->fim->prox = NULL;
        }

        while (li->inicio != NULL) {
            Elem *no = li->inicio;
            li->inicio = li->inicio->prox;
            free(no);
        }
        free(li);
    }
}

int lista_cheia(Lista *li)
{
    (void)li;
    return 0;
}

int lista_vazia(Lista *li)
{
    if (li == NULL) {
        return 1;
    }
    return li->qtd == 0;
}

int tamanho_lista(Lista *li)
{
    if (li == NULL) {
        return 0;
    }
    return li->qtd;
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

    if (li->inicio == NULL) {
        no->prox = no;
        li->inicio = no;
        li->fim = no;
    } else if (al.matricula <= li->inicio->dados.matricula) {
        no->prox = li->inicio;
        li->inicio = no;
        li->fim->prox = li->inicio;
    } else {
        Elem *ant = li->inicio;
        Elem *atual = li->inicio->prox;

        while (atual != li->inicio
               && atual->dados.matricula < al.matricula) {
            ant = atual;
            atual = atual->prox;
        }

        no->prox = atual;
        ant->prox = no;
        if (atual == li->inicio) {
            li->fim = no;
        }
    }

    li->qtd++;
    return 1;
}

int insere_lista_final(Lista *li, struct aluno al)
{
    if (li == NULL) {
        return 0;
    }

    Elem *no = (Elem *)malloc(sizeof(Elem));
    if (no == NULL) {
        return 0;
    }
    no->dados = al;

    if (li->inicio == NULL) {
        no->prox = no;
        li->inicio = no;
        li->fim = no;
    } else {
        no->prox = li->inicio;
        li->fim->prox = no;
        li->fim = no;
    }

    li->qtd++;
    return 1;
}

int insere_lista_inicio(Lista *li, struct aluno al)
{
    if (li == NULL) {
        return 0;
    }

    Elem *no = (Elem *)malloc(sizeof(Elem));
    if (no == NULL) {
        return 0;
    }
    no->dados = al;

    if (li->inicio == NULL) {
        no->prox = no;
        li->inicio = no;
        li->fim = no;
    } else {
        no->prox = li->inicio;
        li->inicio = no;
        li->fim->prox = li->inicio;
    }

    li->qtd++;
    return 1;
}

int remove_lista(Lista *li, int mat)
{
    if (li == NULL || li->inicio == NULL) {
        return 0;
    }

    Elem *ant = li->fim;
    Elem *no = li->inicio;

    do {
        if (no->dados.matricula == mat) {
            break;
        }
        ant = no;
        no = no->prox;
    } while (no != li->inicio);

    if (no->dados.matricula != mat) {
        return 0;
    }

    if (li->qtd == 1) {
        li->inicio = NULL;
        li->fim = NULL;
    } else {
        ant->prox = no->prox;
        if (no == li->inicio) {
            li->inicio = no->prox;
        }
        if (no == li->fim) {
            li->fim = ant;
        }
        li->fim->prox = li->inicio;
    }

    free(no);
    li->qtd--;
    return 1;
}

int remove_lista_final(Lista *li)
{
    if (li == NULL || li->inicio == NULL) {
        return 0;
    }

    Elem *no = li->fim;
    if (li->qtd == 1) {
        li->inicio = NULL;
        li->fim = NULL;
    } else {
        Elem *ant = li->inicio;
        while (ant->prox != li->fim) {
            ant = ant->prox;
        }
        li->fim = ant;
        li->fim->prox = li->inicio;
    }

    free(no);
    li->qtd--;
    return 1;
}

int remove_lista_inicio(Lista *li)
{
    if (li == NULL || li->inicio == NULL) {
        return 0;
    }

    Elem *no = li->inicio;
    if (li->qtd == 1) {
        li->inicio = NULL;
        li->fim = NULL;
    } else {
        li->inicio = no->prox;
        li->fim->prox = li->inicio;
    }

    free(no);
    li->qtd--;
    return 1;
}

int consulta_lista(Lista *li, int mat, struct aluno *al)
{
    if (li == NULL || li->inicio == NULL || al == NULL) {
        return 0;
    }

    Elem *no = li->inicio;
    do {
        if (no->dados.matricula == mat) {
            *al = no->dados;
            return 1;
        }
        no = no->prox;
    } while (no != li->inicio);

    return 0;
}

void imprime_lista(Lista *li)
{
    if (li == NULL || li->inicio == NULL) {
        return;
    }

    Elem *no = li->inicio;
    do {
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
    } while (no != li->inicio);
}
