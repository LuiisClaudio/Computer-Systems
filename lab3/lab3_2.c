#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

int main () {
    int segmento;
    char * mensagem;

    // Aloca a memória compartilhada
    segmento = shmget (8180, 500 * sizeof(char), IPC_EXCL | S_IRUSR |  S_IWUSR);
    mensagem = (char *) shmat(segmento, 0, 0);

    printf("%s\n", mensagem);

    shmdt(mensagem);
    shmctl(segmento, IPC_RMID, 0);
    
    return 0;
}
