// Implementa um deque dinamico duplamente encadeado.
#include <stdio.h>
#include <stdlib.h>

#include "DequeDin.h"

struct elemento{
    struct aluno dados;
    struct elemento *ant;
    struct elemento *prox;
};
typedef struct elemento Elem;

struct deque{
    struct elemento *inicio;
    struct elemento *fim;
    int qtd;
};

// Inicializa os dois extremos e a quantidade para representar um deque vazio.
Deque* cria_Deque(void){
    Deque* de = (Deque*) malloc(sizeof(Deque));
    if(de != NULL){
        de->inicio = NULL;
        de->fim = NULL;
        de->qtd = 0;
    }
    return de;
}

// Percorre o deque pelo inicio, libera cada no e, por ultimo, o descritor.
void libera_Deque(Deque* de){
    if(de != NULL){
        Elem* no;
        while(de->inicio != NULL){
            no = de->inicio;
            de->inicio = de->inicio->prox;
            free(no);
        }
        free(de);
    }
}

// Insere o aluno antes do primeiro no e atualiza o final na primeira insercao.
int insere_inicio_Deque(Deque* de, struct aluno al){
    if(de == NULL)
        return 0;
    Elem* no = (Elem*) malloc(sizeof(Elem));
    if(no == NULL)
        return 0;
    no->dados = al;
    no->ant = NULL;
    no->prox = de->inicio;
    if(de->inicio == NULL)
        de->fim = no;
    else
        de->inicio->ant = no;
    de->inicio = no;
    de->qtd++;
    return 1;
}

// Insere o aluno depois do ultimo no e atualiza o inicio na primeira insercao.
int insere_final_Deque(Deque* de, struct aluno al){
    if(de == NULL)
        return 0;
    Elem* no = (Elem*) malloc(sizeof(Elem));
    if(no == NULL)
        return 0;
    no->dados = al;
    no->ant = de->fim;
    no->prox = NULL;
    if(de->fim == NULL)
        de->inicio = no;
    else
        de->fim->prox = no;
    de->fim = no;
    de->qtd++;
    return 1;
}

// Remove o primeiro aluno e preserva as ligacoes entre os nos restantes.
int remove_inicio_Deque(Deque* de){
    if(de == NULL)
        return 0;
    if(de->inicio == NULL)
        return 0;
    Elem* no = de->inicio;
    de->inicio = de->inicio->prox;
    if(de->inicio == NULL)
        de->fim = NULL;
    else
        de->inicio->ant = NULL;
    free(no);
    de->qtd--;
    return 1;
}

// Remove o ultimo aluno e preserva as ligacoes entre os nos restantes.
int remove_final_Deque(Deque* de){
    if(de == NULL)
        return 0;
    if(de->fim == NULL)
        return 0;
    Elem* no = de->fim;
    de->fim = de->fim->ant;
    if(de->fim == NULL)
        de->inicio = NULL;
    else
        de->fim->prox = NULL;
    free(no);
    de->qtd--;
    return 1;
}

// Consulta o primeiro aluno sem modificar o deque.
int consulta_inicio_Deque(Deque* de, struct aluno* al){
    if(de == NULL || de->inicio == NULL || al == NULL)
        return 0;
    *al = de->inicio->dados;
    return 1;
}

// Consulta o ultimo aluno sem modificar o deque.
int consulta_final_Deque(Deque* de, struct aluno* al){
    if(de == NULL || de->fim == NULL || al == NULL)
        return 0;
    *al = de->fim->dados;
    return 1;
}

// Mostra os alunos do inicio para o final do deque.
void imprime_Deque(Deque* de){
    if(de == NULL)
        return;
    Elem* no = de->inicio;
    while(no != NULL){
        printf("Matricula: %d\n", no->dados.matricula);
        printf("Nome: %s\n", no->dados.nome);
        printf("Notas: %.1f %.1f %.1f\n", no->dados.n1, no->dados.n2, no->dados.n3);
        printf("------------------------------\n");
        no = no->prox;
    }
}
