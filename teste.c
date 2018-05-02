#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, const char * argv[]) 
{
	int status, pid;
	
	if (pid = fork() ==0) 
	{
		if (execv("aux.c", NULL) == -1)
		{
			 exit(-1);
		}
		else 
		{
			perror("exec");
		}
	}
	else 
	{
		wait(&status);
		printf("O filho terminou com estado: %d\n",status);
	}

}