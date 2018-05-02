#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include <sys/shm.h>
#include <sys/stat.h>

#define TRUE 1
#define constante 5



int main() {
  	int i, segmento2;
	pid_t pid1, pid2, pid_aux;
	int *valor;

	segmento2 = shmget (IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR |  S_IWUSR);
	valor = (int *) shmat (segmento2, 0, 0);
	*valor = constante;

	if((pid1 = fork()) < 0) { // Processo pai
		fprintf(stderr, "Erro ao criar filho\n");
		exit(-1);
	}
	else if(pid1 == 0) { // Processo filho 1
		//printf("Processo filho 1 inicializado.\n");

		while(TRUE)
		{
			//printf("PF1 - ");
		}

    printf("\n");
	}
	else {
		if((pid2 = fork()) < 0) { // Processo pai
			fprintf(stderr, "Erro ao criar filho\n");
			exit(-1);
		}

		if(pid2 == 0) { // Processo filho 2
			//printf("Processo filho 2 inicializado.\n");

			while(TRUE)
			{
				//printf("PF2 - ");
			}

      printf("\n");
		}
		else {
			kill(pid1, SIGSTOP);
			kill(pid2, SIGSTOP);

			for(i = 0; i < 10; i++) {
				if(i != 0) {
          //printf("\n\n--------- Processo pai alterna os filhos ---------\n\n");
          printf("\n%d  ", *valor);
          *valor = *valor + constante;
          kill(pid1, SIGSTOP);
        }

				sleep(1);
				kill(pid2, SIGCONT);
				sleep(1);

				pid_aux = pid1;
				pid1 = pid2;
				pid2 = pid_aux;
			}

			kill(pid1, SIGKILL);
			kill(pid2, SIGKILL);

			exit(1);
			}
		}

	return 0;
}