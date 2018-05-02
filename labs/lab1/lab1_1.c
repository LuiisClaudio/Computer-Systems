#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
	int pid = fork(), status;	
	
	if (pid != 0) { /* fork off child process
	*/
	/* Parent code */
		printf("Meu pid: %d\n", getpid());
		waitpid( -1, &status, 0); /* wait for child to exit */
		printf("Pai finalizado\n");
	} 
	else 
	{
	/* Child code */
		//execve (command, parameters, 0); /* execute command */
		printf("Meu pid filho %d\n", getpid());
		printf("Pid pai %d\n", getppid());
		printf("Filho finalizado\n");
		exit(0);
	}
}
