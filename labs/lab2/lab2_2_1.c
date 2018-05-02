#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char *argv[])
{
	char segmento;
	char *p, *t;
	FILE *f;
	char palavra[20];
	
	segmento = shmget (8752, sizeof(char)*20, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);	
	//p = (char*) malloc(sizeof(char)*20);
	p = shmat (segmento, 0, 0);
	t = p;
	f = fopen("mensagemdodia.txt", "r");
	
	fscanf(f,"%s", palavra);
	
	printf("%s\n\n", palavra);

	//strcpy(p, palavra);
	int i = 0;
	while(palavra[i] != '\0')
	{
		*p = palavra[i];
		p = p + sizeof(char);
		i++;
	}
	printf("%s\n", t);
	fclose(f);
	return 0;
}
