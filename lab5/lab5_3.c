#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

void inicioHandler(int signal);
void terminoHandler(int signal);

time_t inicio = 0, fim = 0;

int main(int argc, char *argv[]) {

	pid_t pid;
	
	signal(SIGUSR1, inicioHandler);
	signal(SIGUSR2, terminoHandler);
	
	pid = fork();
	if(pid == 0)
	{
		while(1)
		{
			printf("Observando a ligacão");
		}
	}
	else 
	{
		kill(pid, SIGUSR1);
		int tempo;
		sscanf(argv[1], "%d", &tempo);
		sleep(tempo);
		kill(pid, SIGUSR2);
		int status;
		wait(&status);
	}
	return 0;
			
}

void inicioHandler(int signal)
{
	inicio = time(NULL);
}
void terminoHandler(int signal)
{
	fim = time(NULL);
	printf("\nTempo final: %d\n", (fim - inicio));
	
	if((fim - inicio) <= 90)
	{
		printf("\nTotal a pagar %d\n", (fim - inicio)*2);			
	}
	else
	{
		printf("\nTotal a pagar eh fixo em R$ 1,50\n");
	}
	exit(1);
}
