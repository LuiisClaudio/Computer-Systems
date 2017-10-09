#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, const char * argv[]) {
	int status, pidPai = getpid(), pidFilho = fork();
	
	if (pidFilho != 0) { // Parent Code
		
		printf("Processo pai iniciado\n");
		
		waitpid(-1, &status, 0);
		printf("Processo pai finalizado\n");
	}
	else { // Child Code
		printf("Processo filho iniciado\n");
		
		
		pidFilho = getpid();
		
		printf("Processo filho finalizado\n");
		exit(0);
	}
	
    return 0;
}
