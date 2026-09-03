// Implementa uma fila dinamica simplesmente encadeada com no descritor.
#include <stdio.h>
#include <stdlib.h>

#include "FilaDin.h" // inclui os protótipos

// Definição do tipo fila.
struct elemento{
    struct aluno dados;
    struct elemento *prox;
};
typedef struct elemento Elem;

// Define o nó descritor que guarda os extremos e a quantidade de elementos.
struct fila{
    struct elemento *inicio;
    struct elemento *fim;
    int qtd;
};

// Inicializa os dois extremos e a quantidade para representar uma fila vazia.
Fila* cria_Fila(void){
    Fila* fi = (Fila*) malloc(sizeof(Fila));
    if(fi != NULL){
        fi->fim = NULL;
        fi->inicio = NULL;
        fi->qtd = 0;
    }
    return fi;
}

// Percorre a fila pelo inicio, libera cada no e, por ultimo, o descritor.
void libera_Fila(Fila* fi){
    if(fi != NULL){
        Elem* no;
        while(fi->inicio != NULL){
            no = fi->inicio;
            fi->inicio = fi->inicio->prox;
            free(no);
        }
        free(fi);
    }
}

// Enfileira o aluno no final e trata separadamente a primeira insercao.
int insere_Fila(Fila* fi, struct aluno al){
    if(fi == NULL)
        return 0;
    Elem *no = (Elem*) malloc(sizeof(Elem));
    if(no == NULL)
        return 0;
    no->dados = al;
    no->prox = NULL;
    if(fi->fim == NULL)//fila vazia
        fi->inicio = no;
    else
        fi->fim->prox = no;
    fi->fim = no;
    fi->qtd++;
    return 1;
}

// Desenfileira o primeiro aluno e corrige o ponteiro fim quando a fila fica vazia.
int remove_Fila(Fila* fi){
    if(fi == NULL)
        return 0;
    if(fi->inicio == NULL)//fila vazia
        return 0;
    Elem *no = fi->inicio;
    fi->inicio = fi->inicio->prox;
    if(fi->inicio == NULL)//fila ficou vazia
        fi->fim = NULL;
    free(no);
    fi->qtd--;
    return 1;
}

// Mostra a ordem em que os alunos serao removidos da fila.
void imprime_Fila(Fila* fi){
    if(fi == NULL)
        return;
    Elem* no = fi->inicio;
    while(no != NULL){
        printf("Matricula: %d\n", no->dados.matricula);
        printf("Nome: %s\n", no->dados.nome);
        printf("Notas: %.1f %.1f %.1f\n", no->dados.n1, no->dados.n2, no->dados.n3);
        printf("------------------------------\n");
        no = no->prox;
    }
}

// Consulta o primeiro aluno sem alterar os ponteiros ou a quantidade.
int consulta_Fila(Fila* fi, struct aluno* al){
    if(fi == NULL || fi->inicio == NULL || al == NULL)
        return 0;
    *al = fi->inicio->dados;
    return 1;
}
