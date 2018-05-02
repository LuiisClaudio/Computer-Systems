#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int pid = fork();
	int n=0,i=4;
	int vetor={12,546,516,54,156,54,0,498};
	int *v;
	int elem = 54;
	vetor = shmget (IPC_PRIVATE, sizeof(int)*8, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);

	v = (int *) shmat (vetor, 0, 0);

	if(pid!=0)
	{
		for(i = 4;i<8;i++)
		{
			if(v[i]==elem){
				printf("Processo 2 achou");
				exit;
			}
		}
	}
	else
	{
		for(n = 0;n<i;n++)
		{
			if(v[n]==elem){
				printf("Processo 1 achou");
				exit;
			}
		
		}
	}

 return 0;
}
