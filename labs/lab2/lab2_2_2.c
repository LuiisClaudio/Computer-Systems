#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char *argv[])
{
	char segmento[10];
	char *p;
	
	segmento = shmget (8752, sizeof(char)*10, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);	
	
	p = shmat (segmento, 0, 0);
	
	printf("%s", p);
	
	
	return 0;
}