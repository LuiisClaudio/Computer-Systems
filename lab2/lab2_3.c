#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


int main(void){
    
    char *aux = NULL;
    char *argv[100];
    char comando[100];
    char raiz[100];
    
    int status;
    int i = 0;
    char *token;
    
    while(1){
        
        //Limpa os comandos
        strcpy(comando, "");
        strcpy(raiz, "");
        
        printf("Diga seu comando: ");
        scanf("%[^\n]%*c", comando);
        
        token = strtok(comando," \"");
        
        while(token != NULL) {
            argv[i] = token;
            i++;
            token = strtok(NULL," \"");
        }
        
        argv[i] = NULL;
        strcat(raiz, argv[0]);
        
        
        if (fork() != 0){
            
            printf("\nPai inicializado \n");
            waitpid(-1,&status,0);
            i = 0;
            printf("\nPai finalizado \n");
        }
        else {
            printf("\nFilho iniciado \n");
            printf("Raiz: %s\n", raiz);
            execvp(raiz, argv);
            printf("\nFilho finalizado \n");
        }
        
    }
    
    return 0;
}
