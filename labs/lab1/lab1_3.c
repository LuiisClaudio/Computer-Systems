#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
	char *path = "alomundo";
	execv(path, NULL);
	return 0;
}
