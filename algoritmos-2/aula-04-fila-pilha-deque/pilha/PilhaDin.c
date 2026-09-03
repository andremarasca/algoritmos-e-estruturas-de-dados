// Implementa uma pilha dinamica simplesmente encadeada.
#include <stdio.h>
#include <stdlib.h>

#include "PilhaDin.h" // inclui os protótipos

// Define o nó usado para encadear os elementos da pilha.
struct elemento{
    struct aluno dados;
    struct elemento *prox;
};
typedef struct elemento Elem;

// Inicializa o ponteiro do topo para representar uma pilha vazia.
Pilha* cria_Pilha(void){
    Pilha* pi = (Pilha*) malloc(sizeof(Pilha));
    if(pi != NULL)
        *pi = NULL;
    return pi;
}

// Percorre a pilha pelo topo, libera cada no e, por ultimo, seu ponteiro.
void libera_Pilha(Pilha* pi){
    if(pi != NULL){
        Elem* no;
        while((*pi) != NULL){
            no = *pi;
            *pi = (*pi)->prox;
            free(no);
        }
        free(pi);
    }
}

// Empilha o aluno ao transformar o novo no no topo da estrutura.
int insere_Pilha(Pilha* pi, struct aluno al){
    if(pi == NULL)
        return 0;
    Elem* no;
    no = (Elem*) malloc(sizeof(Elem));
    if(no == NULL)
        return 0;
    no->dados = al;
    no->prox = (*pi);
    *pi = no;
    return 1;
}

// Desempilha o aluno do topo e faz o topo apontar para o proximo no.
int remove_Pilha(Pilha* pi){
    if(pi == NULL)
        return 0;
    if((*pi) == NULL)
        return 0;
    Elem *no = *pi;
    *pi = no->prox;
    free(no);
    return 1;
}

// Mostra a ordem em que os alunos serao removidos da pilha.
void imprime_Pilha(Pilha* pi){
    if(pi == NULL)
        return;
    Elem* no = *pi;
    while(no != NULL){
        printf("Matricula: %d\n", no->dados.matricula);
        printf("Nome: %s\n", no->dados.nome);
        printf("Notas: %.1f %.1f %.1f\n", no->dados.n1, no->dados.n2, no->dados.n3);
        printf("------------------------------\n");
        no = no->prox;
    }
}

// Consulta o aluno do topo sem alterar a pilha.
int consulta_Pilha(Pilha* pi, struct aluno* al){
    if(pi == NULL || *pi == NULL || al == NULL)
        return 0;
    *al = (*pi)->dados;
    return 1;
}
