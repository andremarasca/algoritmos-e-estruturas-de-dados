#include <stdio.h>
#include <stdlib.h>

#include "PilhaDin.h"

int main(void){
    // O ultimo aluno inserido sera o primeiro disponivel no topo.
    struct aluno aluno_32 = {32, "Ana", 8.0f, 7.5f, 9.0f};
    struct aluno aluno_45 = {45, "Bruno", 6.5f, 7.0f, 8.0f};
    struct aluno aluno_18 = {18, "Carla", 9.0f, 8.5f, 9.5f};
    struct aluno aluno_consultado;
    Pilha* pi = cria_Pilha();

    if(pi == NULL){
        printf("Erro ao criar a pilha.\n");
        return EXIT_FAILURE;
    }

    // Toda insercao ocorre no topo da pilha.
    insere_Pilha(pi, aluno_32);
    insere_Pilha(pi, aluno_45);
    insere_Pilha(pi, aluno_18);

    printf("Pilha do topo para a base:\n");
    imprime_Pilha(pi);

    if(consulta_Pilha(pi, &aluno_consultado))
        printf("Topo da pilha: %d\n", aluno_consultado.matricula);

    remove_Pilha(pi);
    printf("\nPilha apos uma remocao:\n");
    imprime_Pilha(pi);

    // A liberacao evita que os nos restantes permanecam ocupando memoria.
    libera_Pilha(pi);
    return EXIT_SUCCESS;
}
