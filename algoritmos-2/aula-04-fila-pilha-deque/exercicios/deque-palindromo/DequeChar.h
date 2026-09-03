#ifndef DEQUE_CHAR_H
#define DEQUE_CHAR_H

typedef struct deque Deque;

Deque *cria_Deque(void);
void libera_Deque(Deque *de);
int insere_final_Deque(Deque *de, char caractere);
int remove_inicio_Deque(Deque *de);
int remove_final_Deque(Deque *de);
int consulta_inicio_Deque(Deque *de, char *caractere);
int consulta_final_Deque(Deque *de, char *caractere);
int tamanho_Deque(Deque *de);

#endif
