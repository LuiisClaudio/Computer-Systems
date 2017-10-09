#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
	int pidFilho[3];
	int pidPai = getpid();
	int matriz[3][3] = {{1,2,3},{4,5,6},{7,8,9}};
	int matriz2[3][3] = {{1,2,3},{4,5,6},{7,8,9}};
	int size_matriz = 3, qtd_pid = 3;
	int soma = 0;
	
	printf("[dad] pid %d\n", pidPai);
	for( int i = 0; i < qtd_pid; i++ )
	{
		if (fork() == 0)
		{
			pidFilho[i] = getpid();
			//printf( "[son] pid %d from pid %d\n", pidFilho[i], getppid());
			
			for(int j = 0; j < size_matriz; j++)
			{
				matriz[i][j] = matriz[i][j] + matriz2[i][j];
				printf("%d ", matriz[i][j]); 
				//exit(0);
			}
			printf("\n");
			exit(0);
		}
	}

	for ( int i = 0; i < 3; i++ )
		wait( NULL );
}