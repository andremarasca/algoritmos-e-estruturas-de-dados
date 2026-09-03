#include <stdio.h>
#include <stdlib.h>

#include "FilaVisitante.h"

// Retira do inicio somente os visitantes que cabem em uma viagem.
static void embarca_vagao(Fila *fi, int capacidade, int viagem)
{
    printf("Viagem %d:", viagem);
    for (int assento = 0; assento < capacidade; assento++) {
        struct visitante visitante;
        if (!consulta_Fila(fi, &visitante)) {
            // A ultima viagem pode terminar antes de ocupar todos os assentos.
            break;
        }
        printf(" %s", visitante.nome);
        remove_Fila(fi);
    }
    printf("\n");
}

int main(void)
{
    const struct visitante visitantes[] = {
        {101, "Ana"},
        {102, "Bruno"},
        {103, "Carla"},
        {104, "Diego"},
        {105, "Eva"},
        {106, "Fabio"},
        {107, "Gabi"},
        {108, "Hugo"},
        {109, "Iara"}
    };
    const size_t quantidade = sizeof(visitantes) / sizeof(visitantes[0]);
    const int capacidade = 4;
    Fila *fi = cria_Fila();

    if (fi == NULL) {
        return EXIT_FAILURE;
    }

    for (size_t indice = 0U; indice < quantidade; indice++) {
        if (!insere_Fila(fi, visitantes[indice])) {
            libera_Fila(fi);
            return EXIT_FAILURE;
        }
    }

    // Cada nova viagem processa o grupo que agora ocupa o inicio da fila.
    int viagem = 1;
    while (!fila_vazia(fi)) {
        embarca_vagao(fi, capacidade, viagem);
        viagem++;
    }

    libera_Fila(fi);
    return EXIT_SUCCESS;
}
