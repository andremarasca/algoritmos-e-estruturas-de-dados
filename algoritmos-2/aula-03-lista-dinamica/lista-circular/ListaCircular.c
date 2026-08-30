#include <stdio.h>
#include <stdlib.h>

#include "ListaCircular.h"

struct elemento {
    struct aluno dados;
    struct elemento *prox;
};

typedef struct elemento Elem;

/*
 * O descritor permite acessar as duas extremidades diretamente e armazena a
 * quantidade. Em toda lista não vazia, fim->prox deve ser igual a inicio.
 */
struct descritor {
    Elem *inicio;
    Elem *fim;
    int qtd;
};

Lista *cria_lista(void)
{
    /* O descritor existe mesmo quando a lista ainda não contém elementos. */
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
        /* Rompe o ciclo para permitir a liberação como uma cadeia linear. */
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
    /* A capacidade é limitada somente pela memória disponível para malloc. */
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
        /* O primeiro nó é, ao mesmo tempo, o início e o fim. */
        no->prox = no;
        li->inicio = no;
        li->fim = no;
    } else if (al.matricula <= li->inicio->dados.matricula) {
        /* Insere antes do início atual e reconecta o fim ao novo início. */
        no->prox = li->inicio;
        li->inicio = no;
        li->fim->prox = li->inicio;
    } else {
        /* Para no primeiro nó cuja matrícula não é menor que a nova. */
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
            /* Retornar ao início indica que a inserção ocorreu no final. */
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
        /* O novo nó aponta para o início antes de se tornar o novo fim. */
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
        /* O antigo início se torna o sucessor do novo nó. */
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

    /* O percurso circular termina depois que retorna ao início. */
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
        /* Remover o único nó restaura o estado vazio do descritor. */
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
        /* A lista simples precisa percorrer os nós para localizar o anterior. */
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
        /* Avança o início e reconecta o fim ao novo primeiro nó. */
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
    /* Processa o primeiro elemento antes de verificar o fim da volta. */
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
    /* NULL não pode encerrar o percurso de uma lista circular não vazia. */
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
