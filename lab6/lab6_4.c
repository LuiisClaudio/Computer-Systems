#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/sem.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>


int setSemValue(int semId);
void delSemValue(int semId);
int semaforoP(int semId);
int semaforoV(int semId);

int semId;
char *mensagemLida;

union semun1 {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int main(int args, char *argv[]) {
    int segmento,semId,i=0;
    char resultado[30],mensagemEscrita[30];
    segmento = shmget(123456, sizeof(char) * 30, IPC_CREAT | S_IRUSR | S_IWUSR);
    mensagemLida = (char *) shmat(segmento, 0, 0);
    semId = semget(1234, 1, IPC_CREAT);
    mensagemLida[0] = 0;
    mensagemLida[1] = 2;
    
    while(1) {
        semaforoP(semId);
        i = 0;
        
        while(mensagemLida[i] != 0) {
            resultado[i] = mensagemLida[i];
            i++;
        }
        resultado[i] = 0;
                if(mensagemLida[i + 1] == 2) {
                    if(strlen(resultado) > 0) {
                        printf("mensagem recebida: %s\n",resultado);
                    }
                    printf("entre com a mensagem: ");
                    scanf("%s",mensagemEscrita);
            i = 0;
            while(mensagemEscrita[i] != 0) {
                mensagemLida[i] = mensagemEscrita[i];
                i++;
            }
            mensagemLida[i] = 0;
            mensagemLida[i + 1] = 1;
            }
        semaforoV(semId);

        sleep(1);
    }


    return 0;
}


int setSemValue(int semId) {
    union semun1 semUnion;
    semUnion.val = 1;
    return semctl(semId, 0, SETVAL, semUnion);
}

void delSemValue(int semId) {
    union semun1 semUnion;
    semctl(semId, 0, IPC_RMID, semUnion);
}

int semaforoP(int semId) {
    struct sembuf semB;
    semB.sem_num = 0;
    semB.sem_op = -1;
    semB.sem_flg = SEM_UNDO;
    semop(semId, &semB, 1);
    return 0;
}

int semaforoV(int semId) {
    struct sembuf semB;
    semB.sem_num = 0;
    semB.sem_op = 1;
    semB.sem_flg = SEM_UNDO;
    semop(semId, &semB, 1);
    return 0;
}

void sigQUITHandler(int signal) {
    delSemValue(semId);
    shmdt(mensagemLida);
    exit(-1);
}
