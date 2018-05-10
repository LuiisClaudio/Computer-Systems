#include <stdlib.h>

int main(void)
{
     system("gcc -o snorlax snorlax.c");
     system("gcc -o demorado demorado.c");
     system("gcc -o nunca_acaba nunca_acaba.c");
     system("gcc -o interpretador interpretador.c");
     system("gcc -o controle_escalonadores controle_escalonadores.c");
     system("gcc -o limpa_memoria limpa_memoria.c");
     system("./limpa_memoria");
     system("rm saidaEscalo*");
     system("rm saidaInter*");
     return 0;
}
