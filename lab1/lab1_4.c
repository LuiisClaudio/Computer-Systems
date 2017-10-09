#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
	int status;

	if (fork() != 0) { // Parent Code
		waitpid(-1, &status, 0);
	}
	else { // Child Code
		execv("alomundo", NULL);
		exit(1);
	}

    return 0;
}

