#include <stdio.h>
#include <stdlib.h>

#include "DequeDin.h"

int main(void){
    // O deque permite inserir e remover alunos em suas duas extremidades.
    struct aluno aluno_32 = {32, "Ana", 8.0f, 7.5f, 9.0f};
    struct aluno aluno_45 = {45, "Bruno", 6.5f, 7.0f, 8.0f};
    struct aluno aluno_18 = {18, "Carla", 9.0f, 8.5f, 9.5f};
    struct aluno aluno_consultado;
    Deque* de = cria_Deque();

    if(de == NULL){
        printf("Erro ao criar o deque.\n");
        return EXIT_FAILURE;
    }

    // As insercoes demonstram o uso independente do inicio e do final.
    insere_final_Deque(de, aluno_32);
    insere_final_Deque(de, aluno_45);
    insere_inicio_Deque(de, aluno_18);

    printf("Deque do inicio para o final:\n");
    imprime_Deque(de);

    if(consulta_inicio_Deque(de, &aluno_consultado))
        printf("Inicio do deque: %d\n", aluno_consultado.matricula);
    if(consulta_final_Deque(de, &aluno_consultado))
        printf("Final do deque: %d\n", aluno_consultado.matricula);

    // Uma remocao em cada extremo deixa somente o elemento central.
    remove_inicio_Deque(de);
    remove_final_Deque(de);
    printf("\nDeque apos remover as duas extremidades:\n");
    imprime_Deque(de);

    // A liberacao evita que os nos restantes permanecam ocupando memoria.
    libera_Deque(de);
    return EXIT_SUCCESS;
}
