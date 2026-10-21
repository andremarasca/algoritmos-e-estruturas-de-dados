/*
 * Aula 11: árvore B com a convenção dos slides de Muriel Mazzetto.
 * Fonte: "11 - Árvores B.pdf", páginas 3, 13 a 75 e 82. O PDF não contém código C.
 *
 * Compilar: gcc -std=c17 -Wall -Wextra -Wpedantic -Wconversion -Wshadow
 *              -Wstrict-prototypes -Werror b_tree.c -o aula_11
 * Executar: ./aula_11
 *
 * Como nas aulas 9 e 10, a criação aloca uma caixa que guarda a primeira página.
 * arvore == NULL indica caixa inexistente. *arvore == NULL indica árvore vazia.
 * A caixa possui todas as páginas alcançáveis a partir de seu conteúdo.
 * liberar_arvore_b devolve páginas e caixa. Depois, o chamador faz arvore = NULL.
 * Ordem m: páginas não raiz têm m a 2m chaves. A raiz não vazia tem 1 a 2m.
 * Todas as folhas ficam no mesmo nível. Uma página interna com k chaves possui k+1 filhos.
 * As chaves int são únicas. Este modelo em memória não implementa acesso a disco.
 * A remoção interna usa o sucessor. A correção tenta empréstimo à esquerda e à direita.
 * Sem empréstimo, concatena com o irmão direito quando existe, ou com o esquerdo.
 * A busca empresta endereço e índice de página, que podem perder validade após mudanças.
 * A inserção reserva todas as páginas antes de alterar a árvore. Falhas são atômicas.
 * O programa implementa B. As variantes B+ e B* são discutidas no Markdown.
 * bool representa verdadeiro/falso e size_t representa tamanhos e índices.
 * O sufixo U indica constante sem sinal. condicao ? a : b seleciona um valor.
 * sizeof mede o tamanho em bytes. CHAR_BIT informa a quantidade de bits por byte.
 */

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef ARVORE_B_ORDEM
#define ARVORE_B_ORDEM 2U
#endif
#define ARVORE_B_MAX_CHAVES (2U * ARVORE_B_ORDEM)
#define ARVORE_B_MAX_FILHOS (ARVORE_B_MAX_CHAVES + 1U)
/* Chaves int únicas limitam a altura. Uma página extra permite criar uma nova raiz. */
#define ARVORE_B_CAPACIDADE_RESERVA (sizeof(int) * CHAR_BIT + 1U)

/* _Static_assert rejeita uma ordem incompatível durante a compilação. */
_Static_assert(ARVORE_B_ORDEM >= 1 && ARVORE_B_ORDEM <= (INT_MAX - 2) / 2,
               "A ordem da árvore B deve ser positiva e caber nos limites da página.");

/* As posições extras guardam uma chave excedente e seu filho adicional durante a divisão. */
typedef struct pagina_b {
    size_t quantidade_chaves;
    bool folha;
    int chaves[ARVORE_B_MAX_CHAVES + 1U];
    struct pagina_b *filhos[ARVORE_B_MAX_FILHOS + 1U];
} pagina_b;

/* O tipo representa um endereço de página. arvore_b* aponta para a caixa da raiz. */
typedef pagina_b *arvore_b;

/* Erros têm estados próprios, permitindo que qualquer valor int seja uma chave válida. */
typedef enum {
    ARVORE_B_INSERIDO,
    ARVORE_B_CHAVE_DUPLICADA,
    ARVORE_B_SEM_MEMORIA,
    ARVORE_B_REFERENCIA_INVALIDA
} resultado_insercao_b;

/* A reserva possui as páginas temporárias até sua incorporação na árvore ou liberação. */
typedef struct {
    pagina_b *paginas[ARVORE_B_CAPACIDADE_RESERVA];
    size_t quantidade;
} reserva_paginas;

