#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
	int pid1, pid2, pid3;
	
	pid1 = fork();
	if(pid1 == 0)
	{
		execv("aux1.c", NULL);
	}
	else
	{
		pid2 = fork();
		if(pid2 == 0)
		{
			execv("aux2.c", NULL);
			pid3 = fork();
			if(pid3 == 0)
			{
				execv("aux3.c", NULL);
			}
			else
			{
				kill(pid1, SIGSTOP);
				kill(pid2, SIGSTOP);
				kill(pid3, SIGSTOP);
			
				while(1)
				{
					kill(pid1, SIGCONT);
					sleep(1);
					kill(pid1, SIGSTOP);
					kill(pid2, SIGCONT);
					sleep(2);
					kill(pid2, SIGSTOP);
					kill(pid3, SIGCONT);
					sleep(2);
					kill(pid3, SIGSTOP);
				}
				exit(1);
			}
		}
	}
}