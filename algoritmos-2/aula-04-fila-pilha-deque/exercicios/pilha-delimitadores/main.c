#include <stdio.h>
#include <stdlib.h>

#include "PilhaChar.h"

// Identifica os caracteres que precisam ser guardados na pilha.
static int abre_delimitador(char caractere)
{
    return caractere == '(' || caractere == '[' || caractere == '{';
}

// Identifica os caracteres que exigem uma abertura correspondente no topo.
static int fecha_delimitador(char caractere)
{
    return caractere == ')' || caractere == ']' || caractere == '}';
}

// Confirma se a abertura do topo pertence ao fechamento atual.
static int delimitadores_correspondem(char abertura, char fechamento)
{
    return (abertura == '(' && fechamento == ')')
        || (abertura == '[' && fechamento == ']')
        || (abertura == '{' && fechamento == '}');
}

// Usa a pilha para manter somente as aberturas que ainda aguardam fechamento.
static int expressao_balanceada(const char *expressao)
{
    if (expressao == NULL) {
        return 0;
    }

    Pilha *pi = cria_Pilha();
    if (pi == NULL) {
        return 0;
    }

    for (size_t indice = 0U; expressao[indice] != '\0'; indice++) {
        const char atual = expressao[indice];

        if (abre_delimitador(atual)) {
            if (!insere_Pilha(pi, atual)) {
                libera_Pilha(pi);
                return 0;
            }
        } else if (fecha_delimitador(atual)) {
            char topo;
            // Um fechamento exige uma abertura compativel no topo.
            if (!consulta_Pilha(pi, &topo) || !delimitadores_correspondem(topo, atual)) {
                libera_Pilha(pi);
                return 0;
            }
            remove_Pilha(pi);
        }
    }

    // Qualquer abertura restante indica que faltou um fechamento.
    char caractere_restante;
    const int balanceada = !consulta_Pilha(pi, &caractere_restante);
    libera_Pilha(pi);
    return balanceada;
}

int main(void)
{
    const char *expressoes[] = {
        "(a+b)",
        "([a+b] * {c-d})",
        "([)]",
        "((a+b)"
    };
    const size_t quantidade = sizeof(expressoes) / sizeof(expressoes[0]);

    for (size_t indice = 0U; indice < quantidade; indice++) {
        printf(
            "%s: %s\n",
            expressoes[indice],
            expressao_balanceada(expressoes[indice]) ? "balanceada" : "nao balanceada"
        );
    }

    return EXIT_SUCCESS;
}
