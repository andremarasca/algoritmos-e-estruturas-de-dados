/*
 * Trechos do PDF: representação (6), criação (7), destruição (8), Pré-Ordem (9),
 * inserção (13 e 22), busca (96), remoção (132) e substituição (156).
 * Esses blocos preservam as instruções, os nomes e os comentários originais.
 * As explicações ficam antes dos blocos para não alterar o código apresentado.
 * Os complementos após remove_atual implementam as partes não fornecidas em C.
 */
#include <stdio.h>
#include <stdlib.h>
#include "ArvoreBinaria.h"

/* PDF, slide 6: cada nó guarda a chave e os endereços das duas subárvores. */
struct NO{
    int info;
    struct NO *esq;
    struct NO *dir;
};

/* Declara as auxiliares usadas antes de sua definição, sem expor o nó no cabeçalho. */
void libera_NO(struct NO* no);
struct NO* remove_atual(struct NO* atual);

/*
 * O slide 7 usa () na definição. A exceção local mantém esse trecho sem alterar
 * a checagem de protótipos das demais funções. O cabeçalho declara (void).
 */
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-prototypes"
#endif

/*
 * PDF, slide 7: malloc reserva a caixa, não um nó. sizeof mede os bytes necessários.
 * A decisão inicializa a caixa somente quando a alocação existe. *raiz acessa seu conteúdo.
 */
