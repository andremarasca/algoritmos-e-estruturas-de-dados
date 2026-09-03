#include <stdio.h>
#include <stdlib.h>

#include "DequeChar.h"

// Compara pares das extremidades ate encontrar uma diferenca ou chegar ao centro.
static int palavra_palindroma(const char *palavra)
{
    if (palavra == NULL) {
        return 0;
    }

    Deque *de = cria_Deque();
    if (de == NULL) {
        return 0;
    }

    // A ordem de insercao preserva a palavra do inicio para o final.
    for (size_t indice = 0U; palavra[indice] != '\0'; indice++) {
        if (!insere_final_Deque(de, palavra[indice])) {
            libera_Deque(de);
            return 0;
        }
    }

    while (tamanho_Deque(de) > 1) {
        char inicio;
        char final;
        consulta_inicio_Deque(de, &inicio);
        consulta_final_Deque(de, &final);

        // Uma unica diferenca comprova que a palavra nao e palindroma.
        if (inicio != final) {
            libera_Deque(de);
            return 0;
        }

        remove_inicio_Deque(de);
        remove_final_Deque(de);
    }

    libera_Deque(de);
    return 1;
}

int main(void)
{
    const char *palavras[] = {"arara", "radar", "abelha", "osso"};
    const size_t quantidade = sizeof(palavras) / sizeof(palavras[0]);

    for (size_t indice = 0U; indice < quantidade; indice++) {
        printf(
            "%s: %s\n",
            palavras[indice],
            palavra_palindroma(palavras[indice]) ? "palindromo" : "nao palindromo"
        );
    }

    return EXIT_SUCCESS;
}
