#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
	int pid = fork(), status;	
	int n = 0;
	if (pid != 0) { /* fork off child process
	*/
	/* Parent code */
		n = n + 50;
		printf("Pai <%d> - <%d>\n", getpid(), n);
		waitpid( -1, &status, 0); /* wait for child to exit */
		printf("Pai final <%d> - <%d>\n", getpid(), n);

	} 
	else 
	{
	/* Child code */
		//execve (command, parameters, 0); /* execute command */
		n = n + 10;
		printf("Filho <%d> - <%d>\n", getpid(), n);
		
		exit(0);
	}
}
