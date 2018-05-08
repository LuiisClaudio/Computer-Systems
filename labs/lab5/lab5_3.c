#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main (void) {

	int fd[2],status;

	pipe(fd);

	if (fork() == 0) {
		char *argv[] = {"ls", NULL};
		dup2(fd[1],1);
		execv("/bin/ls",argv);

	}
	else {
		wait(&status);
		char *argv[] = {"grep", "lab5_3", NULL};
		dup2(fd[0],0);
		execv("/bin/grep",argv);
	}	

	return 0;
}
