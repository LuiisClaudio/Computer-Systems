#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, const char * argv[]) {
	int i, status, pidPai = getpid(), pidFilho = fork();
	
	
	if (pidFilho != 0) { // Parent Code
		//printf("\nProcesso pai vai %d\n", pidPai);
		for (i = 0; i < 100; i++) {
			printf("\npid %d - numero: %d\n", pidPai, i);
			sleep(1);
		}
		printf("\nProcesso pai vai finalizar\n");
		
		waitpid(-1, &status, 0);
		printf("\nProcesso pai finalizado\n");
	}
	else if (fork() != 0) { // Child Code
		printf("\t\nFilho foi criado\n");
		
		for (i = 100; i < 200; i++) {
			printf("\t\npidFilho %d, pidPai %d - numero: %d\n", getpid(), getppid(), i);
			sleep(2);
		}
		printf("\t\nProcesso filho vai finalizar\n");
		
		waitpid(-1, &status, 0);
		printf("\t\nProcesso filho finalizado\n");
	}
	else { // Grandchild Code
		printf("\t\t\nNeto foi criado - %d\n", getpid());
		
		for (i = 200; i < 300; i++) {
			printf("\t\t\npidNeto %d, pidFilho %d - numero: %d\n", getpid(), getppid(), i);
			sleep(3);
		}
		printf("\t\t\nProcesso neto vai finalizar\n");
		printf("\t\t\nProcesso neto finalizado\n");
		exit(1);
	}

    return 0;
}