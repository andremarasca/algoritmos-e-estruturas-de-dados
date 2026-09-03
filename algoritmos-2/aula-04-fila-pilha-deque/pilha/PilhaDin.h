// Define os dados armazenados em cada elemento da pilha.
struct aluno{
    int matricula;
    char nome[30];
    float n1,n2,n3;
};

typedef struct elemento* Pilha;

// Cria uma pilha vazia e devolve o ponteiro que representa seu topo.
Pilha* cria_Pilha(void);

// Libera todos os elementos e o ponteiro da pilha.
void libera_Pilha(Pilha* pi);

// Insere um aluno no topo da pilha.
int insere_Pilha(Pilha* pi, struct aluno al);

// Remove o aluno localizado no topo da pilha.
int remove_Pilha(Pilha* pi);

// Exibe os elementos da pilha do topo para a base.
void imprime_Pilha(Pilha* pi);

// Copia o aluno do topo sem remove-lo da pilha.
int consulta_Pilha(Pilha* pi, struct aluno* al);
