#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
int main (int argc, char *argv[])
{
	int segmento, *p, id, pid, status;
	int matriz1;//= {1,2,3,4};
	int matriz2;// = {11,22,33,44};
	int matriz3;
	int n = 2;
	int *m1, *m2, *m3;
	int nxn = 4;
	int i = 0;
	
	matriz1 = shmget (IPC_PRIVATE, sizeof(int)*nxn, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	matriz2 = shmget (IPC_PRIVATE, sizeof(int)*nxn, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	matriz3 = shmget (IPC_PRIVATE, sizeof(int)*nxn, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	
	m1 = (int *) shmat (matriz1, 0, 0);
	m2 = (int *) shmat (matriz2, 0, 0);
	m3 = (int *) shmat (matriz3, 0, 0);

	for(i = 0; i < 4; i++)
	{
		//*m1 = i;
		*m2 = i*11;;
		//m1 = m1 + sizeof(int);
		m2 = m2 + sizeof(int);
	}
	m1 = m1 - i*sizeof(int);
	m2 = m2 - i*sizeof(int);

	for(i = 0; i < 4; i++)
	{
		//printf("%d, %d ", m1[i], m2[i]);
	}

	id = fork();
	if (id < 0)
	{
		puts ("Erro na criação do novo processo");
		exit (-2);
	}
	else if (id == 0)
	{
		printf("Filho1\n");
		for(i = 0; i < 2; i++)
		{	
			*m3 = m1[i] + m2[i];
			printf("%d ", *m3);
			m3 = m3 + sizeof(int);
		}
		printf("\n");
	}
	else
	{
		
		if(fork() == 0)
		{
			printf("Filho1\n");
			for(i = 0; i < 2; i++)
			{	
				*m3 = m1[i] + m2[i];
				printf("%d ", *m3);
				m3 = m3 + sizeof(int);
			}
			//("\n");
		}
		else
		{
			printf("Pai\n");
			waitpid(-1, &status, 0);
		}	
	}
	
	shmdt (p);
	// libera a memória compartilhada
	shmctl (segmento, IPC_RMID, 0);
	return 0;
}