/* Aloca apenas a caixa da raiz e cria uma árvore vazia, como nas aulas 9 e 10. */
static arvore_b *criar_arvore_b(void) {
    arvore_b *arvore = malloc(sizeof(*arvore));
    /* A raiz vazia só pode ser escrita depois que a caixa foi alocada. */
    if (arvore == NULL) {
        return NULL;
    }
    *arvore = NULL;
    return arvore;
}

/* Inicializa os filhos explicitamente, sem pressupor que bytes zerados representem NULL. */
static pagina_b *criar_pagina(bool folha) {
    pagina_b *pagina = malloc(sizeof(*pagina));
    /* Informa a falha de alocação antes de acessar a página. */
    if (pagina == NULL) {
        return NULL;
    }
    /* O literal composto {0} inicializa campos numéricos em zero e ponteiros em NULL. */
    *pagina = (pagina_b){0};
    pagina->folha = folha;
    return pagina;
}

/* A busca binária localiza a primeira chave maior ou igual à procurada ou o último intervalo. */
static size_t localizar_posicao(const pagina_b *pagina, int chave) {
    size_t inicio = 0U;
    size_t fim = pagina->quantidade_chaves;
    /* Cada comparação preserva o intervalo que pode conter a primeira chave procurada. */
    while (inicio < fim) {
        const size_t meio = inicio + (fim - inicio) / 2U;
        /* Descarta chaves menores que não podem ser o limite procurado. */
        if (pagina->chaves[meio] < chave) {
            inicio = meio + 1U;
        } else {
            fim = meio;
        }
    }
    return inicio;
}

/* A página retornada pertence à árvore. O índice opcional só é escrito em caso de sucesso. */
static const pagina_b *buscar_arvore_b(const arvore_b *arvore, int chave, size_t *indice_chave) {
    /* Uma caixa inexistente não permite consultar o endereço da raiz. */
    if (arvore == NULL) {
        return NULL;
    }
    const pagina_b *pagina = *arvore;
    /* Cada página examinada seleciona uma única próxima página. */
    while (pagina != NULL) {
        const size_t indice = localizar_posicao(pagina, chave);
        /* A chave pode estar em uma página interna ou em uma folha. */
        if (indice < pagina->quantidade_chaves && pagina->chaves[indice] == chave) {
            /* Permite consultar a presença sem fornecer um destino para o índice. */
            if (indice_chave != NULL) {
                *indice_chave = indice;
            }
            return pagina;
        }
        pagina = pagina->folha ? NULL : pagina->filhos[indice];
    }
    return NULL;
}

/* Libera apenas as páginas ainda reservadas, que não possuem filhos. */
static void liberar_reserva(reserva_paginas *reserva) {
    /* Retira as páginas uma a uma e esvazia a reserva mesmo após uma alocação parcial. */
    while (reserva->quantidade > 0U) {
        free(reserva->paginas[--reserva->quantidade]);
    }
}

/* Reserva todas as páginas antes de alterar chaves ou ligações da árvore. */
static bool criar_reserva(reserva_paginas *reserva, size_t quantidade) {
    reserva->quantidade = 0U;
    /* A altura de uma árvore válida com chaves int únicas cabe nesta capacidade. */
    if (quantidade > ARVORE_B_CAPACIDADE_RESERVA) {
        return false;
    }
    /* Mantém as páginas já alocadas disponíveis para desfazer uma reserva incompleta. */
    for (size_t indice = 0U; indice < quantidade; indice++) {
        pagina_b *pagina = criar_pagina(true);
        /* Uma falha na reserva deve preservar integralmente a árvore original. */
        if (pagina == NULL) {
            liberar_reserva(reserva);
            return false;
        }
        reserva->paginas[reserva->quantidade++] = pagina;
    }
    return true;
}

/* O planejamento garante uma página disponível sempre que a reserva é consumida. */
static pagina_b *obter_pagina_reservada(reserva_paginas *reserva, bool folha) {
    pagina_b *pagina = reserva->paginas[--reserva->quantidade];
    pagina->folha = folha;
    return pagina;
}

