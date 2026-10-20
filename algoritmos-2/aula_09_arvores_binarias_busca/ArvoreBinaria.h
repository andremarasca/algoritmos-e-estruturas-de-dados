#ifndef ARVORE_BINARIA_H
#define ARVORE_BINARIA_H

/*
 * Aula 09: árvores binárias de busca.
 * A representação e os nomes do PDF são preservados.
 * ArvBin referencia um nó. ArvBin* referencia a caixa que guarda a raiz.
 * Os campos do nó ficam na implementação, como no slide 6.
 */
typedef struct NO* ArvBin;

/* Cria a caixa com uma raiz vazia. Retorna NULL se a alocação falhar. */
ArvBin* cria_ArvBin(void);

/* Libera todos os nós e a caixa. O chamador deve depois atribuir NULL à sua variável. */
void libera_ArvBin(ArvBin* raiz);

/* Imprime uma chave por linha, antes de percorrer as subárvores. */
void preOrdem_ArvBin(ArvBin *raiz);

/* Insere uma chave única. Retorna 1 ao inserir e 0 para duplicata, falha ou raiz inválida. */
int insere_ArvBin(ArvBin* raiz, int valor);

/*
 * Retorna o endereço do nó ou NULL. A consulta não transfere a propriedade da memória.
 * Não use esse endereço depois de remover o nó, rebalancear ou liberar a árvore.
 */
struct NO* busca_ArvBin(ArvBin *raiz, int valor);

/* Retorna 1 e preenche a quantidade de arestas até a chave. Retorna 0 se não encontrar. */
int consulta_deslocamentos_ArvBin(ArvBin* raiz, int valor, int* deslocamentos);

/*
 * Remove uma chave. Retorna 1 ao remover e 0 para chave ausente ou raiz inválida.
 * O código do PDF promove o maior nó da esquerda sempre que essa subárvore existe,
 * inclusive quando ela é a única subárvore do nó removido.
 */
int remove_ArvBin(ArvBin *raiz, int valor);

/* Complementos do slide 228: vazio tem zero nós e altura zero; folha tem altura um. */
int totalNO_ArvBin(ArvBin* raiz);
int altura_ArvBin(ArvBin* raiz);

/* Imprime as chaves em ordem crescente, uma por linha. */
void emOrdem_ArvBin(ArvBin* raiz);

/* Imprime os filhos antes do pai, uma chave por linha. */
void posOrdem_ArvBin(ArvBin* raiz);

/* Retorna 1 quando a diferença entre alturas é no máximo um em cada nó. */
int balanceada_ArvBin(ArvBin* raiz);

/*
 * Reconstrói por vetor Em-Ordem e inserção dos meios, conforme os slides 234 a 257.
 * Retorna 1 no sucesso, inclusive árvore vazia. Em falha retorna 0 e preserva a original.
 * A caixa da raiz continua sendo a mesma, mas os endereços dos nós são substituídos.
 */
int rebalanceia_ArvBin(ArvBin* raiz);

#endif
