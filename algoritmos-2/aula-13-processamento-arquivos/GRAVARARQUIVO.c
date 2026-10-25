#include <stdio.h>
#define barra 92
/*
Modos de abertura de arquivos de texto:

r   Abre somente para leitura. O arquivo deve existir (read: ler).
r+  Abre para leitura e escrita, sem apagar o conteúdo. O arquivo deve existir.
w   Abre somente para escrita. Cria o arquivo ou apaga o conteúdo existente
    (write: escrever).
w+  Abre para leitura e escrita. Cria o arquivo ou apaga o conteúdo existente.
a   Abre somente para escrita. Cria o arquivo se necessário e acrescenta toda
    escrita ao final, preservando o conteúdo (append: acrescentar).
a+  Abre para leitura e escrita. Cria o arquivo se necessário e acrescenta toda
    escrita ao final. Posicione explicitamente a leitura com fseek ou rewind.

Nos modos com '+', para alternar escrita e leitura, use fflush ou uma função
de posicionamento após escrever. Após ler, use uma função de posicionamento
antes de escrever, exceto quando a leitura tiver encontrado o fim do arquivo.

fopen()   Abre um arquivo e retorna FILE *, ou NULL em caso de falha.
          FILE representa o fluxo usado pela biblioteca para acessar o arquivo.
fclose()  Fecha o fluxo, enviando os dados pendentes de saída. Retorna zero
          em caso de sucesso ou EOF em caso de erro.
putc()    Escreve um caractere no fluxo. Pode ser implementada como macro,
          que pode avaliar o argumento do fluxo mais de uma vez.
fputc()   Escreve um caractere no fluxo, como putc, mas é uma função.
getc()    Lê um caractere como unsigned char convertido para int, ou retorna
          EOF ao encontrar o fim do arquivo ou um erro. Pode ser uma macro
          que avalia o argumento do fluxo mais de uma vez.
fgetc()   Lê um caractere do fluxo, como getc, mas é uma função.
fseek()   Reposiciona o fluxo. Em arquivos de texto, use deslocamento zero ou
          uma posição obtida por ftell com SEEK_SET, sem supor bytes arbitrários.
fprintf() Escreve dados formatados no fluxo, assim como printf escreve em stdout.
fscanf()  Lê dados formatados do fluxo, assim como scanf lê de stdin.
feof()    Consulta o indicador de fim do arquivo, ativado por uma tentativa
          de leitura que encontra esse fim. Não prevê a próxima leitura.
ferror()  Consulta o indicador de erro do fluxo. Retorna um valor diferente
          de zero quando o indicador está ativo.
rewind()  Volta ao início do fluxo e limpa os indicadores de erro e fim do arquivo.
remove()  Remove o arquivo identificado pelo nome informado.
fflush()  Envia os dados pendentes do buffer de saída ao ambiente de execução.
          Não garante gravação física no disco. Não use para limpar a entrada:
          em C padrão, seu uso não é definido para fluxos de entrada.
*/

/*
Duplica as barras invertidas do endereço recebido.
A barra de valor 92 representa '\\' na codificação usada neste exemplo.
A entrada deve terminar em '\0', ocupar menos de 500 bytes e caber no vetor
original após a duplicação. '\0' marca o fim de uma string em C.
Atenção: barras digitadas pelo usuário não precisam ser duplicadas para fopen.
Essa duplicação só é necessária ao escrever barras em literais no código-fonte.
*/
void arruma_endereco (char string[])
{
    int i,j;
    char fila[500];
    /* Preserva o endereço original antes de expandir as barras no vetor recebido. */
    for(i = 0; string[i] != '\0'; i++)
    {
        fila[i] = string[i];
    }
    fila[i] = string[i];
    j = 0;
    /* Reconstrói o endereço, acrescentando uma cópia de cada barra invertida. */
    for(i = 0; fila[i] != '\0'; i++)
    {
        /* Reserva a posição extra usada pela duplicação da barra. */
        if(fila[i] == barra) string[j++] = fila[i];
        string[j++] = fila[i];
    }
    string[j++] = fila[i];
}

/* Solicita um endereço até abrir o arquivo e acrescenta uma linha ao final. */
int main ()
{
    FILE *arquivo;
    char endereco[500];
    /* Repete a solicitação quando fopen não consegue abrir ou criar o arquivo. */
    do
    {
        printf("Informe o local do arquivo: ");
        /* %s lê até um espaço. Sem largura máxima, a entrada pode exceder o vetor. */
        scanf("%s", endereco);
        arruma_endereco(endereco);
        arquivo = fopen(endereco, "a+");
        /* Informa a falha para que outro endereço possa ser solicitado. */
        if(arquivo == NULL) printf("O endereco [%s] e invalido!\n",endereco);
    } while(arquivo == NULL);
    fprintf(arquivo,"Gravado no arquivo.\n");
    fclose(arquivo);
    return 0;
}