/* Conta as páginas cheias consecutivas atingidas pela divisão iniciada na folha. */
static bool planejar_divisoes(const pagina_b *pagina, int chave, size_t *quantidade_paginas) {
    bool recebe_chave = true;
    /* Uma página interna só cresce quando o filho selecionado realmente se divide. */
    if (!pagina->folha) {
        const size_t indice = localizar_posicao(pagina, chave);
        recebe_chave = planejar_divisoes(pagina->filhos[indice], chave, quantidade_paginas);
    }
    /* Uma página com espaço absorve a chave e interrompe a propagação da divisão. */
    if (!recebe_chave || pagina->quantidade_chaves < ARVORE_B_MAX_CHAVES) {
        return false;
    }
    (*quantidade_paginas)++;
    return true;
}

/* Insere a chave e seu filho direito, preservando a posição do filho esquerdo existente. */
static void inserir_na_pagina(pagina_b *pagina, size_t indice, int chave, pagina_b *filho_direito) {
    /* Abre espaço à direita sem sobrescrever as chaves que precisam permanecer. */
    for (size_t posicao = pagina->quantidade_chaves; posicao > indice; posicao--) {
        pagina->chaves[posicao] = pagina->chaves[posicao - 1U];
    }
    /* O separador promovido cria um intervalo adicional para um filho. */
    for (size_t posicao = pagina->quantidade_chaves + 1U; posicao > indice + 1U; posicao--) {
        pagina->filhos[posicao] = pagina->filhos[posicao - 1U];
    }
    pagina->chaves[indice] = chave;
    pagina->filhos[indice + 1U] = filho_direito;
    pagina->quantidade_chaves++;
}

/* Divide 2m+1 chaves em m chaves, uma mediana promovida e outras m chaves. */
static pagina_b *dividir_excedente(pagina_b *pagina, reserva_paginas *reserva, int *promovida) {
    pagina_b *direita = obter_pagina_reservada(reserva, pagina->folha);
    *promovida = pagina->chaves[ARVORE_B_ORDEM];
    direita->quantidade_chaves = ARVORE_B_ORDEM;
    /* Transfere as chaves maiores que a mediana para a página direita. */
    for (size_t indice = 0U; indice < ARVORE_B_ORDEM; indice++) {
        direita->chaves[indice] = pagina->chaves[ARVORE_B_ORDEM + 1U + indice];
    }
    /* A divisão interna também transfere os intervalos dos filhos à direita. */
    if (!pagina->folha) {
        /* Limpa as referências transferidas para que cada filho tenha uma única página
         * proprietária. */
        for (size_t indice = 0U; indice <= ARVORE_B_ORDEM; indice++) {
            direita->filhos[indice] = pagina->filhos[ARVORE_B_ORDEM + 1U + indice];
            pagina->filhos[ARVORE_B_ORDEM + 1U + indice] = NULL;
        }
    }
    pagina->quantidade_chaves = ARVORE_B_ORDEM;
    return direita;
}

/* O retorno não nulo indica divisão. promovida recebe a chave separadora da nova página direita. */
static pagina_b *inserir_com_reserva(pagina_b *pagina, int chave, reserva_paginas *reserva,
                                     int *promovida) {
    const size_t indice = localizar_posicao(pagina, chave);
    /* As novas chaves entram na folha do intervalo correto. */
    if (pagina->folha) {
        inserir_na_pagina(pagina, indice, chave, NULL);
    } else {
        int chave_filho = 0;
        pagina_b *direita =
            inserir_com_reserva(pagina->filhos[indice], chave, reserva, &chave_filho);
        /* A divisão de um filho acrescenta sua mediana e seu novo irmão ao pai. */
        if (direita != NULL) {
            inserir_na_pagina(pagina, indice, chave_filho, direita);
        }
    }
    /* Resolve o excedente temporário antes de retornar ao pai. */
    if (pagina->quantidade_chaves > ARVORE_B_MAX_CHAVES) {
        return dividir_excedente(pagina, reserva, promovida);
    }
    return NULL;
}

