#include <sys/wait.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <time.h>
#include <assert.h>

#define VAZIO ""
#define FIM "fim"
#define SIZE_SEG 200
#define TRUE 1
#define EVER ;;
#define FALSE 0

typedef struct tipoPrioridade {
  int pid;
  int prioridade;
} tpPrioridade;

char** breakStringRT(char* string, int* n);
char** breakStringPR(char* string, int* n);
char** breakStringRR(char* string, int* n);
int compara(const void* a, const void* b);
int words(const char *sentence);


int main (int argc, char *argv[])
{
    int segmentomsg, pid1, pid2, pid3;
    char *mensagem;
    int seg, *flag_escalonador;
    int seg_segundos, *controle_tempo;


    printf("Criando areas de memoria\n");
    //Criando a área de memória compartilhada

    seg = shmget(IPC_PRIVATE, sizeof(int), 0666 | IPC_CREAT);//IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    //Flag usada para saber quem deve ser escalonado
    flag_escalonador = (int*) shmat(seg, 0, 0);
    *flag_escalonador = 0;

    //Controlar o tempo ocupado pelo escalonador Real Timing
    seg_segundos = shmget(IPC_PRIVATE, sizeof(int)*60, 0666 | IPC_CREAT);//IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    controle_tempo = (int*) shmat(seg_segundos, 0, 0);
    for(int tempo = 0; tempo < 60; tempo++)
    {
        controle_tempo[tempo] = 0;
    }


    segmentomsg = shmget(123, SIZE_SEG*sizeof(char), 0666 | IPC_CREAT);//IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
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
    
    pid1 = fork();
    if(pid1 == 0)
    {
        //EscalonadorRT
        int fd1, fd2, quantidadeParametros;
        FILE* fd3;
        int inicio, fim, skip, pid, programasExecutando, anterior;
        char* commands[]={NULL}, **linha, comando[200];
        int vpid[60], i, j;
        //printf("\n\n\t\tReal Timing\n\n");
        //Real Time - colocar todos os pids como -1, como se não houvesse
        //processo para ser executado naquele segundo
        for(i=0;i<60;i++)
            vpid[i]=-1;
        
        if ((fd3 = fopen("saidaEscalonadorRT.txt", "w")) == NULL) {
            printf("Erro na abertura de saidaEscalonadorRT.txt\n");
            exit(-2);
        }
        
        if ((fd2 = open("saida.txt", O_RDWR|O_CREAT|O_TRUNC, 0666)) == -1) {
            perror("Error open() fd1\n");
            exit(-1);
        }
        //Enquanto ainda houverem linhas para serem lidas...
        while(strcmp(mensagem, FIM) != 0)//strcmp(mensagem, FIM) != 0 ) 
        {
            if(words(mensagem) == 4)
            {
                printf("\n\n\t\tReal Timing %s\n\n", mensagem);
                 *flag_escalonador = *flag_escalonador | 0x10;
                while( strcmp(mensagem, VAZIO) == 0) 
                {
                    printf("Aguardando o interpretador preencher o comando\n");
                    fprintf(fd3, "Aguardando o interpretador preencher o comando\n");
                    sleep(1);
                }
                printf("\nRTLeitura feita\n");
                fprintf(fd3, "\nRTLeitura feita\n");
                //Após a leitura da linha, quebra-se a string
                //A função breakString deve quebrar os espaços da string,
                //deixando os parâmetros em ordem:
                //Para RT: [0](nome) [1](inicio) [2](duração)
                linha = breakStringRT(mensagem, &quantidadeParametros);
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
                        //Nao pode escalonar nesse instante
                        controle_tempo[i] = TRUE;
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
        }
        sleep(1);//Para sincronizar os escalonamentos
        //printf("\nIniciando escalonamento dos processos:\n\n");
        //fprintf(fd3, "\nIniciando escalonamento dos processos:\n\n");
        
        //Código do respectivo escalonador
        
        programasExecutando = 1;
        i = time(0);
        
        printf("Horario no relogio: %d segundos\n", i%60);
        fprintf(fd3, "Horario no relogio: %d segundos\n", i%60);
        
        for(i=i%60;i<60;i++) 
        {
            //printf("\tProgramas executando: %d\n", programasExecutando);
            anterior = vpid[abs(i+59)%60];
            if (vpid[i] != anterior && anterior != -1) {
                if (waitpid(anterior, 0, 0 | WNOHANG) != 0) {
                    programasExecutando--;
                    printf("Processo REAL TIMING de pid %d terminou. Removendo da lista.\n", anterior);
                    fprintf(fd3, "Processo de REAL TIMING pid %d terminou. Removendo da lista.\n", anterior);
                    for(j=abs(i+59)%60;j>=0 && vpid[j] == anterior;j--)
                    {
                        vpid[j] = -1;
                        controle_tempo[j] = 0;
                    }
                }
                else 
                {
                    kill(anterior, SIGSTOP);
                    printf("Pausando processo REAL TIMING %d aos %d segundos.\n", anterior, i);
                    fprintf(fd3, "Pausando processo REAL TIMING %d aos %d segundos.\n", anterior, i);
                }
                if (vpid[i] != -1) 
                {
                    kill(vpid[i], SIGCONT);
                    j=i;
                    while(vpid[j] == vpid[i] && j<60)
                    j++;
                    printf("Continuando processo REAL TIMING %d aos %d segundos durante %d segundos.\n", vpid[i], i, j-i);
                    fprintf(fd3, "Continuando processo REAL TIMING %d aos %d segundos durante %d segundos.\n", vpid[i], i, j-i);
                    programasExecutando++;
                }
            }
            else if (anterior == -1 && vpid[i] != -1) {
                kill(vpid[i], SIGCONT);
                j=i;
                while(vpid[j] == vpid[i] && j<60)
                    j++;
                printf("Continuando processo REAL TIMING %d aos %d segundos durante %d segundos.\n", vpid[i], i, j-i);
                fprintf(fd3, "Continuando processo REAL TIMING %d aos %d segundos durante %d segundos.\n", vpid[i], i, j-i);
                programasExecutando++;
            }
            else if (vpid[i] == -1) {
                //printf("Nao ha processo comecando aos %d segundos.\n", i);
                //fprintf(fd3, "Nao ha processo comecando aos %d segundos.\n", i);
            }
            if (i==59 && programasExecutando != 0) {
                i=-1;
                programasExecutando = 0;
            }
            //else if (i==59 && programasExecutando == 0)
            else if(programasExecutando == 0)
            {
                *flag_escalonador = *flag_escalonador & 0xEF; //1110 1111
                *flag_escalonador = *flag_escalonador | 0x1;
                break;
                
            }
            sleep(1);
        }
        
        //printf("FIM DO ESCALONAMENTO REAL TIMING\n");
        //fprintf(fd3, "Fim do escalonamento.\n");
        
        fclose(fd3);
        return 0;
        //FimEscalonadorRT
    }
    else
    {
        //EscalonadorPR
        pid2 = fork();
        if(pid2 == 0)
        {
            
            printf("pid2\n");
            int fd1, fd2, quantidadeParametros;
            FILE* fd3;
            int pid, j;
            char* commands[]={NULL}, **linha, comando[200];
            int i, num;
            tpPrioridade vPpid[100];
            //printf("\n\n\t\tPrioridade\n\n");
            //printf("Pegando area de memoria\n");
            //Criando as áreas de memória compartilhada
            
            if ((fd3 = fopen("saidaEscalonadorPR.txt", "w")) == NULL) {
                printf("Erro na abertura de saidaEscalonadorPR.txt\n");
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
            while(strcmp(mensagem, FIM) != 0) 
            {
                sleep(1);
                if(words(mensagem) == 3)
                {
                    printf("\n\n\t\tPrioridade %s\n\n", mensagem);
                    *flag_escalonador = *flag_escalonador | 0x20;
                    while( strcmp(mensagem, VAZIO) == 0) 
                    {
                        printf("Aguardando o interpretador preencher o comando\n");
                        fprintf(fd3, "Aguardando o interpretador preencher o comando\n");
                        sleep(1);
                    }
                    printf("\nPR Leitura %d feita\n", i+1);
                    fprintf(fd3, "\nPR Leitura %d feita\n", i+1);
                    //Após a leitura da linha, quebra-se a string
                    //A função breakString deve quebrar os espaços da string,
                    //deixando os parâmetros em ordem:
                    //Para Prioridade: [0](nome) [1](prioridade)
                    linha = breakStringPR(mensagem, &quantidadeParametros);
                    strcpy(mensagem, VAZIO);
                    //Com a quebra da string, teremos o nome do programa a ser executado
                    //sempre no índice 0
                    printf("nome: %s\nprioridade: %s\n", linha[0], linha[1]);
                    fprintf(fd3, "nome: %s\nprioridade: %s\n", linha[0], linha[1]);
                    if ( !(pid = fork()) ) 
                    {
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
                    else 
                    {
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
            }
            //Esperando todos os processos lerem o interpretador
            sleep(1);
            //printf("\nIniciando escalonamento dos processos:\n\n");
            //fprintf(fd3, "\nIniciando escalonamento dos processos:\n\n");
            

            //Código do respectivo escalonador
            num=i;
            
            qsort(vPpid, num, sizeof(tpPrioridade), compara);
            
            for(i=0;i<num;i++) {
                printf("Continuando processo de pid %d PRIORIDADE: %d\n", vPpid[i].pid, vPpid[i].prioridade);
                fprintf(fd3, "Continuando processo de pid PRIORIDADE: %d\n", vPpid[i].pid);
                kill(vPpid[i].pid, SIGCONT);
                waitpid(vPpid[i].pid, 0, 0);
                printf("Processo PRIORIDADE de pid %d terminou\n", vPpid[i].pid);
                fprintf(fd3, "Processo PRIORIDADE de pid %d terminou\n", vPpid[i].pid);
            }
            *flag_escalonador = *flag_escalonador & 0xDF; //1101 1111
            *flag_escalonador = *flag_escalonador | 0x2;

            fclose(fd3);
            //printf("FIM DO ESCALONAMENTO PRIORIDADE\n");
            return 0;
            //FimEscalonadorPR
        }
        else{
            pid3 = fork();
            if(pid3 == 0)
            {
                //EscalonadorRR
                printf("pid3\n");
                int fd1, fd2, quantidadeParametros, j;
                FILE* fd3;
                int pid, qtd, processos_executando;
                char* commands[]={NULL},**linha, comando[200];
                int vpid[100], i;
                
                
                if ((fd3 = fopen("saidaEscalonadorRR.txt", "w")) == NULL) {
                    printf("Erro na abertura de saidaEscalonadorRR.txt\n");
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
                while( strcmp(mensagem, FIM) != 0 ) 
                {
                    if(words(mensagem) == 2)
                    {
                        printf("\n\n\n\n\tRR Leitura %d feita %s\n\n\n\n", i+1, mensagem);
                        fprintf(fd3, "\nRR Leitura %d feita %s\n", i+1, mensagem);

                        *flag_escalonador = *flag_escalonador | 0x40;
                        while( strcmp(mensagem, VAZIO) == 0) {
                            printf("Aguardando o interpretador preencher o comando\n");
                            fprintf(fd3, "Aguardando o interpretador preencher o comando\n");
                            sleep(1);
                        }
                        
                        //Após a leitura da linha, quebra-se a string
                        //A função breakString deve quebrar os espaços da string,
                        //deixando os parâmetros em ordem:
                        //Para RR: [0](nome)
                        //Para Prioridade: [0](nome) [1](prioridade)
                        //Para RT: [0](nome) [1](inicio) [2](duração)
                        linha = breakStringRR(mensagem, &quantidadeParametros);
                        strcpy(mensagem, VAZIO);
                        printf("nome: %s\n\n", linha[0]);
                        fprintf(fd3, "nome: %s\n\n", linha[0]);
                        if ( !(pid = fork()) ) 
                        {
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
                            //Processo pai
                            //Coloca o PID no vetor para o RR
                            vpid[i]=pid;
                            printf("vpid[%d]= %d\n", i, pid);
                            fprintf(fd3, "vpid[%d]= %d\n", i, pid);
                            i++;
                            //Intervalo de 1 segundo entre cada processo.
                            sleep(1);
                        }
                        for(j=0;j<quantidadeParametros;j++)
                            free(linha[j]);
                        free(linha);
                    }
                }
                //Esperando todos os processos lerem o interpretador
                sleep(4);
                //printf("\nIniciando escalonamento dos processos:\n\n");
                //fprintf(fd3, "\nIniciando escalonamento dos processos:\n\n");
                
                //Código do respectivo escalonador
                
                qtd = i;
                
                //printf("Iniciando escalonador RR\n");
                //fprintf(fd3, "Iniciando escalonador RR\n");
                for(i=0; i < qtd; i++)
                {
                    if (vpid[i] != -1) {
                    processos_executando++;
                    printf("Continuando processo ROUND-ROBIN de pid %d\n", vpid[i]);
                    fprintf(fd3, "Continuando processo ROUND-ROBIN de pid %d\n", vpid[i]);
                    kill(vpid[i], SIGCONT);
                    sleep(1);
                    if (waitpid(vpid[i], 0, 0 | WNOHANG) != 0) 
                    {
                        printf("Processo ROUND-ROBIN de pid %d terminou\n", vpid[i]);
                        fprintf(fd3, "Processo ROUND-ROBIN de pid %d terminou\n", vpid[i]);
                        vpid[i] = -1;
                    }
                    else 
                    {
                        printf("Pausando processo ROUND-ROBIN de pid %d\n", vpid[i]);
                        fprintf(fd3, "Pausando processo ROUND-ROBIN de pid %d\n", vpid[i]);
                        kill(vpid[i], SIGSTOP);
                    }
                    }
                    if (i == qtd-1 && processos_executando == 0)
                        break;
                    else if (i == qtd-1) {
                        i = -1;
                        processos_executando = 0;
                    }
                }

                *flag_escalonador = *flag_escalonador & 0xBF; //1011 1111
                *flag_escalonador = *flag_escalonador | 0x4;
                //printf("FIM DO ESCALONAMENTO ROBIN\n");
                //fprintf(fd3, "Fim do escalonamento.\n");
                
                fclose(fd3);
                return 0;
                //FimEscalonadorRR
            }
            else
            {
                //No processo pai que gerenciamos qual politica que escalamento que vai executar
                FILE* fd3;
                if ((fd3 = fopen("GerenciadorEscalonador.txt", "w")) == NULL) {
                    printf("Erro na abertura de GerenciadorEscalonador.txt\n");
                    exit(-2);
                }
                //while para garantir que todos os processos leiam as linhas do exec.txt
                while(strcmp(mensagem, FIM) != 0 ) 
                {
                    //printf("Lendo as linhas do interpretador\n");
                    //sleep(1);
                }
                printf("\n\n\n\n\tLido todas as linhas do interpretador\n\n\n");
                fprintf(fd3, "\n\n\n\n\tLido todas as linhas do interpretador\n\n\n");
                sleep(1);
                kill(pid1, SIGSTOP);
				kill(pid2, SIGSTOP);
				kill(pid3, SIGSTOP);
                //*flag_escalonador = *flag_escalonador | 0;

                int conta_segundos = 0;
                while(1)
                {
                    int ocupado;
                    long int segundo_atual = time(0)%60;
                    
                    if (*flag_escalonador == 0x7) //significa que todos os escalonamentos acabaram
                    {
                        kill(pid2, SIGKILL);
                        kill(pid3, SIGKILL);
                        kill(pid1, SIGKILL);
                        break;
                    } 
                    sleep(1);
                   
                    if(controle_tempo[segundo_atual] == TRUE)
                    {
                        ocupado = TRUE;
                        kill(pid2, SIGSTOP);
                        kill(pid3, SIGSTOP);
                        //printf("Ocupado %ld ", time(0)%60);
                    }
                    else
                    {
                        ocupado = FALSE;
                        //printf("Livre %ld ", time(0)%60);
                    }
                    if(ocupado == TRUE)
                    {
                        printf("RT -->%lds\n", segundo_atual);
                        fprintf(fd3, "RT-->%lds\n", segundo_atual);
                        kill(pid1, SIGCONT);
                    }
                    else
                        printf("%lds\n", segundo_atual);
                    if((0x1 & *flag_escalonador) == 0x0) //Se RT nao acabou entra no if
                    {
                        //if(ocupado == TRUE)
                            //printf("---Escalonamento_RT----\n");
                        //Matar o PR e RR
                        kill(pid2, SIGSTOP);
                        kill(pid3, SIGSTOP);
                        kill(pid1, SIGCONT);
                    }
                    else
                    {
                        kill(pid1, SIGSTOP);
                    }

                    if(ocupado == FALSE) 
                    {
                        
                        if( ( (0x2 & *flag_escalonador) == 0x0 ) ) //Processo PR ainda nao acabou
                        {
                            //printf("---Escalonamento_PR----\n");
                            fprintf(fd3, "PR-->%lds\n", segundo_atual);
                            kill(pid2, SIGCONT);
                            kill(pid3, SIGSTOP);
                        }
                        if((0x20 & *flag_escalonador) == 0x0)
                        {
                            if((0x4 & *flag_escalonador) == 0x0)
                            {
                                //printf("---Escalonamento_RR----\n");
                                fprintf(fd3, "RR-->%lds\n", segundo_atual);
                                //kill(pid1, SIGSTOP);
                                kill(pid2, SIGSTOP);
                                kill(pid3, SIGCONT);
                            }
                        }
                    }
                }
            }
        }
        
    }

  
  
  
}
















int words(const char *sentence)
{
    int count=0,i,len;
    char lastC;
    len=strlen(sentence);
    if(len > 0)
    {
        lastC = sentence[0];
    }
    for(i=0; i<=len; i++)
    {
        if((sentence[i]==' ' || sentence[i]=='\0') && lastC != ' ')
        {
            count++;
        }
        lastC = sentence[i];
    }
    return count;
}


char** breakStringRT(char* string, int* n)
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
  assert(tamanhoPalavras);
  
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
  assert(linha);
  for(i=0;i<(*n);i++) {
    linha[i] = (char*) malloc((tamanhoPalavras[i]+1)*sizeof(char));
    assert(linha[i]);
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


char** breakStringPR(char* string, int* n)
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
  assert(tamanhoPalavras);
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
  assert(linha);
  for(i=0;i<(*n);i++) {
    linha[i] = (char*) malloc((tamanhoPalavras[i]+1)*sizeof(char));
    assert(linha[i]);
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


char** breakStringRR(char* string, int* n)
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
  assert(tamanhoPalavras);
  
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
  assert(linha);
  for(i=0;i<(*n);i++) {
    linha[i] = (char*) malloc((tamanhoPalavras[i]+1)*sizeof(char));
    assert(linha[i]);
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
