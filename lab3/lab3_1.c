#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

int main () {
    int i, segmento1, segmento2, segmento3, id, status;
    int * matriz1, * matriz2, * mat3, * bas1, * bas2, * bas3;
    
    // Aloca a memória compartilhada
    segmento1 = shmget(IPC_PRIVATE, 9 * sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR |  S_IWUSR);
    segmento2 = shmget(IPC_PRIVATE, 9 * sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR |  S_IWUSR);
    segmento3 = shmget(IPC_PRIVATE, 9 * sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR |  S_IWUSR);
    
    // associa a memória compartilhada ao processo
    matriz1 = (int *) shmat(segmento1, 0, 0);
    matriz2 = (int *) shmat(segmento2, 0, 0);
    mat3 = (int *) shmat(segmento3, 0, 0);
    
    bas1 = matriz1;
    bas2 = matriz2;
    bas3 = mat3;
    
    for (i = 0; i < 9; i++) {
        *matriz1 = i+1;
        matriz1 += 4;
        
        *matriz2 = i+1;
        matriz2 += 4;
    }
    
    
    matriz1 = bas1;
    matriz2 = bas2;
    
    if ((id = fork()) < 0) { // Processo pai
        puts ("Erro na criação do novo processo");
        exit (-2);
    }
    else if (id == 0) { // Processo filho 1
        for (i = 0; i < 8; i++) {
            *mat3 = *matriz1 + *matriz2;
            matriz2 += 4; //Proximo elemento da "matriz"
            matriz1 += 4; //Proximo elemento da "matriz"
            mat3 += 4;
        }
        id = fork();
        
        if (id == 0) { // Processo filho 2
            for (i = 0; i < 8; i++) {
                *mat3 = *matriz1 + *matriz2;
                matriz2 += 4;
                matriz1 += 4;
                mat3 += 4;
            }
            id = fork();
            
            if (id == 0) { // Processo filho 3
                *mat3 = *matriz1 + *matriz2;
                matriz2 += 4;
                matriz1 += 4;
                mat3 += 4;
                *mat3 = *matriz1 + *matriz2;
                matriz2 += 4;
                matriz1 += 4;
                mat3 += 4;
                *mat3 = *matriz1 + *matriz2;
            }
        }
    }
    else { // Processo pai
        waitpid(-1, &status, 0);
        
        mat3 = bas3;
        printf ("Matriz somada\n");
        
        printf ("%d ", *mat3);
        mat3 += 4;
        printf ("%d ", *mat3);
        mat3 += 4;
        printf ("%d\n", *mat3);
        mat3 += 4;
        printf ("%d ", *mat3);
        mat3 += 4;
        printf ("%d ", *mat3);
        mat3 += 4;
        printf ("%d\n", *mat3);
        mat3 += 4;
        printf ("%d ", *mat3);
        mat3 += 4;
        printf ("%d ", *mat3);
        mat3 += 4;
        printf ("%d\n", *mat3);
    }
    
    // Libera a memória compartilhada das matrizes
    shmdt (matriz1);
    shmdt (matriz2);
    shmdt (mat3);
    
    // Libera a memória compartilhada
    shmctl (segmento1, IPC_RMID, 0);
    shmctl (segmento2, IPC_RMID, 0);
    shmctl (segmento3, IPC_RMID, 0);
    
    return 0;
}