/* Insere uma chave única. Falha de alocação preserva chaves, forma e endereços existentes. */
static resultado_insercao_b inserir_arvore_b(arvore_b *arvore, int chave) {
    /* Rejeita a caixa inexistente antes de examinar seu conteúdo. */
    if (arvore == NULL) {
        return ARVORE_B_REFERENCIA_INVALIDA;
    }
    /* Chaves repetidas não alocam páginas nem alteram a árvore. */
    if (buscar_arvore_b(arvore, chave, NULL) != NULL) {
        return ARVORE_B_CHAVE_DUPLICADA;
    }
    /* Uma árvore vazia precisa de sua primeira página folha. */
    if (*arvore == NULL) {
        pagina_b *raiz = criar_pagina(true);
        /* Só publica a nova raiz depois que a alocação é bem-sucedida. */
        if (raiz == NULL) {
            return ARVORE_B_SEM_MEMORIA;
        }
        raiz->chaves[0] = chave;
        raiz->quantidade_chaves = 1U;
        *arvore = raiz;
        return ARVORE_B_INSERIDO;
    }
    size_t quantidade_paginas = 0U;
    const bool raiz_vai_dividir = planejar_divisoes(*arvore, chave, &quantidade_paginas);
    quantidade_paginas += raiz_vai_dividir ? 1U : 0U;
    reserva_paginas reserva = {0};
    /* A reserva completa impede falha de memória no meio de uma sequência de divisões. */
    if (!criar_reserva(&reserva, quantidade_paginas)) {
        return ARVORE_B_SEM_MEMORIA;
    }
    int promovida = 0;
    pagina_b *direita = inserir_com_reserva(*arvore, chave, &reserva, &promovida);
    /* Somente a divisão da raiz aumenta o número de níveis. */
    if (direita != NULL) {
        pagina_b *nova_raiz = obter_pagina_reservada(&reserva, false);
        nova_raiz->chaves[0] = promovida;
        nova_raiz->quantidade_chaves = 1U;
        nova_raiz->filhos[0] = *arvore;
        nova_raiz->filhos[1] = direita;
        *arvore = nova_raiz;
    }
    liberar_reserva(&reserva);
    return ARVORE_B_INSERIDO;
}

/* Desloca chaves à esquerda para retirar uma entrada da folha ou um separador após concatenação. */
static void remover_chave_pagina(pagina_b *pagina, size_t indice) {
    /* As chaves posteriores fecham a posição removida e mantêm a ordem crescente. */
    for (size_t posicao = indice + 1U; posicao < pagina->quantidade_chaves; posicao++) {
        pagina->chaves[posicao - 1U] = pagina->chaves[posicao];
    }
    pagina->quantidade_chaves--;
}

/* Faz o separador descer e a maior chave do irmão esquerdo subir ao pai. */
static void emprestar_esquerda(pagina_b *pai, size_t indice_filho) {
    pagina_b *filho = pai->filhos[indice_filho];
    pagina_b *irmao = pai->filhos[indice_filho - 1U];
    /* O separador recebido precisa ocupar a posição anterior às chaves atuais do filho. */
    for (size_t indice = filho->quantidade_chaves; indice > 0U; indice--) {
        filho->chaves[indice] = filho->chaves[indice - 1U];
    }
    /* O empréstimo entre páginas internas também transfere o intervalo de filho adjacente. */
    if (!filho->folha) {
        /* Abre espaço na borda esquerda para o intervalo transferido. */
        for (size_t indice = filho->quantidade_chaves + 1U; indice > 0U; indice--) {
            filho->filhos[indice] = filho->filhos[indice - 1U];
        }
        filho->filhos[0] = irmao->filhos[irmao->quantidade_chaves];
        irmao->filhos[irmao->quantidade_chaves] = NULL;
    }
    filho->chaves[0] = pai->chaves[indice_filho - 1U];
    pai->chaves[indice_filho - 1U] = irmao->chaves[irmao->quantidade_chaves - 1U];
    filho->quantidade_chaves++;
    irmao->quantidade_chaves--;
}

