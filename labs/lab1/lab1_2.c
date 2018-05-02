#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
	int pid = fork(), pid_filho, pid_neto, ostatus;	
	
	if (pid != 0) { 
		for(int i = 0; i < 50; i++)
		{
			printf("Pai %d %d\n", getpid(),i);
			sleep(2);
		}
		printf("Processo pai vai morrer\n");
	} 
	else
	{
		pid_filho = fork();
		if(pid_filho != 0)
		{
			for(int i = 100; i < 200; i++)
			{	
				printf("Pai:%d Filho:%d  %d\n", getppid(), getpid(),i);
				sleep(1);
			}
			printf("Filho vai morrer\n");
		}
		else
		{
			for(int i = 300; i < 350; i++)
			{	
				printf("Filho:%d Neto:%d  %d\n", getppid(), getpid(),i);
				sleep(2);
			}
			exit(0);
		}
		
	}
}
