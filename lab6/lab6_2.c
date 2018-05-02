#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>


void sigQUITHandler(int signal);
int setSemValue(int semId);
void delSemValue(int semId);
int semaforoP(int semId);
int semaforoV(int semId);

int semId;

union semun1 {
  int val;
  struct semid_ds *buf;
  unsigned short *array;
};


int main() {
  int pid,i=0;
  int fd = open("buffer.txt",O_RDWR);
  char buffer[] = "abcdefghijklmnop";
  char bufferRead[17];
  signal(SIGQUIT, sigQUITHandler);
  semId = semget(8752, 1, 0666 | IPC_CREAT);
  setSemValue(semId);
  pid = fork();
  while(1) {
    if(pid == 0) {
      semaforoP(semId);
            lseek(fd,0,SEEK_SET);
            char vazio = 0;
                for(i=0;i<16;i++) {
                    write(fd,&buffer[i],1);
                }
                write(fd,&vazio,1);
      semaforoV(semId);
    } else {

      semaforoP(semId);
            lseek(fd,0,SEEK_SET);
      if(read(fd,bufferRead,1) > 0) {
        read(fd,&(bufferRead[1]),16);
        printf("foi lido: %s\n",bufferRead);
      }
      semaforoV(semId);
            sleep(1);
    }
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
  exit(-1);
}