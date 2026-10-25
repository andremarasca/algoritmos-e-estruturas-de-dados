/*
 * Aula 10: árvores AVL com chaves inteiras únicas.
 * Fonte: Muriel Mazzetto, "10 - Árvores AVL.pdf", páginas 9, 21 e 44 a 49.
 * Adaptação de materiais_apoio/aula_10_avl_tree.c. O PDF não contém código C.
 *
 * Compilar: gcc -std=c17 -Wall -Wextra -Wpedantic -Wconversion -Wshadow
 *              -Wstrict-prototypes -Werror avl_tree.c -o aula_10
 * Executar: ./aula_10
 *
 * Como na aula 9, a criação aloca uma caixa que guarda o endereço do primeiro nó.
 * arvore == NULL indica caixa inexistente. *arvore == NULL indica árvore vazia.
 * A caixa possui todos os nós alcançáveis a partir de seu conteúdo.
 * A altura conta níveis: árvore vazia = 0, folha = 1. FB = esquerda menos direita.
 * Invariantes: ordem estrita de ABB, alturas corretas e FB entre -1 e 1.
 * A remoção com dois filhos usa a menor chave da subárvore direita, o sucessor.
 * Duplicata ou falha de inserção preserva a árvore.
 * A busca devolve um ponteiro emprestado, inválido após alterações ou liberação.
 * liberar_avl devolve nós e caixa. Depois, o chamador deve fazer arvore = NULL.
 * bool representa verdadeiro/falso e size_t representa tamanhos e índices.
 * O sufixo U indica constante sem sinal. condicao ? a : b seleciona um valor.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/* Cada nó guarda a chave, a altura da subárvore e os endereços dos dois filhos. */
typedef struct no_avl {
    int chave;
    int altura;
    struct no_avl *esquerda;
    struct no_avl *direita;
} no_avl;

/* O tipo representa um endereço de nó. arvore_avl* aponta para a caixa da raiz. */
typedef no_avl *arvore_avl;

/* Resultados explícitos distinguem duplicata, falta de memória e referência inválida. */
typedef enum {
    AVL_INSERIDO,
    AVL_CHAVE_DUPLICADA,
    AVL_SEM_MEMORIA,
    AVL_ARVORE_INVALIDA
} resultado_insercao_avl;

/* Aloca apenas a caixa da raiz e cria uma árvore vazia, como na ABB da aula 9. */
static arvore_avl *criar_avl(void) {
    arvore_avl *arvore = malloc(sizeof(*arvore));
    /* A caixa só pode receber uma raiz vazia quando sua alocação existe. */
    if (arvore == NULL) {
        return NULL;
    }
    *arvore = NULL;
    return arvore;
}

/* Uma subárvore vazia não acrescenta níveis à altura do pai. */
static int altura_no(const no_avl *no) {
    return no == NULL ? 0 : no->altura;
}

/* Recalcula a altura após atualizar as alturas dos filhos de um nó existente. */
static void atualizar_altura(no_avl *no) {
    const int altura_esquerda = altura_no(no->esquerda);
    const int altura_direita = altura_no(no->direita);
    no->altura = 1 + (altura_esquerda > altura_direita ? altura_esquerda : altura_direita);
}

/* O fator usa altura da esquerda menos altura da direita, com o sinal adotado na aula. */
static int fator_balanceamento(const no_avl *no) {
    return no == NULL ? 0 : altura_no(no->esquerda) - altura_no(no->direita);
}

/* Aloca uma folha. sizeof(*no) mede o nó, e não o ponteiro. */
static no_avl *criar_no_avl(int chave) {
    no_avl *no = malloc(sizeof(*no));
    /* Informa a falha antes de escrever em um endereço inválido. */
    if (no == NULL) {
        return NULL;
    }
    no->chave = chave;
    no->altura = 1;
    no->esquerda = NULL;
    no->direita = NULL;
    return no;
}

/* Caso LL: raiz e raiz->esquerda devem existir. A rotação não aloca nem libera nós. */
static no_avl *rotacionar_direita(no_avl *raiz) {
    no_avl *nova_raiz = raiz->esquerda;
    no_avl *subarvore_transferida = nova_raiz->direita;
    /* Preserva a subárvore intermediária antes de substituir a ligação de seu antigo pai. */
    nova_raiz->direita = raiz;
    raiz->esquerda = subarvore_transferida;
    /* Atualiza primeiro o nó rebaixado, pois a altura do promovido depende dele. */
    atualizar_altura(raiz);
    atualizar_altura(nova_raiz);
    return nova_raiz;
}

