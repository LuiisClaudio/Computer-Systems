#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
	int pidFilho[4];
	int pidPai = getpid();
	int vet[10] = {0,1,23,34,4,75,69,79,18,9,10};
	int size_vet = 10, qtd_pid = 4;
	int procurado = 9;
	
	printf( "[dad] pid %d\n", pidPai);

	for ( int i = 0; i < 4; i++ )
	{
		if (fork() == 0)
		{
			pidFilho[i] = getpid();
			printf( "[son] pid %d from pid %d\n", pidFilho[i], getppid());
			
			for(int ini = i; ini < size_vet; ini += 4)
			{
				if(procurado == vet[ini])
				{
					printf("\nFilho %d achou o elemento procurado %d\n", i, vet[ini]);
					exit(0);
				}
			}
			exit(0);
		}
	}
	

	for ( int i = 0; i < 3; i++ )
		wait( NULL );
	
}
