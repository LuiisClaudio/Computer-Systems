#include <unistd.h>
#include <stdio.h>

int main(void)
{
  int numero=0, i;
  
  for(i=0;i<50;i++) {
    scanf(" %d", &numero);
    if (numero != 0) {
      usleep(numero*1000);
      printf("%d\n", numero);
    }
  }
  return 0;
}