/* Empresta a menor chave do irmão direito por meio do separador no pai. */
static void emprestar_direita(pagina_b *pai, size_t indice_filho) {
    pagina_b *filho = pai->filhos[indice_filho];
    pagina_b *irmao = pai->filhos[indice_filho + 1U];
    filho->chaves[filho->quantidade_chaves] = pai->chaves[indice_filho];
    /* O primeiro filho do irmão pertence ao intervalo após o separador que desceu. */
    if (!filho->folha) {
        filho->filhos[filho->quantidade_chaves + 1U] = irmao->filhos[0];
    }
    pai->chaves[indice_filho] = irmao->chaves[0];
    /* Fecha a posição deixada pela menor chave emprestada. */
    for (size_t indice = 1U; indice < irmao->quantidade_chaves; indice++) {
        irmao->chaves[indice - 1U] = irmao->chaves[indice];
    }
    /* Desloca os filhos restantes e elimina a referência proprietária duplicada. */
    if (!irmao->folha) {
        /* Há um intervalo de filho a mais que a quantidade de chaves. */
        for (size_t indice = 1U; indice <= irmao->quantidade_chaves; indice++) {
            irmao->filhos[indice - 1U] = irmao->filhos[indice];
        }
        irmao->filhos[irmao->quantidade_chaves] = NULL;
    }
    filho->quantidade_chaves++;
    irmao->quantidade_chaves--;
}

/* Concatena a página com falta, um separador do pai e um irmão com ocupação mínima. */
static void concatenar_filhos(pagina_b *pai, size_t indice_separador) {
    pagina_b *esquerda = pai->filhos[indice_separador];
    pagina_b *direita = pai->filhos[indice_separador + 1U];
    const size_t quantidade_esquerda = esquerda->quantidade_chaves;
    const size_t quantidade_pai = pai->quantidade_chaves;
    esquerda->chaves[quantidade_esquerda] = pai->chaves[indice_separador];
    /* Após a retirada, a concatenação reúne (m-1)+1+m = 2m chaves. */
    for (size_t indice = 0U; indice < direita->quantidade_chaves; indice++) {
        esquerda->chaves[quantidade_esquerda + 1U + indice] = direita->chaves[indice];
    }
    /* A página esquerda assume os filhos transferidos. Só o recipiente direito será liberado. */
    if (!esquerda->folha) {
        /* Transfere todos os intervalos pertencentes à página direita. */
        for (size_t indice = 0U; indice <= direita->quantidade_chaves; indice++) {
            esquerda->filhos[quantidade_esquerda + 1U + indice] = direita->filhos[indice];
        }
    }
    esquerda->quantidade_chaves += direita->quantidade_chaves + 1U;
    remover_chave_pagina(pai, indice_separador);
    /* Retira do pai a referência da página direita sem perder os irmãos posteriores. */
    for (size_t indice = indice_separador + 2U; indice <= quantidade_pai; indice++) {
        pai->filhos[indice - 1U] = pai->filhos[indice];
    }
    pai->filhos[quantidade_pai] = NULL;
    free(direita);
}

/* Corrige o filho após a remoção e permite propagar a falta de ocupação ao pai. */
static void corrigir_filho(pagina_b *pai, size_t indice_filho) {
    /* Uma remoção que mantém a ocupação mínima não exige mudança estrutural. */
    if (pai->filhos[indice_filho]->quantidade_chaves >= ARVORE_B_ORDEM) {
        return;
    }
    /* Prefere o irmão esquerdo quando ele pode emprestar sem ficar abaixo do mínimo. */
    if (indice_filho > 0U && pai->filhos[indice_filho - 1U]->quantidade_chaves > ARVORE_B_ORDEM) {
        emprestar_esquerda(pai, indice_filho);
        return;
    }
    /* Tenta o irmão direito quando a esquerda não pode emprestar. */
    if (indice_filho < pai->quantidade_chaves &&
        pai->filhos[indice_filho + 1U]->quantidade_chaves > ARVORE_B_ORDEM) {
        emprestar_direita(pai, indice_filho);
        return;
    }
    const size_t indice_separador =
        indice_filho < pai->quantidade_chaves ? indice_filho : indice_filho - 1U;
    concatenar_filhos(pai, indice_separador);
}

