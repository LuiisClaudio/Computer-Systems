#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
	int fd[2],status;
	const char textoTX[] = "Its over 9000!!!";
	char textoRX[sizeof(textoTX)]; 
	if(pipe(fd) < 0) {
		puts("Error open()");
		exit(-1);	
	}
	if(fork() == 0) {
		close(fd[0]);
		write(fd[1],textoTX, strlen(textoTX) + 1);
	} else {
		wait(&status);	
		close(fd[1]);
		read(fd[0],textoRX, sizeof(textoRX));
		printf("%s\n",textoRX);
	}
	return 0;
}