/* Caso RR: raiz e raiz->direita devem existir. A subárvore intermediária muda de pai. */
static no_avl *rotacionar_esquerda(no_avl *raiz) {
    no_avl *nova_raiz = raiz->direita;
    no_avl *subarvore_transferida = nova_raiz->esquerda;
    nova_raiz->esquerda = raiz;
    raiz->direita = subarvore_transferida;
    atualizar_altura(raiz);
    atualizar_altura(nova_raiz);
    return nova_raiz;
}

/* Restaura uma subárvore após inserir ou remover em uma AVL anteriormente válida. */
static no_avl *rebalancear(no_avl *raiz) {
    /* A remoção de uma folha pode deixar uma subárvore sem altura para atualizar. */
    if (raiz == NULL) {
        return NULL;
    }
    atualizar_altura(raiz);
    const int fator = fator_balanceamento(raiz);

    /* O excesso de altura à esquerda exige uma correção LL ou LR. */
    if (fator > 1) {
        /* O filho inclinado para dentro exige a primeira rotação do caso LR. */
        if (fator_balanceamento(raiz->esquerda) < 0) {
            raiz->esquerda = rotacionar_esquerda(raiz->esquerda);
        }
        return rotacionar_direita(raiz);
    }
    /* O excesso de altura à direita exige uma correção RR ou RL. */
    if (fator < -1) {
        /* O filho inclinado para dentro exige a primeira rotação do caso RL. */
        if (fator_balanceamento(raiz->direita) > 0) {
            raiz->direita = rotacionar_direita(raiz->direita);
        }
        return rotacionar_esquerda(raiz);
    }
    return raiz;
}

/* Desce como em uma ABB e corrige os ancestrais no retorno da recursão. */
static no_avl *inserir_no(no_avl *raiz, int chave, resultado_insercao_avl *resultado) {
    /* Somente uma posição vazia pode receber a nova folha. */
    if (raiz == NULL) {
        no_avl *no = criar_no_avl(chave);
        *resultado = no == NULL ? AVL_SEM_MEMORIA : AVL_INSERIDO;
        return no;
    }
    /* Uma chave já presente não cria outro nó nem altera a árvore. */
    if (chave == raiz->chave) {
        *resultado = AVL_CHAVE_DUPLICADA;
        return raiz;
    }
    /* A ordem da ABB determina o único ramo que pode receber a chave. */
    if (chave < raiz->chave) {
        raiz->esquerda = inserir_no(raiz->esquerda, chave, resultado);
    } else {
        raiz->direita = inserir_no(raiz->direita, chave, resultado);
    }
    /* Uma inserção rejeitada ou sem memória não alterou a subárvore. */
    if (*resultado != AVL_INSERIDO) {
        return raiz;
    }
    return rebalancear(raiz);
}

/* Atualiza o conteúdo da caixa, preservando seu endereço mesmo quando a raiz muda. */
static resultado_insercao_avl inserir_avl(arvore_avl *arvore, int chave) {
    /* Sem caixa, não existe uma posição válida para guardar a raiz. */
    if (arvore == NULL) {
        return AVL_ARVORE_INVALIDA;
    }
    resultado_insercao_avl resultado = AVL_SEM_MEMORIA;
    *arvore = inserir_no(*arvore, chave, &resultado);
    return resultado;
}

/* A busca consulta um ramo por vez e devolve um ponteiro sem transferir a propriedade. */
static const no_avl *buscar_avl(const arvore_avl *arvore, int chave) {
    /* Uma caixa inexistente não permite consultar o endereço da raiz. */
    if (arvore == NULL) {
        return NULL;
    }
    const no_avl *no = *arvore;
    /* Encerra ao encontrar a chave ou uma posição vazia que comprova sua ausência. */
    while (no != NULL && no->chave != chave) {
        no = chave < no->chave ? no->esquerda : no->direita;
    }
    return no;
}

/* O nó mais à esquerda contém a menor chave de uma subárvore não vazia. */
static no_avl *no_minimo(no_avl *raiz) {
    no_avl *no = raiz;
    /* Descarta a cada passo os nós que não podem conter a menor chave. */
    while (no->esquerda != NULL) {
        no = no->esquerda;
    }
    return no;
}

