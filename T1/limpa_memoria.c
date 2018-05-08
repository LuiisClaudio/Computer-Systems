#include <sys/wait.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <math.h>


int main(void)
{
    int segmentomsg;
    char *comando;
    segmentomsg = shmget(1234, 200*sizeof(char), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    comando = (char*) shmat(segmentomsg, 0, 0);

    shmdt (comando);
    /* Deallocate the shared memory segment.  */ 
    shmctl (segmentomsg, IPC_RMID, 0); 
}