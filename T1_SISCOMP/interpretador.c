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
#define SIZE_SEG 200

int main (int argc, char *argv[])
{
  //int fd1, segmentomsg, i;
  int fd1, segmentomsg;
  FILE* fd2;
  char *comando;
  
  printf("Criando areas de memoria\n");  

  //Criando a área de memória compartilhada
  segmentomsg = shmget(1234, sizeof(char)*200, IPC_CREAT | S_IRUSR | S_IWUSR);
  
  
  //Associando as área às variáveis pid e i
  comando = (char*) shmat(segmentomsg, 0, 0);

  
  strcpy(comando, VAZIO);
  

  //Abre os arquivos lista de execucao e saida para todos
  //Não usaremos O_TRUNC neste, pois perderia nosso arquivo.
  if ((fd1 = open("exec.txt", O_RDWR|O_CREAT, 0666)) == -1) {
    printf("Erro na abertura de exec.txt\n");
    exit(-1);
  }

  //Aqui usamos O_TRUNC pois queremos começar o arquivo do zero.
  //A flag O_TRUNC faz com que, se o arquivo existe(e seu acesso
  //permita escrita), ele seja truncado para comprimento 0.
  if ((fd2 = fopen("saidaInterpretador.txt", "w")) == NULL) {
    printf("Erro na abertura de saida.txt\n");
    exit(-2);
  }
  
  //Altera os pipes de entrada e saída, redirecionando-os para os arquivos
  //A entrada passa a ser o arquivo "exec.txt"
  if (dup2(fd1,0) == -1) {
    printf("Erro troca de entrada\n");
    exit(-4);
  }
  //A saida passa a ser o arquivo "saida.txt"
  //Retirado para colocar saída no terminal
  /*if (dup2(fd2,1) == -1) {
    printf("Erro troca de saida fd2\n");
    exit(-5);
  }*/
  
  //Enquanto ainda houverem linhas para serem lidas...
  while( scanf(" %199[^\n]", comando) == 1 ) {
    //Escrevendo informação
    printf("Comando obtido. Comando:\n%s\n", comando);
    fprintf(fd2, "Comando obtido. Comando:\n%s\n", comando);
    //Comando copiado. Já está na área de memória, portanto
    //o escalonador já possui acesso ao comando.
    while (strcmp(comando, VAZIO) != 0) {
      //Enquanto o escalonador não esvaziar o espaço, o
      //interpretador dorme.
      printf("Aguardando o escalonador limpar o comando\n");
      fprintf(fd2, "Aguardando o escalonador limpar o comando\n");
      sleep(1);
    }
  }
  strcpy(comando, FIM);
  shmdt(comando);
  //Sinalizar fim do interpretador

  // libera a memória compartilhada
  //shmctl (segmentomsg, IPC_RMID, 0);

  printf("Fim do interpretador.\n");
  fprintf(fd2, "Fim do interpretador.\n");
  return 0;
}
