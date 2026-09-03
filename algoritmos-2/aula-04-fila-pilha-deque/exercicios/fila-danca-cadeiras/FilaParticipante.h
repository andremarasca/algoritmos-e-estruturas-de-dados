#ifndef FILA_PARTICIPANTE_H
#define FILA_PARTICIPANTE_H

struct participante {
    int numero;
    char nome[30];
};

typedef struct fila Fila;

Fila *cria_Fila(void);
void libera_Fila(Fila *fi);
int insere_Fila(Fila *fi, struct participante participante);
int remove_Fila(Fila *fi);
int consulta_Fila(Fila *fi, struct participante *participante);
int tamanho_Fila(Fila *fi);

#endif
