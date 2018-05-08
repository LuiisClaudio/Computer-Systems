#include <sys/wait.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define VAZIO ""
#define FIM "fim"

//Função para quebrar as linhas do arquivo exec.txt em um
//vetor de strings.
char** breakString(char* string, int* n);

int main (int argc, char *argv[])
{
  int fd1, fd2, quantidadeParametros, segmentomsg;
  FILE* fd3;
  int inicio, fim, skip, pid, programasExecutando, anterior;
  char* commands[]={NULL}, *mensagem, **linha, comando[200];
  int vpid[60], i, j;
  
  printf("Pegando area de memoria\n");
  //Criando as áreas de memória compartilhada
  segmentomsg = shmget(123, 200*sizeof(char), 0666 | IPC_CREAT);//IPC_EXCL | S_IRUSR | S_IWUSR);
  if(segmentomsg < 0)
  {
      printf("Failed to create shm\n");
      exit(1);
  }

  mensagem = (char*) shmat(segmentomsg, 0, 0);
  if(mensagem < 0)
  {
        printf("Falha no attach");
        exit(1);
  }
  //printf("---->>>>%s\n", mensagem); //@@@
  //Real Time - colocar todos os pids como -1, como se não houvesse
  //processo para ser executado naquele segundo
  for(i=0;i<60;i++)
    vpid[i]=-1;
  
  if ((fd3 = fopen("saidaEscalonadorRT.txt", "w")) == NULL) {
    printf("Erro na abertura de saida.txt\n");
    exit(-2);
  }
  
  if ((fd2 = open("saida.txt", O_RDWR|O_CREAT|O_TRUNC, 0666)) == -1) {
    perror("Error open() fd1\n");
    exit(-1);
  }
  
  //Enquanto ainda houverem linhas para serem lidas...
  while( strcmp(mensagem, FIM) != 0 ) {
    while( strcmp(mensagem, VAZIO) == 0) {
      printf("Aguardando o interpretador preencher o comando\n");
      fprintf(fd3, "Aguardando o interpretador preencher o comando\n");
      sleep(1);
    }
    printf("\nLeitura feita\n");
    fprintf(fd3, "\nLeitura feita\n");
    //Após a leitura da linha, quebra-se a string
    //A função breakString deve quebrar os espaços da string,
    //deixando os parâmetros em ordem:
    //Para RT: [0](nome) [1](inicio) [2](duração)
    //printf("\n00000\n");//@@@
    //return 0; //###
    //printf("---->>>>%s\n", mensagem); //@@@
    linha = breakString(mensagem, &quantidadeParametros);
    strcpy(mensagem, VAZIO);
    //Se a variável skip for diferente de zero representa que já
    //existe um programa rodando no intervalo do programa sendo
    //iniciado, portanto este será ignorado
    skip=0;
    //Com a quebra da string, teremos o nome do programa a ser executado
    //sempre no índice 0
    printf("nome: %s\ninicio: %s\nduracao: %s\n", linha[0], linha[1], linha[2]);
    fprintf(fd3, "nome: %s\ninicio: %s\nduracao: %s\n", linha[0], linha[1], linha[2]);
    //Por ser RT, precisamos verificar se o programa irá
    //sobrepor algum outro já existente ou se I+D>=60.
    //A variável skip irá determinar se devemos pular o programa
    //ou não.

    //printf("\n1111111111\n");//@@@
    //return 0; //Tirar isso ###
    
    //printf("Linha 1:%s\n", linha[1]);
    inicio = atoi(linha[1]);
    //printf("Linha 2:%s\n", linha[2]);
    fim = atoi(linha[2]);
    fim=inicio+fim-1;
    //printf("Aqui 1");
    if (fim > 60)
      skip++;
    //printf("Aqui 2");
    for(i=inicio;i<=fim;i++) {
      if(vpid[i]!=-1) {
        skip++;
        break;
      }
    }    
  
    //printf("\n222\n");//@@@    
    //return 0; //tirar depis ###
    if ( skip == 0 && !(pid = fork()) ) {
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
    else if (skip == 0) {
      //Por ser RT, devemos guardar o pid do processo em cada
      //segundo que ele deve estar executando.
      for(i=inicio;i<=fim;i++) {
        vpid[i]=pid;
        printf("vpid[%d]= %d\n", i, pid);
        fprintf(fd3, "vpid[%d]= %d\n", i, pid);
      }
      //Intervalo de 1 segundo entre cada processo.
      sleep(1);
    }
    else {
      printf("Tempo de operacao do programa coincide com outro ou intervalo invalido.\nPrograma abortado.\n");
      fprintf(fd3, "Tempo de operacao do programa coincide com outro ou intervalo invalido.\nPrograma abortado.\n");
      sleep(1);
    }
    for(i=0;i<quantidadeParametros;i++)
      free(linha[i]);
    free(linha);
  }
  shmdt(mensagem);
  printf("\nIniciando escalonamento dos processos:\n\n");
  fprintf(fd3, "\nIniciando escalonamento dos processos:\n\n");
  shmctl(segmentomsg, IPC_RMID, 0);
  
  //Código do respectivo escalonador
  
  programasExecutando = 1;
  i = time(0);
  
  printf("Horario no relogio: %d segundos\n", i%60);
  fprintf(fd3, "Horario no relogio: %d segundos\n", i%60);
  
  for(i=i%60;i<60;i++) {
    anterior = vpid[abs(i+59)%60];
    if (vpid[i] != anterior && anterior != -1) {
      if (waitpid(anterior, 0, 0 | WNOHANG) != 0) {
        printf("Processo de pid %d terminou. Removendo da lista.\n", anterior);
        fprintf(fd3, "Processo de pid %d terminou. Removendo da lista.\n", anterior);
        for(j=abs(i+59)%60;j>=0 && vpid[j] == anterior;j--)
          vpid[j] = -1;
      }
      else {
        kill(anterior, SIGSTOP);
        printf("Pausando processo %d aos %d segundos.\n", anterior, i);
        fprintf(fd3, "Pausando processo %d aos %d segundos.\n", anterior, i);
      }
      if (vpid[i] != -1) {
        kill(vpid[i], SIGCONT);
        j=i;
        while(vpid[j] == vpid[i] && j<60)
          j++;
        printf("Continuando processo %d aos %d segundos durante %d segundos.\n", vpid[i], i, j-i);
        fprintf(fd3, "Continuando processo %d aos %d segundos durante %d segundos.\n", vpid[i], i, j-i);
        programasExecutando++;
      }
    }
    else if (anterior == -1 && vpid[i] != -1) {
      kill(vpid[i], SIGCONT);
      j=i;
      while(vpid[j] == vpid[i] && j<60)
        j++;
      printf("Continuando processo %d aos %d segundos durante %d segundos.\n", vpid[i], i, j-i);
      fprintf(fd3, "Continuando processo %d aos %d segundos durante %d segundos.\n", vpid[i], i, j-i);
      programasExecutando++;
    }
    else if (vpid[i] == -1) {
      printf("Nao ha processo comecando aos %d segundos.\n", i);
      fprintf(fd3, "Nao ha processo comecando aos %d segundos.\n", i);
    }
    if (i==59 && programasExecutando != 0) {
      i=-1;
      programasExecutando = 0;
    }
    else if (i==59 && programasExecutando == 0)
      break;
    sleep(1);
  }
  
  printf("Fim do escalonamento.\n");
  fprintf(fd3, "Fim do escalonamento.\n");
  
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
