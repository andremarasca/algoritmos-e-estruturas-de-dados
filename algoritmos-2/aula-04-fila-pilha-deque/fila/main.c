#include <stdio.h>
#include <stdlib.h>

#include "FilaDin.h"

int main(void){
    // Os alunos entram na fila na mesma ordem em que foram declarados.
    struct aluno aluno_32 = {32, "Ana", 8.0f, 7.5f, 9.0f};
    struct aluno aluno_45 = {45, "Bruno", 6.5f, 7.0f, 8.0f};
    struct aluno aluno_18 = {18, "Carla", 9.0f, 8.5f, 9.5f};
    struct aluno aluno_consultado;
    Fila* fi = cria_Fila();

    if(fi == NULL){
        printf("Erro ao criar a fila.\n");
        return EXIT_FAILURE;
    }

    // Toda insercao ocorre no final da fila.
    insere_Fila(fi, aluno_32);
    insere_Fila(fi, aluno_45);
    insere_Fila(fi, aluno_18);

    printf("Fila apos as insercoes:\n");
    imprime_Fila(fi);

    if(consulta_Fila(fi, &aluno_consultado))
        printf("Inicio da fila: %d\n", aluno_consultado.matricula);

    remove_Fila(fi);
    printf("\nFila apos uma remocao:\n");
    imprime_Fila(fi);

    // A liberacao evita que os nos restantes permaneçam ocupando memoria.
    libera_Fila(fi);
    return EXIT_SUCCESS;
}
