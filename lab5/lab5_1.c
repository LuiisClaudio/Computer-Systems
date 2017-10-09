#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
	int pid1, pid2, i;
	
	pid1 = fork();
	if(pid1 == 0)
	{
		printf("Esse processo é 1 %d--%ḍ\n", getpid(), getppid());
		while(1)
		{
			printf("\n\tLOOP do Processo 1\n");
		}
	}
	else
	{
		pid2 = fork();
		if(pid2 == 0)
		{
			printf("Esse processo é 2 %d--%ḍ\n", getpid(), getppid());
			while(1)
			{
				printf("\n\tloop DO PROCESSO 2\n");
			}	
		}
		else
		{
			printf("Processo pai %d\n", getpid());	
			//Aqui o pai controla a troca em processo 1 e 2
			kill(pid1, SIGSTOP);
			kill(pid2, SIGSTOP);
			printf("\n\npid1->%d, pid2->%d\n", pid1, pid2);
			for(i = 0; i < 10; i++)
			{
				kill(pid1, SIGCONT);
				sleep(1);
				kill(pid1, SIGSTOP);
				kill(pid2, SIGCONT);
				sleep(1);
				kill(pid2, SIGSTOP);
			}
			kill(pid1, SIGKILL);
			kill(pid2, SIGKILL); 			
		}
	}
}