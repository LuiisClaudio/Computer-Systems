#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>

union semaun{

	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

// inicializa o valor do semáforo
int setSemValue(int semId);

// remove o semáforo
void delSemValue(int semId);

// operação P
int semaforoP(int semId);

//operação V
int semaforoV(int semId);

int main (){

	int segmento, *num, semID, i;

	segmento = shmget(1234, sizeof(int), IPC_CREAT | S_IRUSR | S_IWUSR);
	semID = semget(1234, 1, IPC_CREAT);

	num = (int *) shmat(segmento, 0, 0);

	for(i=0;i<10;i++){

		semaforoP(semID);
		*num += 1;
		printf("somando 1: %d\n", *num);
		sleep(2);
		semaforoV(semID);
		sleep(1);
	}

	shmdt(num);
	shmctl(segmento, IPC_RMID, 0);

	return 0;
}

int setSemValue(int semId){

	union semaun semUnion;
	semUnion.val = 1;
	return semctl(semId, 0, SETVAL, semUnion);
}

void delSemValue(int semId){

	union semaun semUnion;
	semctl(semId, 0, IPC_RMID, semUnion);
}

int semaforoP(int semId){

	struct sembuf semB;
	semB.sem_num = 0;
	semB.sem_op = -1;
	semB.sem_flg = SEM_UNDO;
	semop(semId, &semB, 1);
	return 0;
}

int semaforoV(int semId){

	struct sembuf semB;
	semB.sem_num = 0;
	semB.sem_op = 1;
	semB.sem_flg = SEM_UNDO;
	semop(semId, &semB, 1);
	return 0;
}