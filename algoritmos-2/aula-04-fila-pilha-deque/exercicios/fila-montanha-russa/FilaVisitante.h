#ifndef FILA_VISITANTE_H
#define FILA_VISITANTE_H

struct visitante {
    int ingresso;
    char nome[30];
};

typedef struct fila Fila;

Fila *cria_Fila(void);
void libera_Fila(Fila *fi);
int insere_Fila(Fila *fi, struct visitante visitante);
int remove_Fila(Fila *fi);
int consulta_Fila(Fila *fi, struct visitante *visitante);
int fila_vazia(Fila *fi);

#endif
