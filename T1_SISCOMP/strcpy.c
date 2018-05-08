#define VAZIO ""
#define FIM "fim"
#define SIZE_SEG 200

int main (int argc, char *argv[])
{
	//char comando[200];

	int segmentomsg;
  	char *comando;
  
  	printf("Criando areas de memoria\n");

  	//Criando a área de memória compartilhada
  	segmentomsg = shmget(8888, SIZE_SEG*sizeof(char), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
  
  
  	//Associando as área às variáveis pid e i
  	comando = (char*) shmat(segmentomsg, 0, 0);

	//strcpy(comando, VAZIO);
  	i = 0;
	while(comando[i] != '\0')
	{
	 	comando[i] = 0;
	 	i++;
	}
	printf("%s", comando);
}