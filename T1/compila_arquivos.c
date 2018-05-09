#include <stdlib.h>

int main(void)
{
     system("gcc -o dorminhoco dorminhoco.c");
     system("gcc -o durabastante durabastante.c");
     system("gcc -o infinito infinito.c");
     system("gcc -o teste_mmq teste_mmq.c mmq.c gauss.c matriz.c -lm");
     system("gcc -o interpretador interpretador.c");
     system("gcc -o controle_escalonadores controle_escalonadores.c");
     system("gcc -o escalonadorRR escalonadorRR.c");
     system("gcc -o escalonadorPR escalonadorPR.c");
     system("gcc -o escalonadorRT escalonadorRT.c");
     system("gcc -o limpa_memoria limpa_memoria.c");
     system("./limpa_memoria");
     return 0;
}