ArvBin* cria_ArvBin(){
    ArvBin* raiz = (ArvBin*) malloc(sizeof(ArvBin));
    if(raiz != NULL)
        *raiz = NULL;
    return raiz;
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

/*
 * PDF, slide 8: a parada evita acessar uma subárvore vazia. A Pós-Ordem libera os
 * filhos antes do pai. free devolve a memória. no = NULL altera somente a cópia local.
 */
void libera_NO(struct NO* no){
    if(no == NULL)
        return;
    libera_NO(no->esq);
    libera_NO(no->dir);
    free(no);
    no = NULL;
}

/* PDF, slide 8: a decisão aceita raiz inexistente; depois são liberados nós e caixa. */
void libera_ArvBin(ArvBin* raiz){
    if(raiz == NULL)
        return;
    libera_NO(*raiz);//libera cada nó
    free(raiz);//libera a raiz
}

/*
 * PDF, slide 9: as decisões separam caixa inexistente e posição vazia.
 * printf registra a chave antes das duas chamadas. -> acessa um campo pelo ponteiro.
 * & obtém o endereço de um campo para que ele seja tratado como raiz de subárvore.
 */
void preOrdem_ArvBin(ArvBin *raiz){
    if(raiz == NULL)
        return;
    if(*raiz != NULL){
        printf("%d\n",(*raiz)->info);
        preOrdem_ArvBin(&((*raiz)->esq));
        preOrdem_ArvBin(&((*raiz)->dir));
    }
}

/*
 * PDF, slides 13 e 22: as primeiras decisões rejeitam caixa inexistente e falha de
 * alocação. A árvore vazia recebe o primeiro nó. Caso contrário, o laço procura uma
 * posição vazia e guarda seu pai em ant. A igualdade descarta a duplicata e libera
 * o candidato. As comparações escolhem o lado e, ao final, atualizam o vínculo do pai.
 */
int insere_ArvBin(ArvBin* raiz, int valor){
    if(raiz == NULL)
        return 0;
    struct NO* novo;
    novo = (struct NO*) malloc(sizeof(struct NO));
    if(novo == NULL)
        return 0;
    novo->info = valor;
    novo->dir = NULL;
    novo->esq = NULL;

    if(*raiz == NULL)
        *raiz = novo;
    else{
        struct NO* atual = *raiz;
        struct NO* ant = NULL;
        while(atual != NULL){
            ant = atual;
            if(valor == atual->info){
                free(novo);
                return 0;//elemento já existe
            }

            if(valor > atual->info)
                atual = atual->dir;
            else
                atual = atual->esq;
        }
        if(valor > ant->info)
            ant->dir = novo;
        else
            ant->esq = novo;
    }
    return 1;
}

/*
 * PDF, slide 96: a primeira decisão aceita caixa inexistente; 0 é ponteiro nulo
 * nesse retorno. O laço segue uma única subárvore. A igualdade retorna o próprio
 * nó e a comparação seguinte descarta o lado que não pode conter a chave.
 */
struct NO* busca_ArvBin(ArvBin *raiz, int valor){
    if(raiz == NULL)
        return 0;
    struct NO* atual = *raiz;
    while(atual != NULL){
        if(valor == atual->info){
            return atual;
        }
        if(valor > atual->info)
            atual = atual->dir;
        else
            atual = atual->esq;
    }
    return NULL;
}

/*
 * PDF, slide 132: o laço procura a chave. Ao encontrar, as decisões distinguem
 * a raiz do filho esquerdo ou direito e guardam a substituição no vínculo correto.
 * A atualização de ant antes da descida preserva o pai do próximo nó examinado.
 */
int remove_ArvBin(ArvBin *raiz, int valor){
    if(raiz == NULL)
        return 0;
    struct NO* ant = NULL;
    struct NO* atual = *raiz;
    while(atual != NULL){
        if(valor == atual->info){
            if(atual == *raiz)
                *raiz = remove_atual(atual);
            else{
                if(ant->dir == atual)
                    ant->dir = remove_atual(atual);
                else
                    ant->esq = remove_atual(atual);
            }
            return 1;
        }
        ant = atual;
        if(valor > atual->info)
            atual = atual->dir;
        else
            atual = atual->esq;
    }
    return 0;
}

/*
 * PDF, slide 156. Pré-condição: atual existe e foi localizado pela remoção.
 * Sem filho esquerdo, a primeira decisão devolve a direita, inclusive NULL.
 * Havendo esquerda, o laço encontra seu maior nó. A decisão seguinte desconecta
 * um substituto mais profundo sem perder o filho esquerdo dele. As referências
 * são preparadas antes de liberar atual. O nó substituto não é copiado nem liberado.
 */
struct NO* remove_atual(struct NO* atual){
    struct NO *no1, *no2;
    if(atual->esq == NULL){
        no2 = atual->dir;
        free(atual);
        return no2;
    }
    no1 = atual;
    no2 = atual->esq;
    while(no2->dir != NULL){
        no1 = no2;
        no2 = no2->dir;
    }

    if(no1 != atual){
        no1->dir = no2->esq;
        no2->esq = atual->esq;
    }
    no2->dir = atual->dir;
    free(atual);
    return no2;
}

/* Complemento do slide 228: soma as subárvores e inclui o nó atual. */
int totalNO_ArvBin(ArvBin* raiz) {
    /* A ausência da caixa ou do nó não contribui para a contagem. */
    if (raiz == NULL || *raiz == NULL) {
        return 0;
    }

    int total_esq = totalNO_ArvBin(&((*raiz)->esq));
    int total_dir = totalNO_ArvBin(&((*raiz)->dir));
    return 1 + total_esq + total_dir;
}

/* Complemento do slide 126: conta descidas por arestas, sem contar a visita inicial. */
int consulta_deslocamentos_ArvBin(ArvBin* raiz, int valor, int* deslocamentos) {
    /* O resultado precisa de um destino válido e de uma árvore criada. */
    if (raiz == NULL || deslocamentos == NULL) {
        return 0;
    }

    struct NO* atual = *raiz;
    int total = 0;

    /* A busca segue somente o lado que ainda pode conter a chave. */
    while (atual != NULL) {
        /* A raiz encontrada imediatamente possui zero deslocamentos. */
        if (valor == atual->info) {
            *deslocamentos = total;
            return 1;
        }

        /* A comparação preserva a mesma seleção de subárvore da busca do PDF. */
        if (valor < atual->info) {
            atual = atual->esq;
        /* O valor maior só pode estar na subárvore direita. */
        } else {
            atual = atual->dir;
        }
        total++;
    }
    return 0;
}

/* Complemento do slide 228: conta os níveis do caminho mais longo até uma folha. */
int altura_ArvBin(ArvBin* raiz) {
    /* O retorno zero permite que uma folha tenha altura um. */
    if (raiz == NULL || *raiz == NULL) {
        return 0;
    }

    int altura_esq = altura_ArvBin(&((*raiz)->esq));
    int altura_dir = altura_ArvBin(&((*raiz)->dir));

    /* Apenas o maior caminho determina a altura, sem somar caminhos distintos. */
    if (altura_esq > altura_dir) {
        return 1 + altura_esq;
    }
    return 1 + altura_dir;
}

/* Complemento do slide 228: visita a raiz entre os valores menores e os maiores. */
void emOrdem_ArvBin(ArvBin* raiz) {
    /* A parada evita imprimir ou acessar uma posição vazia. */
    if (raiz == NULL || *raiz == NULL) {
        return;
    }

    emOrdem_ArvBin(&((*raiz)->esq));
    printf("%d\n", (*raiz)->info);
    emOrdem_ArvBin(&((*raiz)->dir));
}

/* Complemento do slide 228: conclui as duas subárvores antes de imprimir o pai. */
void posOrdem_ArvBin(ArvBin* raiz) {
    /* A parada encerra os ramos que não possuem nó. */
    if (raiz == NULL || *raiz == NULL) {
        return;
    }

    posOrdem_ArvBin(&((*raiz)->esq));
    posOrdem_ArvBin(&((*raiz)->dir));
    printf("%d\n", (*raiz)->info);
}

/* Calcula alturas enquanto verifica o critério local do slide 230 em todos os nós. */
static int verifica_balanceamento_NO(struct NO* no, int* altura) {
    /* Uma subárvore vazia é balanceada e não acrescenta níveis. */
    if (no == NULL) {
        *altura = 0;
        return 1;
    }

    int altura_esq = 0;
    int altura_dir = 0;

    /* Uma violação em qualquer descendente já invalida a árvore inteira. */
    if (!verifica_balanceamento_NO(no->esq, &altura_esq) ||
        !verifica_balanceamento_NO(no->dir, &altura_dir)) {
        return 0;
    }

    int diferenca = altura_esq - altura_dir;

    /* Detecta os dois sentidos de desequilíbrio sem depender da posição das folhas. */
    if (diferenca < -1 || diferenca > 1) {
        return 0;
    }

    *altura = altura_dir + 1;
    /* Propaga o caminho mais longo ao pai para que cada nó seja examinado uma vez. */
    if (altura_esq > altura_dir) {
        *altura = altura_esq + 1;
    }
    return 1;
}

/* Distingue caixa inválida de árvore vazia e verifica a diferença de alturas. */
int balanceada_ArvBin(ArvBin* raiz) {
    /* Sem caixa não existe uma árvore criada para consultar. */
    if (raiz == NULL) {
        return 0;
    }

    int altura = 0;
    return verifica_balanceamento_NO(*raiz, &altura);
}

/* Salva as chaves em um vetor previamente dimensionado com totalNO_ArvBin. */
static void salva_emOrdem_NO(struct NO* no, int vetor[], int* indice) {
    /* Posições vazias não ocupam entradas no vetor ordenado. */
    if (no == NULL) {
        return;
    }

    salva_emOrdem_NO(no->esq, vetor, indice);
    vetor[*indice] = no->info;
    *indice = *indice + 1;
    salva_emOrdem_NO(no->dir, vetor, indice);
}

/* Insere o meio e repete nos intervalos inclusivos esquerdo e direito. */
static int insere_meios_ArvBin(ArvBin* raiz, const int vetor[], int inicio, int fim) {
    /* O intervalo vazio encerra a reconstrução daquele ramo. */
    if (inicio > fim) {
        return 1;
    }

    /* A divisão inteira escolhe o meio da esquerda e esta forma evita somar os limites. */
    int meio = inicio + (fim - inicio) / 2;

    /* Uma falha de alocação interrompe a construção para que o chamador possa desfazê-la. */
    if (!insere_ArvBin(raiz, vetor[meio])) {
        return 0;
    }

    /* Conclui a esquerda antes da direita, na ordem apresentada pelos slides. */
    if (!insere_meios_ArvBin(raiz, vetor, inicio, meio - 1)) {
        return 0;
    }
    return insere_meios_ArvBin(raiz, vetor, meio + 1, fim);
}

/* Implementa a reconstrução dos slides sem destruir a original antes do sucesso. */
int rebalanceia_ArvBin(ArvBin* raiz) {
    /* A operação exige uma caixa válida para receber a nova raiz. */
    if (raiz == NULL) {
        return 0;
    }

    int total = totalNO_ArvBin(raiz);

    /* A árvore vazia já atende ao objetivo e não exige alocações. */
    if (total == 0) {
        return 1;
    }

    /* size_t é o tipo de tamanho em bytes; a conversão prepara o cálculo para malloc. */
    int* vetor = malloc((size_t)total * sizeof(*vetor));

    /* Sem o vetor não há como guardar todas as chaves antes da reconstrução. */
    if (vetor == NULL) {
        return 0;
    }

    int indice = 0;
    salva_emOrdem_NO(*raiz, vetor, &indice);
    ArvBin* nova = cria_ArvBin();

    /* A criação da nova caixa pode falhar sem afetar a árvore original. */
    if (nova == NULL) {
        free(vetor);
        return 0;
    }

    /* Descarta a árvore parcial e os auxiliares quando alguma inserção falha. */
    if (!insere_meios_ArvBin(nova, vetor, 0, total - 1)) {
        libera_ArvBin(nova);
        free(vetor);
        return 0;
    }

    libera_NO(*raiz);
    *raiz = *nova;
    free(nova);
    free(vetor);
    return 1;
}
