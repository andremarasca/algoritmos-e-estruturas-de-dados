#ifndef LISTA_CIRCULAR_H
#define LISTA_CIRCULAR_H

struct aluno {
    int matricula;
    char nome[30];
    float n1;
    float n2;
    float n3;
};

/*
 * A definição completa do descritor permanece privada em ListaCircular.c.
 * O usuário manipula a lista somente pelas funções declaradas abaixo.
 */
typedef struct descritor Lista;

/* Cria uma lista circular vazia. */
Lista *cria_lista(void);

/* Libera todos os elementos e, por último, libera o descritor. */
void libera_lista(Lista *li);

/* A lista dinâmica não tem capacidade fixa, portanto retorna 0. */
int lista_cheia(Lista *li);

/* Retorna 1 quando a lista é nula ou não contém elementos. */
int lista_vazia(Lista *li);

/* Retorna a quantidade de elementos armazenados. */
int tamanho_lista(Lista *li);

int insere_lista_ordenada(Lista *li, struct aluno al);
int insere_lista_final(Lista *li, struct aluno al);
int insere_lista_inicio(Lista *li, struct aluno al);
int remove_lista(Lista *li, int mat);
int remove_lista_final(Lista *li);
int remove_lista_inicio(Lista *li);

/* Copia o aluno encontrado para al e retorna 1 em caso de sucesso. */
int consulta_lista(Lista *li, int mat, struct aluno *al);

/* Imprime uma volta completa pela lista circular. */
void imprime_lista(Lista *li);

#endif
