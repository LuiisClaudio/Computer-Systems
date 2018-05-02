#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include <sys/shm.h>
#include <sys/stat.h>

#define EVER ;;

void escreve(int sinal);
void le(int sinal);

int msg, i=1;
int *mensagem;
	

int main (int argc, char *argv[])
{
	msg = shmget (IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR |  S_IWUSR);
	mensagem = (int *)shmat(msg, 0, 0);
  	*mensagem = 5;

	for(EVER){
		escreve(SIGUSR1);
		sleep(1);
	}
	
}

void escreve(int sinal)
{
	if(sinal == SIGUSR1){
		*mensagem = *mensagem + 1;
		printf("msg escrita\n");
		le(SIGUSR1);
	}
}

void le(int sinal)
{
	printf("msg lida\n");
	printf("%d\n", *mensagem);
	escreve(SIGUSR2);

}