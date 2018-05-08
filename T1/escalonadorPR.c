#include <sys/wait.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <math.h>

#define VAZIO ""
#define FIM "fim"

//Struct para programas de prioridade
typedef struct tipoPrioridade {
  int pid;
  int prioridade;
} tpPrioridade;

//Função para quebrar as linhas do arquivo exec.txt em um
//vetor de strings.
char** breakString(char* string, int* n);

//Função de comparação para qsort
int compara(const void* a, const void* b);

int main (int argc, char *argv[])
{
  int fd1, fd2, quantidadeParametros, segmentomsg;
  FILE* fd3;
  int pid, j;
  char* commands[]={NULL}, *mensagem, **linha, comando[200];
  int i, num;
  tpPrioridade vPpid[100];
  
  printf("Pegando area de memoria\n");
  //Criando as áreas de memória compartilhada
  segmentomsg = shmget(1234, 200*sizeof(char), IPC_EXCL | S_IRUSR | S_IWUSR);
  
  mensagem = (char*) shmat(segmentomsg, 0, 0);
  
  if ((fd3 = fopen("saidaEscalonadorPR.txt", "w")) == NULL) {
    printf("Erro na abertura de saida.txt\n");
    exit(-2);
  }
  
  if ((fd2 = open("saida.txt", O_RDWR|O_CREAT|O_TRUNC, 0666)) == -1) {
    perror("Error open() fd1\n");
    exit(-1);
  }
  
  //"i" será usado para guardar o pid de cada processo e
  //a quantidade de processos(em casos não-RT).
  i=0;
  //Enquanto ainda houverem linhas para serem lidas...
  while( strcmp(mensagem, FIM) != 0 ) {
    while( strcmp(mensagem, VAZIO) == 0) {
      printf("Aguardando o interpretador preencher o comando\n");
      fprintf(fd3, "Aguardando o interpretador preencher o comando\n");
      sleep(1);
    }
    printf("\nLeitura %d feita\n", i+1);
    fprintf(fd3, "\nLeitura %d feita\n", i+1);
    //Após a leitura da linha, quebra-se a string
    //A função breakString deve quebrar os espaços da string,
    //deixando os parâmetros em ordem:
    //Para Prioridade: [0](nome) [1](prioridade)
    linha = breakString(mensagem, &quantidadeParametros);
    strcpy(mensagem, VAZIO);
    //Com a quebra da string, teremos o nome do programa a ser executado
    //sempre no índice 0
    printf("nome: %s\nprioridade: %s\n", linha[0], linha[1]);
    fprintf(fd3, "nome: %s\nprioridade: %s\n", linha[0], linha[1]);
    if ( !(pid = fork()) ) {
      //Processo filho
      //Redirecionar a entrada do filho para a entrada
      //do programa a ser executado. O nome do arquivo
      //de entrada é sempre "(nomeDoPrograma)entrada.txt"
      strcpy(comando, linha[0]);
      strcat(comando, "entrada.txt");
      //Com o nome do arquivo, podemos abri-lo e redirecionar
      //a entrada.
      if ((fd1 = open(comando, O_RDWR|O_CREAT, 0666)) == -1) {
        perror("Error open() fd1\n");
        exit(-1);
      }
      if (dup2(fd1,0) == -1) {
        perror("Erro dup2 fd1\n");
        exit(-4);
      }
      if (dup2(fd2,1) == -1) {
        perror("Erro dup2 fd2\n");
        exit(-5);
      }
      //Agora já redirecionamos a entrada do novo programa,
      //podemos colocá-lo para rodar.
      strcpy(comando,"./");
      strcat(comando, linha[0]);
      raise(SIGSTOP);
      execve(comando, commands, 0);
    }
    else {
      //Por ser PR, devemos colocar a prioridade na struct
      vPpid[i].pid=pid;
      vPpid[i].prioridade = atoi(linha[1]);
      printf("vPpid[%d].pid= %d\nvPpid[%d].prioridade = %d\n", i, vPpid[i].pid, i, vPpid[i].prioridade);
      fprintf(fd3, "vPpid[%d].pid= %d\nvPpid[%d].prioridade = %d\n", i, vPpid[i].pid, i, vPpid[i].prioridade);
      i++;
      //Intervalo de 1 segundo entre cada processo.
      sleep(1);
    }
    for(j=0;j<quantidadeParametros;j++)
      free(linha[j]);
    free(linha);
  }
  shmdt(mensagem);
  printf("\nIniciando escalonamento dos processos:\n\n");
  fprintf(fd3, "\nIniciando escalonamento dos processos:\n\n");
  shmctl(segmentomsg, IPC_RMID, 0);
  
  //Código do respectivo escalonador
  
  num=i;
  
  qsort(vPpid, num, sizeof(tpPrioridade), compara);
  
  for(i=0;i<num;i++) {
    printf("Continuando processo de pid %d\n", vPpid[i].pid);
    fprintf(fd3, "Continuando processo de pid %d\n", vPpid[i].pid);
    kill(vPpid[i].pid, SIGCONT);
    waitpid(vPpid[i].pid, 0, 0);
    printf("Processo de pid %d terminou\n", vPpid[i].pid);
    fprintf(fd3, "Processo de pid %d terminou\n", vPpid[i].pid);
  }
  
  fclose(fd3);
  return 0;
}

