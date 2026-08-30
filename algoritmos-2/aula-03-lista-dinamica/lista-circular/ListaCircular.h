#ifndef LISTA_CIRCULAR_H
#define LISTA_CIRCULAR_H

struct aluno {
    int matricula;
    char nome[30];
    float n1;
    float n2;
    float n3;
};

typedef struct descritor Lista;

Lista *cria_lista(void);
void libera_lista(Lista *li);
int lista_cheia(Lista *li);
int lista_vazia(Lista *li);
int tamanho_lista(Lista *li);
int insere_lista_ordenada(Lista *li, struct aluno al);
int insere_lista_final(Lista *li, struct aluno al);
int insere_lista_inicio(Lista *li, struct aluno al);
int remove_lista(Lista *li, int mat);
int remove_lista_final(Lista *li);
int remove_lista_inicio(Lista *li);
int consulta_lista(Lista *li, int mat, struct aluno *al);
void imprime_lista(Lista *li);

#endif