/* A menor chave está na primeira posição da folha mais à esquerda da subárvore não vazia. */
static int menor_chave_subarvore(const pagina_b *pagina) {
    /* Segue o intervalo menor que todos os separadores até alcançar a folha. */
    while (!pagina->folha) {
        pagina = pagina->filhos[0];
    }
    return pagina->chaves[0];
}

/* A remoção não aloca memória e só altera a árvore depois de encontrar a chave. */
static bool remover_chave(pagina_b *pagina, int chave) {
    const size_t indice = localizar_posicao(pagina, chave);
    /* Uma chave nesta página pode ser retirada aqui ou substituída por seu sucessor. */
    if (indice < pagina->quantidade_chaves && pagina->chaves[indice] == chave) {
        /* Uma chave de folha não possui intervalos descendentes para preservar. */
        if (pagina->folha) {
            remover_chave_pagina(pagina, indice);
            return true;
        }
        const int sucessor = menor_chave_subarvore(pagina->filhos[indice + 1U]);
        pagina->chaves[indice] = sucessor;
        (void)remover_chave(pagina->filhos[indice + 1U], sucessor);
        corrigir_filho(pagina, indice + 1U);
        return true;
    }
    /* Chegar à folha sem igualdade comprova a ausência sem alterar páginas. */
    if (pagina->folha) {
        return false;
    }
    const bool removido = remover_chave(pagina->filhos[indice], chave);
    /* Somente uma remoção bem-sucedida pode ter causado falta de ocupação. */
    if (removido) {
        corrigir_filho(pagina, indice);
    }
    return removido;
}

/* A contração elimina um nível quando a raiz perde seu último separador. */
static bool remover_arvore_b(arvore_b *arvore, int chave) {
    /* Caixas inexistentes e árvores vazias não contêm a chave procurada. */
    if (arvore == NULL || *arvore == NULL) {
        return false;
    }
    const bool removido = remover_chave(*arvore, chave);
    /* Uma raiz interna sem chaves tem um único filho, que assume a raiz. */
    if ((*arvore)->quantidade_chaves == 0U) {
        pagina_b *raiz_anterior = *arvore;
        *arvore = raiz_anterior->folha ? NULL : raiz_anterior->filhos[0];
        free(raiz_anterior);
    }
    return removido;
}

/* O percurso ordenado intercala intervalos de filhos e suas chaves separadoras. */
static void exibir_em_ordem(const pagina_b *pagina) {
    /* Uma árvore vazia não acrescenta chaves à saída. */
    if (pagina == NULL) {
        return;
    }
    /* Cada separador é precedido pelo intervalo à sua esquerda. */
    for (size_t indice = 0U; indice < pagina->quantidade_chaves; indice++) {
        /* Folhas não possuem páginas filhas para percorrer. */
        if (!pagina->folha) {
            exibir_em_ordem(pagina->filhos[indice]);
        }
        printf(" %d", pagina->chaves[indice]);
    }
    /* O último intervalo fica após todas as chaves da página. */
    if (!pagina->folha) {
        exibir_em_ordem(pagina->filhos[pagina->quantidade_chaves]);
    }
}

