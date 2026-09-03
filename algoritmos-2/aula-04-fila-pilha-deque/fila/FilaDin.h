// Define os dados armazenados em cada elemento da fila.
struct aluno{
    int matricula;
    char nome[30];
    float n1,n2,n3;
};

typedef struct fila Fila;

// Cria uma fila vazia e devolve seu descritor.
Fila* cria_Fila(void);

// Libera todos os elementos e o descritor da fila.
void libera_Fila(Fila* fi);

// Insere um aluno no final da fila.
int insere_Fila(Fila* fi, struct aluno al);

// Remove o aluno localizado no inicio da fila.
int remove_Fila(Fila* fi);

// Exibe os elementos da fila do inicio para o final.
void imprime_Fila(Fila* fi);

// Copia o aluno do inicio sem remove-lo da fila.
int consulta_Fila(Fila* fi, struct aluno* al);