/* Remove como em uma ABB e rebalanceia os ancestrais afetados no retorno. */
static no_avl *remover_no(no_avl *raiz, int chave, bool *removido) {
    /* Uma posição vazia comprova que a chave procurada não existe. */
    if (raiz == NULL) {
        return NULL;
    }
    /* A igualdade identifica o nó cuja chave deve sair da árvore. */
    if (chave == raiz->chave) {
        *removido = true;
        /* Preserva o filho direito antes de liberar um nó sem filho esquerdo. */
        if (raiz->esquerda == NULL) {
            no_avl *direita = raiz->direita;
            free(raiz);
            return direita;
        }
        /* O filho esquerdo substitui o nó cuja subárvore direita está vazia. */
        if (raiz->direita == NULL) {
            no_avl *esquerda = raiz->esquerda;
            free(raiz);
            return esquerda;
        }
        no_avl *sucessor = no_minimo(raiz->direita);
        raiz->chave = sucessor->chave;
        bool sucessor_removido = false;
        raiz->direita = remover_no(raiz->direita, sucessor->chave, &sucessor_removido);
    } else {
        /* Sem igualdade, a ordem da ABB seleciona o único ramo possível. */
        if (chave < raiz->chave) {
            raiz->esquerda = remover_no(raiz->esquerda, chave, removido);
        } else {
            raiz->direita = remover_no(raiz->direita, chave, removido);
        }
    }
    /* Uma chave ausente deve preservar a subárvore e seus endereços. */
    if (!*removido) {
        return raiz;
    }
    return rebalancear(raiz);
}

/* Informa se uma chave foi removida. Referência inválida ou chave ausente resulta em false. */
static bool remover_avl(arvore_avl *arvore, int chave) {
    /* Rejeita a referência inválida antes de acessar a árvore. */
    if (arvore == NULL) {
        return false;
    }
    bool removido = false;
    *arvore = remover_no(*arvore, chave, &removido);
    return removido;
}

/* Exibe as chaves em ordem crescente sem alterar a árvore. */
static void exibir_em_ordem(const no_avl *raiz) {
    /* Ramos vazios não acrescentam chaves ao percurso. */
    if (raiz == NULL) {
        return;
    }
    exibir_em_ordem(raiz->esquerda);
    printf(" %d", raiz->chave);
    exibir_em_ordem(raiz->direita);
}

/* A pré-ordem indentada usa E/D para distinguir os filhos, inclusive quando um está ausente. */
static void exibir_arvore_avl(const no_avl *raiz, int profundidade, const char *ligacao) {
    /* Somente nós existentes precisam de uma linha na saída. */
    if (raiz == NULL) {
        return;
    }
    /* %*s usa a largura recebida para recuar quatro espaços por nível. */
    printf("%*s%s: %d (h=%d, FB=%d)\n", profundidade * 4, "", ligacao, raiz->chave, raiz->altura,
           fator_balanceamento(raiz));
    exibir_arvore_avl(raiz->esquerda, profundidade + 1, "E");
    exibir_arvore_avl(raiz->direita, profundidade + 1, "D");
}

/* Libera os nós em Pós-Ordem, preservando os filhos até concluir suas chamadas. */
static void liberar_nos_avl(no_avl *no) {
    /* Um ramo vazio não possui nó para devolver à memória. */
    if (no == NULL) {
        return;
    }
    liberar_nos_avl(no->esquerda);
    liberar_nos_avl(no->direita);
    free(no);
}

/* Libera todos os nós e a caixa. O chamador deve depois fazer arvore = NULL. */
static void liberar_avl(arvore_avl *arvore) {
    /* Uma caixa inexistente não possui alocações para liberar. */
    if (arvore == NULL) {
        return;
    }
    liberar_nos_avl(*arvore);
    free(arvore);
}

/* Exibe as chaves ordenadas e a forma da árvore após uma operação concluída. */
static void mostrar_arvore(const arvore_avl *arvore) {
    const no_avl *raiz = arvore == NULL ? NULL : *arvore;
    printf("Em ordem:");
    exibir_em_ordem(raiz);
    printf("\n");
    /* O estado vazio precisa de uma indicação visível, pois o percurso não exibe chaves. */
    if (raiz == NULL) {
        puts("Raiz: vazia");
        return;
    }
    exibir_arvore_avl(raiz, 0, "Raiz");
}

