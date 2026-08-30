#ifndef LISTA_DINAMICA_H
#define LISTA_DINAMICA_H

struct aluno {
    int matricula;
    char nome[30];
    float n1;
    float n2;
    float n3;
};

typedef struct elemento *Lista;

Lista *cria_lista(void);
void libera_lista(Lista *li);
int insere_lista_ordenada(Lista *li, struct aluno al);
int remove_lista(Lista *li, int mat);
void imprime_lista(Lista *li);

#endif