/* A notação compacta preserva os limites das páginas e a ordem dos filhos. */
static void exibir_compacta(const pagina_b *pagina) {
    /* O marcador visível distingue árvore vazia de ausência de saída. */
    if (pagina == NULL) {
        printf("vazia");
        return;
    }
    printf("[");
    /* Espaços separam chaves que pertencem à mesma página. */
    for (size_t indice = 0U; indice < pagina->quantidade_chaves; indice++) {
        printf("%s%d", indice == 0U ? "" : " ", pagina->chaves[indice]);
    }
    printf("]");
    /* Os parênteses listam os filhos do menor para o maior intervalo. */
    if (!pagina->folha) {
        printf("(");
        /* Uma página interna possui exatamente quantidade_chaves+1 intervalos de filhos. */
        for (size_t indice = 0U; indice <= pagina->quantidade_chaves; indice++) {
            printf("%s", indice == 0U ? "" : ",");
            exibir_compacta(pagina->filhos[indice]);
        }
        printf(")");
    }
}

/* Exibe os limites das páginas em pré-ordem, com irmãos da esquerda para a direita. */
static void exibir_paginas(const pagina_b *pagina, int profundidade) {
    /* Um ramo vazio não possui página para exibir. */
    if (pagina == NULL) {
        return;
    }
    /* %*s usa a largura recebida para recuar quatro espaços por nível. */
    printf("%*s[", profundidade * 4, "");
    /* Exibe as chaves entre colchetes para preservar o limite da página. */
    for (size_t indice = 0U; indice < pagina->quantidade_chaves; indice++) {
        printf("%s%d", indice == 0U ? "" : " ", pagina->chaves[indice]);
    }
    printf("]\n");
    /* Somente páginas internas possuem descendentes para exibir. */
    if (!pagina->folha) {
        /* A ordem dos filhos identifica os intervalos adjacentes de cada separador. */
        for (size_t indice = 0U; indice <= pagina->quantidade_chaves; indice++) {
            exibir_paginas(pagina->filhos[indice], profundidade + 1);
        }
    }
}

/* Como as folhas estão no mesmo nível, qualquer caminho da raiz à folha mede a altura. */
static size_t altura_arvore_b(const arvore_b *arvore) {
    const pagina_b *pagina = arvore == NULL ? NULL : *arvore;
    size_t altura = 0U;
    /* O caminho mais à esquerda visita exatamente uma página de cada nível. */
    while (pagina != NULL) {
        altura++;
        pagina = pagina->folha ? NULL : pagina->filhos[0];
    }
    return altura;
}

/* Libera recursivamente os filhos antes da página que os possui. */
static void liberar_pagina(pagina_b *pagina) {
    /* Árvores vazias e ramos ausentes não possuem memória para liberar. */
    if (pagina == NULL) {
        return;
    }
    /* Somente páginas internas precisam liberar descendentes. */
    if (!pagina->folha) {
        /* Cada referência ativa de filho representa uma subárvore de propriedade exclusiva. */
        for (size_t indice = 0U; indice <= pagina->quantidade_chaves; indice++) {
            liberar_pagina(pagina->filhos[indice]);
        }
    }
    free(pagina);
}

/* Libera todas as páginas e a caixa. O chamador deve depois fazer arvore = NULL. */
static void liberar_arvore_b(arvore_b *arvore) {
    /* Uma caixa inexistente não possui alocações para liberar. */
    if (arvore == NULL) {
        return;
    }
    liberar_pagina(*arvore);
    free(arvore);
}

/* Exibe apenas estados em que excedentes e faltas temporárias já foram corrigidos. */
static void mostrar_arvore(const arvore_b *arvore) {
    printf("Estado: ");
    exibir_compacta(*arvore);
    printf("\nAltura: %zu\nEm ordem:", altura_arvore_b(arvore));
    exibir_em_ordem(*arvore);
    printf("\n");
    exibir_paginas(*arvore, 0);
}

