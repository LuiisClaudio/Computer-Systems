#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define TRUE 1
#define EVER ;;

int main() {
	pid_t pid1, pid2, pid3;

	if((pid1 = fork()) < 0) { // Processo pai
		fprintf(stderr, "Erro ao criar filho\n");
		exit(-1);
	}
	else if(pid1 == 0) { // Processo filho 1
		printf("Processo filho 1 inicializado.\n");

		while(TRUE)
			printf("PF1 - ");

    printf("\n");
	}
	else 
	{
		if((pid2 = fork()) < 0) 
		{ // Processo pai
			fprintf(stderr, "Erro ao criar filho\n");
			exit(-1);
		}

		if(pid2 == 0) 
		{ // Processo filho 2
			printf("Processo filho 2 inicializado.\n");

			while(TRUE)
				printf("PF2 - ");

      		printf("\n");
		}
		else 
		{
			if((pid3 = fork()) < 0) { // Processo pai
				fprintf(stderr, "Erro ao criar filho\n");
				exit(-1);
			}
			if(pid3 == 0) 
			{ // Processo filho 2
				printf("Processo filho 2 inicializado.\n");

				while(TRUE)
					printf("PF3 - ");
				printf("\n");
			}
			else
			{
				kill(pid1, SIGSTOP);
				kill(pid2, SIGSTOP);
				kill(pid3, SIGSTOP);

				for(EVER)
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

				kill(pid1, SIGKILL);
				kill(pid2, SIGKILL);
				kill(pid3, SIGKILL);

				exit(1);
			}
		}
	}

	return 0;
}
