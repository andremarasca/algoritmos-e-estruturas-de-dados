#ifndef PILHA_CHAR_H
#define PILHA_CHAR_H

typedef struct elemento *Pilha;

// Cria uma pilha vazia.
Pilha *cria_Pilha(void);

// Libera todos os elementos e o ponteiro da pilha.
void libera_Pilha(Pilha *pi);

// Insere um caractere no topo da pilha.
int insere_Pilha(Pilha *pi, char caractere);

// Remove o caractere localizado no topo.
int remove_Pilha(Pilha *pi);

// Copia o caractere do topo sem remove-lo.
int consulta_Pilha(Pilha *pi, char *caractere);

#endif