/* Cada exemplo começa vazio e possui suas páginas até concluir a última operação. */
static bool executar_exemplo(const char *titulo, const int chaves[], size_t quantidade_chaves,
                             const int remocoes[], size_t quantidade_remocoes) {
    arvore_b *arvore = criar_arvore_b();
    /* A demonstração precisa da caixa para guardar e atualizar o endereço da raiz. */
    if (arvore == NULL) {
        fputs("Não foi possível criar a caixa da raiz.\n", stderr);
        return false;
    }
    printf("\n=== %s ===\n", titulo);
    mostrar_arvore(arvore);
    /* Exibe cada etapa para permitir comparar as divisões com os estados do PDF. */
    for (size_t indice = 0U; indice < quantidade_chaves; indice++) {
        printf("Inserir %d\n", chaves[indice]);
        /* Libera os recursos se a demonstração não consegue alocar a próxima página. */
        if (inserir_arvore_b(arvore, chaves[indice]) != ARVORE_B_INSERIDO) {
            liberar_arvore_b(arvore);
            arvore = NULL;
            fputs("Não foi possível inserir a chave do exemplo.\n", stderr);
            return false;
        }
        mostrar_arvore(arvore);
    }
    /* Localiza uma chave presente sem transferir a propriedade da página. */
    if (quantidade_chaves > 0U) {
        size_t posicao_chave = 0U;
        const pagina_b *encontrada = buscar_arvore_b(arvore, chaves[0], &posicao_chave);
        printf("Buscar %d: %s\n", chaves[0], encontrada == NULL ? "ausente" : "encontrada");
        printf("Duplicata %d rejeitada: %s\n", chaves[0],
               inserir_arvore_b(arvore, chaves[0]) == ARVORE_B_CHAVE_DUPLICADA ? "sim" : "não");
    }
    printf("Buscar -1: %s\n", buscar_arvore_b(arvore, -1, NULL) == NULL ? "ausente" : "encontrada");
    printf("Remover -1: %s\n", remover_arvore_b(arvore, -1) ? "removida" : "ausente");
    /* As remoções são cumulativas e seguem as políticas de irmãos e sucessor definidas acima. */
    for (size_t indice = 0U; indice < quantidade_remocoes; indice++) {
        printf("Remover %d\n", remocoes[indice]);
        /* Os alvos existem nestes exemplos. Sua ausência indicaria um erro. */
        if (!remover_arvore_b(arvore, remocoes[indice])) {
            liberar_arvore_b(arvore);
            arvore = NULL;
            fputs("Não foi possível remover a chave do exemplo.\n", stderr);
            return false;
        }
        mostrar_arvore(arvore);
    }
    liberar_arvore_b(arvore);
    arvore = NULL;
    puts("Árvore liberada.");
    return true;
}

/* Reproduz os três exemplos principais com a convenção m=2 da aula. */
int main(void) {
    /* A ordem escolhida alcança exatamente a organização das páginas do slide 4. */
    const int visao_geral[] = {1, 2, 7, 9, 12, 5, 6, 16, 18, 21};
    /* O slide 34 omite a ordem de 1,4,10,16. Esta ordem reproduz o estado mostrado. */
    const int animacao[] = {7, 2, 5, 12, 9, 1, 4, 10, 16, 8};
    const int remocoes_animacao[] = {2, 8, 9, 5};
    const int exercicio[] = {15, 25, 47, 10, 8, 64, 53, 9, 13, 19, 14};
    const int remocoes_exercicio[] = {47, 25};
    printf("Árvore B de ordem m=%zu: páginas não raiz têm %zu a %zu chaves.\n",
           (size_t)ARVORE_B_ORDEM, (size_t)ARVORE_B_ORDEM, (size_t)ARVORE_B_MAX_CHAVES);
    /* Cada exemplo libera suas páginas antes de retornar sucesso ou falha. */
    if (!executar_exemplo("Slides 4 a 12: visão geral", visao_geral, 10U, NULL, 0U) ||
        !executar_exemplo("Slides 15 a 75: inserção e remoção", animacao, 10U, remocoes_animacao,
                          4U) ||
        !executar_exemplo("Slide 82: exercício", exercicio, 11U, remocoes_exercicio, 2U)) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
