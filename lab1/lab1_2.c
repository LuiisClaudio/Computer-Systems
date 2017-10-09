#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
	int x = 0;
	int pai = getpid();
	if (fork() != 0) { // Parent Code
		for(int i = 0; i < 100; i++) 
			x += 5;
		printf("Pai %d - %d\n", pai, x);
	}
	else { // Child Code
		for(int i = 0; i < 100; i++) 
			x += 10;
		
		printf("Filho %d - %d\n", getpid(), x);
		exit(0);
	}
	
	return 0;
}
