#include <stdio.h>
#include <sys/shm.h>
#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>


#define MAXITENS 64
#define MAXQUEUE 8


typedef struct circularQueue {
	int queue[MAXQUEUE];
	int ini;
	int end;
	int size;
} circularQueue;

void inicialize(circularQueue * q);
void insert(circularQueue * q, int value);
int removee(circularQueue * q);
int isFull(circularQueue * q);
int isEmpty(circularQueue * q);

void make(circularQueue * queue, int pidProd, int pidCons);
void consume(circularQueue * queue, int pidCons, int pidProd);

int main (void) {
	int seg1, seg2, status;
	int pidProd, pidCons, *sharedPidCons;
	circularQueue * queue;
	
	// Aloca a memória compartilhada
	seg1 = shmget (IPC_PRIVATE, sizeof(circularQueue), IPC_CREAT | IPC_EXCL | S_IRUSR |  S_IWUSR);
	seg2 = shmget (IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR |  S_IWUSR);
	
	// Associa a memória compartilhada ao processo
	queue = (circularQueue *) shmat (seg1, 0, 0);
	sharedPidCons = (int *) shmat (seg2, 0, 0);
	
	inicialize(queue);
	*sharedPidCons = 0;
	
	pidProd = fork();
	
	if (pidProd != 0) { // Processo pai
		pidCons = fork();
		
		if (pidCons != 0) { // Processo pai
			*sharedPidCons = pidCons;
			shmdt(sharedPidCons);
			
			waitpid (-1, &status, 0);
			waitpid (-1, &status, 0);
			
			printf("Programa chegou no seu final\n");
			
			shmdt(queue);
			shmctl(seg1, IPC_RMID, 0);
		}
		else { // Processo consumidor
			printf("Cosumidor foi inicializado\n");
			
			shmdt(sharedPidCons);
			pidCons = getpid();
			
			consume(queue, pidCons, pidProd);
			
			shmdt(queue);
			exit(1);
		}
	}
	else { // Processo produtor
		while (*sharedPidCons == 0);
		printf("Produtor foi inicializado\n");
		
		pidCons = *sharedPidCons;
		shmctl(seg2, IPC_RMID, 0);
		pidProd = getpid();
		
		make(queue, pidProd, pidCons);
		
		shmdt(queue);
		exit(1);
	}
	
	return 0;
}

void make(circularQueue * queue, int pidProd, int pidCons)
 {
	int i, item;
	
	for (i = 0; i < MAXITENS; i++) {
		// Gero item diferente de 0
		item = rand()%100 + 1;
		
		// Recebo sinal de lotada
		if (isFull(queue))
			kill(pidProd, SIGSTOP);
		
		// Insiro item
		insert(queue, item);
		printf("Produtor %d produziu um item, valor do item %d\n", i + 1, item);
		
		// Mando sinal de nao vazia
		if (queue->size == 1)
			kill(pidCons, SIGCONT);
		
		sleep(1);
	}
}

void consume(circularQueue * queue, int pidCons, int pidProd)
{
	int i, item;
	
	for (i = 0; i < MAXITENS; i++) {
		// Recebo sinal de vazia
		if (isEmpty(queue))
			kill(pidCons, SIGSTOP);
		
		// Consumo item
		item = removee(queue);
		
		// Mando sinal de nao lotada
		if (queue->size == MAXQUEUE - 1)
			kill(pidProd, SIGCONT);
		
		printf("Cosumidor %d consumiu, valor %d\n", i + 1, item);
		sleep(2);
	}
}

/// Fila Circular - implementacao

void inicialize(circularQueue * q) {
	int i;
	
	for (i = 0; i < MAXQUEUE; i++) {
		q->queue[i] = 0;
	}
	
	q->ini = 0;
	q->end = 0;
	q->size = 0;
}

void insert(circularQueue * q, int value)
 {
	q->queue[q->end] = value;
	q->end = (q->end + 1) % MAXQUEUE;
	(q->size)++;
}

int removee(circularQueue * q)
{
	int value;
	value = q->queue[q->ini];
	
	q->queue[q->ini] = 0;
	q->ini = (q->ini + 1) % MAXQUEUE;
	(q->size)--;
	
	return value;
}

int isFull(circularQueue * q)
{
	return q->size == MAXQUEUE;
}

int isEmpty(circularQueue * q)
{
	return q->size == 0;
}