char** breakString(char* string, int* n)
{
  char** linha;
  int i, j=0, k, *tamanhoPalavras;
  
  //Vamos contar a quantidade de espaços para saber
  //qual a politica que devemos utilizar e quantos
  //espaços devemos alocar para o vetor de strings final.
  //"i" começa em 5 pois sempre descartamos "Exec ", já
  //que sempre serão os 5 primeiros caracteres de toda linha.
  (*n)=1;
  for(i=5;string[i] != '\0';i++) {
    if (string[i] == ' ')
      (*n)++;
  }
  
  //Agora sabemos quantos strings teremos, mas ainda não
  //sabemos o tamanho de cada um. Iremos alocar um vetor de
  //int para guardar o tamanho de cada um deles
  tamanhoPalavras = (int*) malloc((*n)*sizeof(int));
  
  //Temos o vetor de tamanhos. Agora vamos contar o tamanho
  //de cada palavra.
  for(i=5;string[i] != '\0';i++) {
    //Na contagem de caracteres, devemos pular os caracteres invalidos
    //O espaço nos faz pular de palavra
    if ( string[i] == ' ' )
      j++;
    //Os outros caracteres não devemos contar como tamanho
    else if ( !(string[i] == '=' || (j == 1 && (string[i] == 'P' || string[i] == 'R' || string[i] == 'I')) || (j == 2 && string[i] == 'D')) )
      tamanhoPalavras[j]++;
  }
  
  //Com o tamanho exato de cada palavra, podemos alocar o
  //vetor final de cada linha.
  linha = (char**) malloc((*n)*sizeof(char*));
  for(i=0;i<(*n);i++) {
    linha[i] = (char*) malloc((tamanhoPalavras[i]+1)*sizeof(char));
  }
  
  //Agora precisamos copiar as palavras para as casas do
  //vetor linha.
  j=0;
  k=0;
  for(i=5;string[i] != '\0';i++) {
    if(string[i] != ' ' && !(string[i] == '=' || (j == 1 && (string[i] == 'P' || string[i] == 'R' || string[i] == 'I')) || (j == 2 && string[i] == 'D')) && k < tamanhoPalavras[j]) {
      linha[j][k]=string[i];
      k++;
    }
    else if (k > tamanhoPalavras[j]) {
      printf("Erro no tamanho da palavra\n");
      exit(0);
    }
    else if (string[i] == ' ') {
      linha[j][k] = '\0';
      j++;
      k=0;
    }
  }
  linha[j][k] = '\0';
  
  //Com isso, temos todas as palavras copiadas corretamente
  free(tamanhoPalavras);
  return linha;
}

int compara(const void* a, const void* b)
{
  tpPrioridade elem1 = *((tpPrioridade*)a);
  tpPrioridade elem2 = *((tpPrioridade*)b);
  if (elem1.prioridade > elem2.prioridade)
    return 1;
  if (elem1.prioridade < elem2.prioridade)
    return -1;
  return 0;
}
