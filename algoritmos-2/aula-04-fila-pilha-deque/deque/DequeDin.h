// Define os dados armazenados em cada elemento do deque.
struct aluno{
    int matricula;
    char nome[30];
    float n1,n2,n3;
};

typedef struct deque Deque;

// Cria um deque vazio e devolve seu descritor.
Deque* cria_Deque(void);

// Libera todos os elementos e o descritor do deque.
void libera_Deque(Deque* de);

// Insere um aluno no inicio do deque.
int insere_inicio_Deque(Deque* de, struct aluno al);

// Insere um aluno no final do deque.
int insere_final_Deque(Deque* de, struct aluno al);

// Remove o aluno localizado no inicio do deque.
int remove_inicio_Deque(Deque* de);

// Remove o aluno localizado no final do deque.
int remove_final_Deque(Deque* de);

// Copia o aluno do inicio sem remove-lo do deque.
int consulta_inicio_Deque(Deque* de, struct aluno* al);

// Copia o aluno do final sem remove-lo do deque.
int consulta_final_Deque(Deque* de, struct aluno* al);

// Exibe os elementos do deque do inicio para o final.
void imprime_Deque(Deque* de);
