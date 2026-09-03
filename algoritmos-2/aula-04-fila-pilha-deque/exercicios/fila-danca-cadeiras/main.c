#include <stdio.h>
#include <stdlib.h>

#include "FilaParticipante.h"

// Movimenta a fila durante a musica e elimina quem estiver no inicio ao final.
static int executa_rodada(Fila *fi, int passos)
{
    for (int passo = 0; passo < passos; passo++) {
        struct participante participante;
        if (!consulta_Fila(fi, &participante)) {
            return 0;
        }

        // O participante passa pela frente e retorna ao final da fila.
        remove_Fila(fi);
        if (!insere_Fila(fi, participante)) {
            return 0;
        }
    }

    // O participante eliminado sai sem retornar ao final.
    struct participante eliminado;
    if (!consulta_Fila(fi, &eliminado)) {
        return 0;
    }
    printf("Eliminado: %s\n", eliminado.nome);
    return remove_Fila(fi);
}

int main(void)
{
    const struct participante participantes[] = {
        {1, "Ana"},
        {2, "Bruno"},
        {3, "Carla"},
        {4, "Diego"},
        {5, "Eva"}
    };
    const int passos_por_rodada[] = {2, 3, 1, 2};
    const size_t quantidade = sizeof(participantes) / sizeof(participantes[0]);
    Fila *fi = cria_Fila();

    if (fi == NULL) {
        return EXIT_FAILURE;
    }

    for (size_t indice = 0U; indice < quantidade; indice++) {
        if (!insere_Fila(fi, participantes[indice])) {
            libera_Fila(fi);
            return EXIT_FAILURE;
        }
    }

    // Cada rodada reduz a fila em exatamente um participante.
    size_t rodada = 0U;
    while (tamanho_Fila(fi) > 1) {
        if (!executa_rodada(fi, passos_por_rodada[rodada])) {
            libera_Fila(fi);
            return EXIT_FAILURE;
        }
        rodada++;
    }

    struct participante vencedor;
    if (consulta_Fila(fi, &vencedor)) {
        printf("Vencedor: %s\n", vencedor.nome);
    }

    libera_Fila(fi);
    return EXIT_SUCCESS;
}
