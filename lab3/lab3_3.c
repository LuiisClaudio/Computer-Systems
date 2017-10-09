#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

void imprimirMatriz(int * mat);

int main () {
    int i, j, segmento1, segmento2, id, status, aux;
    int * matriz1, * matriz2, * bas1, * bas2;
    
    // Aloca a memória compartilhada
    segmento1 = shmget(IPC_PRIVATE, 16 * sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR |  S_IWUSR);
    segmento2 = shmget(IPC_PRIVATE, 16 * sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR |  S_IWUSR);
    
    // associa a memória compartilhada ao processo
    matriz1 = (int *) shmat(segmento1, 0, 0);
    matriz2 = (int *) shmat(segmento2, 0, 0);
    
    bas1 = matriz1;
    bas2 = matriz2;
    
    printf("Entre com os numeros da matriz:\n");
    for (i = 0; i < 16; i++) {
        if (i%4 == 0)
            printf("Linha %d: ", i/4 + 1);
        
        scanf(" %d", &aux);
        *matriz1 = aux;
        matriz1 += 4;
    }
    matriz1 = bas1;
    
    if ((id = fork()) < 0) { // Processo pai
        puts ("Erro na criação do novo processo");
        exit (-2);
    }
    else if (id == 0) { // Processo filho 1
        for (i = 0; i < 4; i++) {
            *matriz2 = *matriz1;
            matriz2 += 4;
            matriz1 += 16;
        }
        
        id = fork();
        
        if (id == 0) { // Processo filho 2
            matriz1 = bas1 + 4;
            for (i = 0; i < 4; i++) {
                *matriz2 = *matriz1;
                matriz2 += 4;
                matriz1 += 16;
            }
            
            id = fork();
            
            if (id == 0) { // Processo filho 3
                matriz1 = bas1 + 8;
                for (i = 0; i < 4; i++) {
                    *matriz2 = *matriz1;
                    matriz2 += 4;
                    matriz1 += 16;
                }
                
                id = fork();
                
                if (id == 0) { // Processo filho 4
                    matriz1 = bas1 + 12;
                    for (i = 0; i < 4; i++) {
                        *matriz2 = *matriz1;
                        matriz2 += 4;
                        matriz1 += 16;
                    }
                }
            }
        }
    }
    else { // Processo pai
        sleep(1);
        waitpid(-1, &status, 0);
        
        matriz1 = bas1;
        printf ("\nMatriz:\n");
        imprimirMatriz(matriz1);
        
        matriz2 = bas2;
        printf ("Transposta:\n");
        imprimirMatriz(matriz2);
    }
    
    // Libera a memória compartilhada das matrizes
    shmdt (matriz1);
    shmdt (matriz2);
    
    // Libera a memória compartilhada
    shmctl (segmento1, IPC_RMID, 0);
    shmctl (segmento2, IPC_RMID, 0);
    
    return 0;
}

void imprimirMatriz(int *matriz) 
{
    printf ("%d ", *matriz);
    matriz += 4;
    printf ("%d ", *matriz);
    matriz += 4;
    printf ("%d ", *matriz);
    matriz += 4;
    printf ("%d\n", *matriz);
    matriz += 4;
    printf ("%d ", *matriz);
    matriz += 4;
    printf ("%d ", *matriz);
    matriz += 4;
    printf ("%d ", *matriz);
    matriz += 4;
    printf ("%d\n", *matriz);
    matriz += 4;
    printf ("%d ", *matriz);
    matriz += 4;
    printf ("%d ", *matriz);
    matriz += 4;
    printf ("%d ", *matriz);
    matriz += 4;
    printf ("%d\n", *matriz);
    matriz += 4;
    printf ("%d ", *matriz);
    matriz += 4;
    printf ("%d ", *matriz);
    matriz += 4;
    printf ("%d ", *matriz);
    matriz += 4;
    printf ("%d\n\n", *matriz);
}