/* Cada exemplo começa vazio. Suas operações são cumulativas e as chaves são únicas. */
static bool executar_exemplo(const char *titulo, const int chaves[], size_t quantidade_chaves,
                             const int remocoes[], size_t quantidade_remocoes) {
    arvore_avl *arvore = criar_avl();
    /* A demonstração só pode começar depois de criar a caixa da raiz. */
    if (arvore == NULL) {
        fputs("Não foi possível criar a caixa da raiz.\n", stderr);
        return false;
    }
    printf("\n=== %s ===\n", titulo);
    mostrar_arvore(arvore);
    /* Exibe cada inserção para permitir comparar seus estados com a aula. */
    for (size_t indice = 0U; indice < quantidade_chaves; indice++) {
        printf("Inserir %d\n", chaves[indice]);
        /* Libera a árvore parcial quando não é possível concluir o exemplo. */
        if (inserir_avl(arvore, chaves[indice]) != AVL_INSERIDO) {
            liberar_avl(arvore);
            arvore = NULL;
            fputs("Não foi possível inserir a chave do exemplo.\n", stderr);
            return false;
        }
        mostrar_arvore(arvore);
    }
    /* Confere chaves presentes, duplicadas e ausentes após construir a árvore. */
    if (quantidade_chaves > 0U) {
        printf("Buscar %d: %s\n", chaves[0],
               buscar_avl(arvore, chaves[0]) == NULL ? "ausente" : "encontrada");
        printf("Duplicata %d rejeitada: %s\n", chaves[0],
               inserir_avl(arvore, chaves[0]) == AVL_CHAVE_DUPLICADA ? "sim" : "não");
    }
    printf("Buscar -1: %s\n", buscar_avl(arvore, -1) == NULL ? "ausente" : "encontrada");
    printf("Remover -1: %s\n", remover_avl(arvore, -1) ? "removida" : "ausente");
    /* Aplica cada remoção à árvore resultante da operação anterior. */
    for (size_t indice = 0U; indice < quantidade_remocoes; indice++) {
        printf("Remover %d\n", remocoes[indice]);
        /* Os alvos existem nestes exemplos. Sua ausência indicaria um erro. */
        if (!remover_avl(arvore, remocoes[indice])) {
            liberar_avl(arvore);
            arvore = NULL;
            fputs("Não foi possível remover a chave do exemplo.\n", stderr);
            return false;
        }
        mostrar_arvore(arvore);
    }
    liberar_avl(arvore);
    arvore = NULL;
    puts("Árvore liberada.");
    return true;
}

/* Executa as quatro rotações, o caso sem rotação e os exercícios dos slides. */
int main(void) {
    const int rr[] = {10, 20, 30};
    const int ll[] = {30, 20, 10};
    const int rl[] = {10, 30, 20};
    const int lr[] = {30, 10, 20};
    const int sem_rotacao[] = {5, 3, 7, 2, 4, 6, 1};
    const int primeira[] = {15, 29, 40, 8, 65, 10, 9, 87, 28, 14};
    const int remocoes_primeira[] = {29, 28};
    const int segunda[] = {15, 25, 47, 10, 8, 64, 53, 9, 13, 19, 14};
    const int remocoes_segunda[] = {47, 25};
    const int remocao_lr[] = {30, 20, 40, 25};
    const int chaves_remocao_lr[] = {40};

    /* Interrompe na primeira falha. Cada exemplo libera sua árvore antes de retornar. */
    if (!executar_exemplo("RR: rotação simples à esquerda", rr, 3U, NULL, 0U) ||
        !executar_exemplo("LL: rotação simples à direita", ll, 3U, NULL, 0U) ||
        !executar_exemplo("RL: direita, depois esquerda", rl, 3U, NULL, 0U) ||
        !executar_exemplo("LR: esquerda, depois direita", lr, 3U, NULL, 0U) ||
        !executar_exemplo("Slides 15 a 18: inserir 1 sem rotação", sem_rotacao, 7U, NULL, 0U) ||
        !executar_exemplo("Slide 46", primeira, 10U, remocoes_primeira, 2U) ||
        !executar_exemplo("Slide 47", segunda, 11U, remocoes_segunda, 2U) ||
        !executar_exemplo("Extra: rotação LR após remover 40", remocao_lr, 4U, chaves_remocao_lr,
                          1U)) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
