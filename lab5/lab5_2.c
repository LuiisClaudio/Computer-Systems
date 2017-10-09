#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

void errorHandler(int signal);

int main() {
	int n1, n2;
	
	signal(SIGFPE, errorHandler);
	
	printf("\nDiga seu dois numeros para operacoes basicas\n");
	scanf("%f %f", &n1, &n2);
	printf("Operacao de soma = %d + %d = %d\n", n1, n2, n1+n2);
	printf("Operacao de soma = %d - %d = %d\n", n1, n2, n1-n2);
	printf("Operacao de soma = %d * %d = %d\n", n1, n2, n1*n2);
	printf("Operacao de soma = %d / %d = %d\n", n1, n2, n1/n2);
}

void errorHandler(int signal)
{
	printf("Sinal de divisão por 0\n");
	exit(-1);	
}
